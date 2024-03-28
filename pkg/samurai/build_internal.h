#include <inttypes.h>
#include <stdbool.h>
#include <unistd.h>

#include "graph.h"
#include "util.h"

/* internal use, for sharing between build.c and build.zig */
extern bool consoleused;
extern struct edge *work;
typedef struct poller_s poller_t;
void printstatus(struct edge *e, struct string *cmd);
void edgedone(struct edge *e);
extern size_t nstarted, nfinished, ntotal;
struct job {
	struct string *cmd;
	struct edge *edge;
	struct buffer buf;
	size_t next;
	pid_t pid;
	poller_t* poller;
	bool failed;
};
