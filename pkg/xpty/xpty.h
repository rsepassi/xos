// xpty: x-platform pseudo-tty
//
// All int-returning functions return 0 on success and -1 on error, unless
// otherwise specified.
//
// TODO:
// * error string

#if defined(_WIN32)
#ifndef NTDDI_VERSION 
#define NTDDI_VERSION NTDDI_WIN10_RS5
#endif
#include <winsock2.h>
#include <windows.h>
#include <consoleapi.h>
#endif

#include "uv.h"

// Terminal size
typedef struct {
  int rows;
  int cols;
} xpty_size;

#if defined(_WIN32)
typedef struct {
  uv_loop_t* loop;
  xpty_size size;
  uv_pipe_t handles[2];
  uv_file pipe_fds[2][2];
  HPCON hPC;
  int pid;
} xpty;
#else
typedef struct {
  uv_loop_t* loop;
  xpty_size size;
  uv_pipe_t handle;
  int pid;
  int ptmfd;
  int ptsfd;
} xpty;
#endif

// Initialize an xpty instance.
typedef struct {
  uv_loop_t* loop;
} xpty_init_opts;
int xpty_init(const xpty_init_opts opts, xpty* pty);

// Destory an xpty instance and its associated resources, including the
// executing process.
void xpty_destroy(xpty* pty);

// Start a process under a pty
typedef struct {
  char** argv;
  char** env;
  xpty_size size;
} xpty_start_opts;
int xpty_start(xpty* pty, const xpty_start_opts opts);

// Get the pid of the started process.
int xpty_pid(xpty* pty);

// Get the current pty size
xpty_size xpty_get_size(xpty* pty);
// Set the current pty size
int xpty_set_size(xpty* pty, const xpty_size size);

// Get the pty streams
uv_stream_t* xpty_read_stream(xpty* pty);
uv_stream_t* xpty_write_stream(xpty* pty);
