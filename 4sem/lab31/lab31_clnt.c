#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

#define SOCK_PATH "/tmp/my_sock"
#define LINUX_SOCK_PATH "\0/tmp/my_sock" // abstract socket namespace (only in linux)

// sock - pointer to existed sock, (failed connection)
int reconnect(int *sock, struct sockaddr *addr, socklen_t addrlen) {
	// by SUSv3: close existed, open new, connect
	close(*sock);
	*sock = socket(AF_UNIX, SOCK_STREAM, 0);
	return connect(*sock, addr, sizeof(struct sockaddr_un));
}

void my_connect(int *sock, struct sockaddr *addr, socklen_t addrlen) {
	while (reconnect(sock, (struct sockaddr*)addr, addrlen) == -1) {
		printf("connect failed\n");
		sleep(1);
	}
	printf("connected\n");
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("missing string");
		return EXIT_FAILURE;
	}
	if (argc < 3) {
		printf("missing amount of strings\n");
		return EXIT_FAILURE;
	}

	char *string = argv[1];

	struct sockaddr_un addr;
	memset(&addr, 0, sizeof(struct sockaddr_un));
	addr.sun_family = AF_UNIX;
	strncpy(addr.sun_path, SOCK_PATH, sizeof(addr.sun_path) - 1);
	// strncpy(addr.sun_path, LINUX_SOCK_PATH, sizeof(addr.sun_path) - 1);

	int sockfd = socket(AF_UNIX, SOCK_STREAM, 0);

	/*if (connect(sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un)) == -1) {
		perror("connect failed");
	}*/
	my_connect(&sockfd, (struct sockaddr*)&addr, sizeof(struct sockaddr_un));

	
	int i = 0;
	while (i < atoi(argv[2])) {
		write(sockfd, string, strlen(string));
		sleep(1);
		i++;
	}
	/*char buff[20] = { 0 };
	while (true) {
		printf("enter string: ");
		scanf("%s", buff);
		if (strcmp(buff, "exit") == 0) {
			break;
		}
		write(sockfd, buff, strlen(buff));
	}*/
	close(sockfd);

	return 0;
}