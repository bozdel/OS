#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

#define BACKLOG 5
#define SOCK_PATH "/tmp/my_sock"
#define LINUX_SOCK_PATH "\0/tmp/my_sock" // abstract socket namespace (only in linux)

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

	if (access(SOCK_PATH, F_OK) == 0) { // file already exist
		unlink(SOCK_PATH);
	}
	if (bind(lsockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
		perror("bind failed");
	}

	if (listen(lsockfd, BACKLOG) == -1) {
		perror("listen failed");
	}

	for (;;) {
		int sockfd;
		if ((sockfd = accept(lsockfd, NULL, 0)) == -1) {
			perror("accept failed");
		}

		char c;
		while (read(sockfd, &c, sizeof(char))) {
			c = toupper(c);
			write(STDOUT_FILENO, &c, 1);
		}

		close(sockfd);
	}

	// unlink(SOCK_PATH);

	return 0;
}