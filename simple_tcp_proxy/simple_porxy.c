// $Id: simple-proxy.c,v 1.4 2005/02/25 04:23:24 dean Exp $

//
// simple-proxy.c: simple tcp proxy for async bi-directional data transfer
//
// usage: simple-proxy host port
//
// this will open a socket to host:port and copy data from stdin to the socket,
// and from the socket to stdout.  errors may appear on stderr.
//
// most tcp proxy programs do not properly handle bi-directional data transfer
// -- the work fine with request/response types of protocols, but for streaming
// protocols such as rsync they can easily lock up by blocking on a write().
// simple-proxy doesn't have this problem.
//
// simple-proxy is "simple" because it forks for each direction of data
// transfer... resulting in 3 processes total including the parent.  this is
// the lazy way to accomplish async bi-directional proxying... the not-lazy way
// involves non-blocking i/o and some pesky buffering and state machine code.

// Copyright (c) 2005 Dean Gaudet <dean@arctic.org>
//
// Permission is hereby granted, free of charge, to any person obtaining a
// copy of this software and associated documentation files (the "Software"),
// to deal in the Software without restriction, including without limitation
// the rights to use, copy, modify, merge, publish, distribute, sublicense,
// and/or sell copies of the Software, and to permit persons to whom the
// Software is furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included
// in all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
// THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR
// OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
// ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
// OTHER DEALINGS IN THE SOFTWARE.

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <signal.h>
#include <unistd.h>
#include <errno.h>
#include <sys/wait.h>
#include <ctype.h>

static int
connect_to(const char *host, const char *port)
{
	int s_port;
	struct hostent *hep;
	struct sockaddr_in sa;
	int s;
	const int just_say_no = 1;

	if (isdigit(port[0])) {
		s_port = htons(strtoul(port, 0, 0));
	}
	else {
		struct servent *sep = getservbyname(port, "tcp");
		if (!sep) {
			fprintf(stderr, "service '%s' not found\n", port);
			exit(1);
		}
		s_port = sep->s_port;
	}

	hep = gethostbyname(host);
	if (!hep) {
		fprintf(stderr, "gethostbyname error: %s\n", hstrerror(h_errno));
		exit(1);
	}
	if (hep->h_addrtype != AF_INET) {
		fprintf(stderr, "h_addrtype == %u, and we only support %u (AF_INET)\n",
			hep->h_addrtype, AF_INET);
		exit(1);
	}
	if (!hep->h_addr_list[0]) {
		fprintf(stderr, "no addresses found.\n");
		exit(1);
	}

	memset(&sa, 0, sizeof(sa));
	sa.sin_family = AF_INET;
	sa.sin_addr = *(struct in_addr *)hep->h_addr_list[0];
	sa.sin_port = s_port;

	s = socket(PF_INET, SOCK_STREAM, 0);
	if (s < 0) {
		perror("socket");
		exit(1);
	}

	if (connect(s, (struct sockaddr *)&sa, sizeof(sa)) != 0) {
		perror("connect");
		exit(1);
	}

	// don't even bother reporting problems with disabling nagle
	setsockopt(s, IPPROTO_TCP, TCP_NODELAY, (char *)&just_say_no, sizeof(just_say_no));

	return s;
}


static int
streamer(int from, int to)
{
	char buf[8192];
	ssize_t rc;
	ssize_t amt;
	ssize_t offset;

	while (1) {
		rc = read(from, buf, sizeof(buf));
		if (rc <= 0) {
			if (rc == 0) break;
			if (errno == EINTR || errno == EAGAIN) continue;
			perror("read error");
			return -1;
		}
		offset = 0;
		amt = rc;
		while (amt) {
			rc = write(to, buf+offset, amt);
			if (rc < 0) {
				if (errno == EINTR || errno == EAGAIN) continue;
				perror("write error");
				return -1;
			}
			offset += rc;
			amt -= rc;
		}
	}
	return 0;
}

#define MAX_FD (256)

static pid_t
launch_streamer(int from, int to)
{
	pid_t child;
	int i;

	child = fork();
	if (child < 0) {
		perror("fork");
		exit(1);
	}
	if (child != 0) {
		return child;
	}

	for (i = 0; i < MAX_FD; ++i) {
		if (i != 2 && i != from && i != to) {
			close(i);
		}
	}

	exit(streamer(from, to));
}


int
main(int argc, char **argv)
{
	int s;
	pid_t from_0_to_s;
	pid_t from_s_to_1;
	int status;
	pid_t dead;
	int exit_code;

	if (argc != 3) {
		fprintf(stderr, "usage: %s host port\n", argv[0]);
		exit(1);
	}

	s = connect_to(argv[1], argv[2]);
	if (s > MAX_FD) {
		fprintf(stderr, "socket %d is too large, we support at most %d\n", s, MAX_FD);
		exit(1);
	}

	signal(SIGPIPE, SIG_IGN);

	from_0_to_s = launch_streamer(0, s);
	from_s_to_1 = launch_streamer(s, 1);

	exit_code = 0;
	while ((dead = wait(&status)) != -1) {
		if (dead == from_0_to_s) {
			shutdown(s, 1);
			shutdown(0, 0);
			close(0);
			from_0_to_s = -1;
			if (from_s_to_1 == -1) break;
		}
		else if (dead == from_s_to_1) {
			shutdown(1, 1);
			shutdown(s, 0);
			close(1);
			from_s_to_1 = -1;
			if (from_0_to_s == -1) break;
		}
		else {
			fprintf(stderr, "unknown child came home\n");
			exit_code = 1;
		}
	}
	if (from_0_to_s != -1) {
		kill(from_0_to_s, SIGINT);
		exit_code = 1;
	}
	if (from_s_to_1 != -1) {
		kill(from_s_to_1, SIGINT);
		exit_code = 1;
	}

	return exit_code;
}