#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "xpty.h"
#include "uv.h"

#define CHECK(cond) \
  do { \
    if (!(cond)) { \
      exit(1); \
    } \
  } while(0)

#define CHECK0(cond) \
  CHECK((cond) == 0)

void alloc_cb(uv_handle_t* handle, size_t n, uv_buf_t* buf) {
  static char read_buf[2048];
  *buf = uv_buf_init(read_buf, sizeof(read_buf));
}

void read_cb(uv_stream_t* handle, ssize_t n, const uv_buf_t* buf) {
  printf("read %ld bytes\n", n);
  printf("-S--\n");
  if (n > 0) {
    printf("%.*s", (int)buf->len, buf->base);
  }
  printf("-E--\n");
}

void write_cb(uv_write_t* handle, int status) {
  if (status == 0) printf("write success\n");
  else printf("write fail\n");
}

void timer_cb(uv_timer_t* handle) {
  xpty* pty = (xpty*)uv_handle_get_data(handle);
  uv_read_stop(xpty_read_stream(pty));
  xpty_destroy(pty);
  uv_timer_stop(handle);
}

int main(int argc, char** argv) {
	uv_loop_t* loop = malloc(sizeof(uv_loop_t));
	uv_loop_init(loop);

  xpty pty = {0};
  xpty_init_opts init_opts = {.loop = loop};
  CHECK0(xpty_init(init_opts, &pty));

  char* xargv[2] = {
    "/bin/sh",
    NULL,
  };
  xpty_start_opts start_opts = {
    .argv = xargv,
    .env = NULL,
    .size = {
      .rows = 80,
      .cols = 80,
    },
  };
  CHECK0(xpty_start(&pty, start_opts));

  uv_stream_t* read_stream = xpty_read_stream(&pty);
  uv_stream_t* write_stream = xpty_write_stream(&pty);
  CHECK0(uv_read_start(read_stream, alloc_cb, read_cb));
  uv_timer_t timer = {0};
  uv_timer_init(loop, &timer);
  uv_handle_set_data((uv_handle_t*)&timer, &pty);
  uv_timer_start(&timer, timer_cb, 1000, 0);

  uv_write_t write = {0};
  uv_buf_t bufs[1];
  char* buf = "ls\r";
  bufs[0] = uv_buf_init(buf, strlen(buf));
  CHECK0(uv_write(&write, write_stream, bufs, 1, write_cb));

	uv_run(loop, UV_RUN_DEFAULT);

	uv_loop_close(loop);
	free(loop);
	return 0;
}
