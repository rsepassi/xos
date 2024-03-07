#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

// https://man7.org/linux/man-pages/man3/ioctl.3p.html
// https://man7.org/linux/man-pages/man2/ioctl_tty.2.html
#include <sys/ioctl.h>
// https://man7.org/linux/man-pages/man3/fcntl.3p.html
#include <fcntl.h>
// https://man7.org/linux/man-pages/man3/termios.3.html
#include <termios.h>

#include "uv.h"
#include "xpty.h"

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

int xpty_start(xpty* pty, const xpty_start_opts opts) {
  if (opts.argv[0] == NULL) return -1;
  if (opts.argv[0][0] != '/') return -1;

  int ptmfd = posix_openpt(O_RDWR | O_NOCTTY);
  if (ptmfd < 0) return -1;
  pty->ptmfd = ptmfd;
  const char* sname = ptsname(pty->ptmfd);
  if (sname == NULL || strlen(sname) == 0) goto err;

  int rc;

  rc = grantpt(ptmfd);
  if (rc != 0) goto err;

  rc = unlockpt(pty->ptmfd);
  if (rc != 0) goto err;

  int ptsfd = open(sname, O_RDWR | O_NOCTTY);
  if (ptsfd < 0) goto err;
  pty->ptsfd = ptsfd;

  rc = fcntl(pty->ptmfd, F_SETFD, FD_CLOEXEC);
  if (rc == -1) goto err;
  rc = fcntl(pty->ptsfd, F_SETFD, FD_CLOEXEC);
  if (rc == -1) goto err;

  struct termios tmode;
  rc = tcgetattr(pty->ptmfd, &tmode);
  if (rc != 0) goto err;

  // TODO: check/document?
  tmode.c_iflag = ICRNL | IXON | IXANY | IMAXBEL | BRKINT | IUTF8;
  tmode.c_oflag = OPOST | ONLCR;
  tmode.c_lflag = ICANON | ISIG | IEXTEN | ECHO | ECHOE | ECHOK | ECHOKE | ECHOCTL;

  tmode.c_cc[VEOF] = 4;
  tmode.c_cc[VEOL] = -1;
  tmode.c_cc[VEOL2] = -1;
  tmode.c_cc[VERASE] = 0x7f;
  tmode.c_cc[VWERASE] = 23;
  tmode.c_cc[VKILL] = 21;
  tmode.c_cc[VREPRINT] = 18;
  tmode.c_cc[VINTR] = 3;
  tmode.c_cc[VQUIT] = 0x1c;
  tmode.c_cc[VSUSP] = 26;
  tmode.c_cc[VSTART] = 17;
  tmode.c_cc[VSTOP] = 19;
  tmode.c_cc[VLNEXT] = 22;
  tmode.c_cc[VDISCARD] = 15;
  tmode.c_cc[VMIN] = 1;
  tmode.c_cc[VTIME] = 0;

#if (__APPLE__)
  tmode.c_cc[VDSUSP] = 25;
  tmode.c_cc[VSTATUS] = 20;
#endif

  cfsetispeed(&tmode, B38400);
  cfsetospeed(&tmode, B38400);

  rc = tcsetattr(pty->ptmfd, TCSANOW, &tmode);
  if (rc != 0) goto err;

  process_state* state = malloc(sizeof(process_state));
  if (state == NULL) goto err;
  process_state state_ = {0};
  state_.pty = pty;
  *state = state_;
  uv_handle_set_data((uv_handle_t*)&state->handle, state);

  uv_process_options_t popts = {0};
  popts.flags = UV_PROCESS_CTTY;
  popts.exit_cb = process_exit_cb;
  popts.file = opts.argv[0];
  popts.args = opts.argv;
  popts.env = opts.env;
  uv_stdio_container_t stdio[3];
  popts.stdio_count = 3;
  stdio[STDIN_FILENO].flags = UV_INHERIT_FD;
  stdio[STDIN_FILENO].data.fd = pty->ptsfd;
  stdio[STDOUT_FILENO].flags = UV_INHERIT_FD;
  stdio[STDOUT_FILENO].data.fd = pty->ptsfd;
  stdio[STDERR_FILENO].flags = UV_INHERIT_FD;
  stdio[STDERR_FILENO].data.fd = pty->ptsfd;
  popts.stdio = stdio;

  rc = uv_spawn(pty->loop, &state->handle, &popts);
  if (rc != 0) goto err;
  pty->pid = state->handle.pid;

  rc = xpty_set_size(pty, opts.size);
  if (rc != 0) goto err;
  pty->size = opts.size;

  rc = uv_pipe_init(pty->loop, &pty->handle, 0);
  if (rc != 0) goto err;
  rc = uv_pipe_open(&pty->handle, pty->ptmfd);
  if (rc != 0) goto err;

  return 0;

err:
  xpty_destroy(pty);
  return -1;
}

int xpty_set_size(xpty* pty, const xpty_size size) {
  struct winsize wins;
  wins.ws_col = size.cols;
  wins.ws_row = size.rows;
  wins.ws_xpixel = 0;
  wins.ws_ypixel = 0;
  int rc;
  rc = ioctl(pty->ptmfd, TIOCSWINSZ, &wins);
  if (rc == -1) return -1;
  rc = ioctl(pty->ptsfd, TIOCSWINSZ, &wins);
  if (rc == -1) return -1;
  pty->size = size;
  return 0;
}

void xpty_destroy(xpty* pty) {
  // Destroy in reverse order of creation
  uv_close((uv_handle_t*)&pty->handle, NULL);

  if (pty->pid > 0) {
    uv_kill(pty->pid, SIGTERM);
    pty->pid = 0;
  }

  if (pty->ptsfd > 0) {
    close(pty->ptsfd);
    pty->ptsfd = 0;
  }
  if (pty->ptmfd > 0) {
    close(pty->ptmfd);
    pty->ptmfd = 0;
  }
}

uv_stream_t* xpty_read_stream(xpty* pty) {
  return (uv_stream_t*)&pty->handle;
}

uv_stream_t* xpty_write_stream(xpty* pty) {
  return (uv_stream_t*)&pty->handle;
}

int xpty_pid(xpty* pty) {
  if (pty->pid == 0) return -1;
  return pty->pid;
}

xpty_size xpty_get_size(xpty* pty) {
  return pty->size;
}
