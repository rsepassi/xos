#include <stdio.h>

#include "xpty.h"
#include "uv.h"

typedef struct {
  xpty* pty;
  uv_process_t handle;
} process_state;

static void process_exit_cb(uv_process_t* handle, int64_t exit_status, int term_signal) {
  process_state* state = (process_state*)uv_handle_get_data((uv_handle_t*)handle);
  uv_close((uv_handle_t*)handle, NULL);
  free(state);
}

int xpty_init(const xpty_init_opts opts, xpty* pty) {
  const xpty x = {0};
  *pty = x;
  pty->loop = opts.loop;
  return 0;
}

static int setup_cb(uv_handle_t* handle, void* data) {
  process_state* state = (process_state*)uv_handle_get_data((uv_handle_t*)handle);
  STARTUPINFOEXW* startup = (STARTUPINFOEXW*)data;
  if (!UpdateProcThreadAttribute(
        startup->lpAttributeList,
        0,
        PROC_THREAD_ATTRIBUTE_PSEUDOCONSOLE,
        state->pty->hPC,
        sizeof(HPCON),
        NULL,
        NULL)) {
    return -1;
  }
  return 0;
}

int xpty_start(xpty* pty, const xpty_start_opts opts) {
  if (opts.argv[0] == NULL) return -1;

  int rc;
  // Create input and output pipes
  rc = uv_pipe(pty->pipe_fds[0], 0, 0);
  if (rc != 0) goto err;
  rc = uv_pipe(pty->pipe_fds[1], 0, 0);
  if (rc != 0) goto err;

  HRESULT hr = S_OK;
  COORD consize = {opts.size.cols, opts.size.rows};
  uv_file in_read = pty->pipe_fds[1][0];
  uv_file out_write = pty->pipe_fds[0][1];
  hr = CreatePseudoConsole(
      consize,
      uv_get_osfhandle(in_read),
      uv_get_osfhandle(out_write),
      0,
      &pty->hPC);
  if (FAILED(hr)) goto err;

  process_state* state = malloc(sizeof(process_state));
  if (state == NULL) goto err;
  process_state state_ = {0};
  state_.pty = pty;
  *state = state_;
  uv_handle_set_data((uv_handle_t*)&state->handle, state);

  uv_process_options_t popts = {0};
  popts.flags = UV_PROCESS_CTTY | UV_PROCESS_WINDOWS_HIDE_CONSOLE | UV_PROCESS_WINDOWS_HIDE_GUI;
  popts.exit_cb = process_exit_cb;
  popts.file = opts.argv[0];
  popts.args = opts.argv;
  popts.env = opts.env;
  uv_stdio_container_t stdio[3];
  popts.stdio_count = 3;
  stdio[STDIN_FILENO].flags = UV_INHERIT_FD;
  stdio[STDIN_FILENO].data.fd = in_read;
  stdio[STDOUT_FILENO].flags = UV_INHERIT_FD;
  stdio[STDOUT_FILENO].data.fd = out_write;
  stdio[STDERR_FILENO].flags = UV_INHERIT_FD;
  stdio[STDERR_FILENO].data.fd = out_write;
  popts.stdio = stdio;
  popts.setup_cb = setup_cb;

  rc = uv_spawn(pty->loop, &state->handle, &popts);
  if (rc != 0) goto err;
  pty->pid = state->handle.pid;

  pty->size = opts.size;

  rc = uv_pipe_init(pty->loop, &pty->handles[0], 0);
  if (rc != 0) goto err;
  rc = uv_pipe_init(pty->loop, &pty->handles[1], 0);
  if (rc != 0) goto err;

  rc = uv_pipe_open(&pty->handles[0], pty->pipe_fds[0][0]);
  if (rc != 0) goto err;
  rc = uv_pipe_open(&pty->handles[1], pty->pipe_fds[1][1]);
  if (rc != 0) goto err;

  uv_stream_set_blocking(&pty->handles[0], 1);
  if (rc != 0) goto err;
  uv_stream_set_blocking(&pty->handles[1], 1);
  if (rc != 0) goto err;

  return 0;

err:
  xpty_destroy(pty);
  return -1;
}

int xpty_set_size(xpty* pty, const xpty_size size) {
  COORD consize;
  consize.X = size.cols;
  consize.Y = size.rows;
  ResizePseudoConsole(pty->hPC, consize);
  pty->size = size;
  return 0;
}

void xpty_destroy(xpty* pty) {
  // Destroy in reverse order of creation

  for (int i = 0; i < sizeof(pty->handles); ++i) {
    uv_close((uv_handle_t*)&pty->handles[i], NULL);
  }

  if (pty->pid > 0) {
    uv_kill(pty->pid, SIGTERM);
    pty->pid = 0;
  }

  ClosePseudoConsole(pty->hPC);

  for (int i = 0; i < sizeof(pty->pipe_fds); ++i) {
    for (int j = 0; j < sizeof(pty->pipe_fds[0]); ++j) {
      CloseHandle(uv_get_osfhandle(pty->pipe_fds[i][j]));
    }
  }
}

uv_stream_t* xpty_read_stream(xpty* pty) {
  return (uv_stream_t*)&pty->handles[0];
}

uv_stream_t* xpty_write_stream(xpty* pty) {
  return (uv_stream_t*)&pty->handles[1];
}

int xpty_pid(xpty* pty) {
  if (pty->pid == 0) return -1;
  return pty->pid;
}

xpty_size xpty_get_size(xpty* pty) {
  return pty->size;
}
