#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <errno.h>

#include <sys/time.h>
#include <sys/select.h>

#include <fcntl.h>

#define BACKLOG 5
#define SOCK_PATH "/tmp/my_sock"
#define LINUX_SOCK_PATH "\0/tmp/my_sock" // abstract socket namespace (only in linux)

#define MAX(a,b) ((a) > (b) ? (a) : (b))

/*
remove = unlink if reg file, rmdir if dir
unlink for removing hard links
*/



int main() {
	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);
	// strncpy(addr.sun_path, LINUX_SOCK_PATH, sizeof(addr.sun_path) - 1);

	int lsockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	//making listen socket non-blocking
	int flags;
	if ( (flags = fcntl(lsockfd, F_GETFL)) == -1 ) {
		close(lsockfd);
		return EXIT_FAILURE;
	}
	flags |= O_NONBLOCK;
	if (fcntl(lsockfd, F_SETFL, flags) == -1) {
		close(lsockfd);
		return EXIT_FAILURE;
	}


	if (access(SOCK_PATH, F_OK) == 0) { // file already exist
		unlink(SOCK_PATH);
	}
	if (bind(lsockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
		perror("bind failed");
	}

	if (listen(lsockfd, BACKLOG) == -1) {
		perror("listen failed");
	}

	int nfds = 0;
	fd_set readfds;
	FD_ZERO(&readfds);
	
	int socks[10] = { 0 };
	for (int i = 0; i < 10; i++) {
		socks[i] = -1;
	}

	struct timeval tm;
	tm.tv_sec = 0;
	tm.tv_usec = 0;
	int ind = 0;
	int maxfd = 0;

	int prev = 0;
	for (;;) {
		int sockfd;
		if ((sockfd = accept(lsockfd, NULL, 0)) == -1) {
			if (errno != EAGAIN) {
				perror("accept failed");
				printf("errno: %d\n", errno);
			}
		}
		else {
			socks[ind] = sockfd;
			ind++;
			// printf("amount: %d\n", ind);
		}

		if (maxfd <= sockfd) {
			maxfd = sockfd + 1;
		}
		

		char c;
		int r;

		FD_ZERO(&readfds);
		for (int i = 0; i < 10; i++) {
			if (socks[i] != -1) {
				FD_SET(socks[i], &readfds);
			}
		}
		
		int num = select(maxfd, &readfds, NULL, NULL, &tm); // ready only if i/o will not block regardless something will be r/w
		if (num != prev) {
			// printf("ready: %d\n", num);
			prev = num;
		}

		for (int i = 0; i < ind; i++) {
			if (FD_ISSET(socks[i], &readfds)) {
				int r = -1;
				if (read(socks[i], &c, sizeof(char)) != 0) {
					write(STDOUT_FILENO, &c, 1);
				}
				else {
					printf("connection closed\n");
					close(socks[i]);
					socks[i] = -1;
				}

			}
		}


	}

	// unlink(SOCK_PATH);

	return 0;
}