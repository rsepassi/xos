// Write randombytes to stdout
#include <stdio.h>
#include <stdlib.h>

#include "randombytes.h"

int main(int argc, char** argv) {
	const int BUFSIZE = 1024;
	const int limit = argc == 2 ? atoi(argv[1]) : -1;
	char buf[BUFSIZE];

	int n = 0;
	while (1) {
			int res = randombytes(buf, BUFSIZE);
      if (res != 0) break;

			int bytes_written = fwrite(buf, 1, BUFSIZE, stdout);
			if (bytes_written < 1 || ferror(stdout)) break;
			n += bytes_written;

			if (limit >= 0 && n >= limit) break;
	}

	return 0;
}

