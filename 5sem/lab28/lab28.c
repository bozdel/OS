#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <termios.h>
#include <poll.h>
#include <stdbool.h>
#include <fcntl.h> // remove (it's for test)
#include "url_parser.h"

#define CONCLOSED -2
#define NOMEM -1

#define SOCK 1
#define STDIN 0

#define BLOCK -1 // for poll call

#define MB 1000000

struct html_page {
	char *data;
	int size;
	int r_pos;
	int w_pos;
};

struct termios tp;

void set_old_mode() {
	if (tcsetattr(STDIN_FILENO, TCSANOW, &tp) == -1) {
		perror("tcsetattr, set_old_mode");
	}
}

void set_new_mode() {
	if (tcgetattr(STDIN_FILENO, &tp) == -1) {
		perror("tcgetattr");
	}
	struct termios tmp_tp;
	memcpy(&tmp_tp, &tp, sizeof(struct termios));
	tmp_tp.c_lflag &= ~(ICANON);
	tmp_tp.c_cc[VMIN] = 1;
	tmp_tp.c_cc[VTIME] = 0;
	if (tcsetattr(STDIN_FILENO, TCSANOW, &tmp_tp) == -1) {
		perror("tcsetattr, set_new_mode");
	}
}

struct html_page init_page(int size) {
	struct html_page page;
	page.data = (char*)calloc(size, sizeof(char));
	page.size = size;
	page.w_pos = 0;
	page.r_pos = 0;
	return page;
}

void free_html_page(struct html_page page) {
	free(page.data);
}
#include <assert.h>
int print_str(struct html_page *page, FILE *tty_page) {
	char line[81] = { 0 };
	char *data = page->data;
	int pos = page->r_pos;
	int i = 0;
	printf("%d\n", page->w_pos);
	bool is_cr = false;
	bool is_end = false;
	bool str_done = false;
	bool no_data = false;
	while (!is_cr && !is_end && !str_done && !no_data) {
		assert(i < 80);
		line[i] = data[pos + i];
		i++;

		is_cr = data[pos + i] == '\n';
		is_end = data[pos + i] == '\0';
		str_done = i >= 80;
		no_data = pos + i > page->w_pos;
	}
	printf("is_cr: %s, is_end: %s, std_done: %s, no_data: %s\n",
		is_cr ? "true" : "false",
		is_end ? "true" : "false",
		str_done ? "true" : "false",
		no_data ? "true" : "false");
	printf("here\n");
	printf("pos + i: %d\npage->w_pos: %d\n", pos + i, page->w_pos);
/*
	for (i = 0; i < 80 && data[pos + i] != '\n' && data[pos + i] !='\0'; i++) {
		line[i] = data[pos + i];
	}
*/
	fprintf(tty_page, "%s\n", line);
	// possibly due to noncanonical mode outbuffer equal to one byte, so if sending multiple they devided by one and
	// inserting \n between. check this
	// for (int i = 0; i < 81; i++) fprintf(tty_page, "%c", line[i]);
	// fprintf(tty_page, "\n");
	// printf("nodata: %s\n", no_data ? "true" : "false");
	page->r_pos += i;
	if (no_data) return -1;
	return 0;
}

int print_25(struct html_page *page, FILE *errf) {
	int res = 0;
	int str_num = 0;
	for (int i = 0; i < 25; i++) {
		fprintf(errf, "(string: %d) ", i);
		if (print_str(page, errf) == -1) {
			fprintf(errf, "no data\n");
			return -1;
		}
	}
	return 0;
}

int recv_page_seg(int sockfd, struct html_page *page) {
	char buff[100];
	int recvd = recv(sockfd, buff, 100, 0);
	printf("received: %d\n", recvd);
	if (recvd == 0) return CONCLOSED;
	int pos = page->w_pos;
	if (pos + recvd > page->size) return NOMEM;
	memcpy((page->data) + pos, buff, recvd);
	page->w_pos += recvd;
	return 0;
}

struct addrinfo get_hints(int ai_flags, int ai_family, int ai_socktype, int ai_protocol) {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_flags = ai_flags;
	hints.ai_family = ai_family;
	hints.ai_socktype = ai_socktype;
	hints.ai_protocol = ai_protocol;
	return hints;
}

/*

*/

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("missing url\n");
		return EXIT_FAILURE;
	}

	atexit(set_old_mode);
	set_new_mode();

	struct url url;
	parse_url(argv[1], &url);

	struct addrinfo *addrs = NULL;
	// 3rd arg - 0 (sock_dgram/sock_stream)
	struct addrinfo hints = get_hints(0, AF_UNSPEC, 0, 0);

	// int err = getaddrinfo(argv[1], "http", &hints, &addrs);
	int err = getaddrinfo(url.host, url.scheme, &hints, &addrs);
	if (err != 0) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
		// free addrs by freeaddrinfo???
		return EXIT_FAILURE;
	}


	int sockfd;
	for (struct addrinfo *addri = addrs; addri != NULL; addri = addri->ai_next) {
		sockfd = socket(addri->ai_family, addri->ai_socktype, addri->ai_protocol);
		if (sockfd == -1) continue;

		if (connect(sockfd, addri->ai_addr, addri->ai_addrlen) == 0) {
			printf("connected\n");
			break;
		}
		printf("connection failed\n");
		close(sockfd);
	}

	char req[] = "GET /\n";
	int req_len = strlen(req);

	send(sockfd, req, req_len, 0);


	struct html_page page = init_page(1 * MB);

	char buff[100] = { 0 };


	struct pollfd fds[2] = {{STDIN_FILENO, POLLIN, 0}, {sockfd, POLLIN, 0}};


	FILE *tty = fopen("/dev/pts/2", "w");
	FILE *tty_page = fopen("/dev/pts/5", "w");
	if (tty == NULL) {
		perror("fopen tty");
		return -1;
	}
	if (tty_page == NULL) {
		perror("fopen tty page");
		return -1;
	}

	
	

	while (true) {
		if (poll(fds, 2, BLOCK) == -1) {
			perror("poll");
			// free resources
			return EXIT_FAILURE;
		}
		if (fds[STDIN].revents & POLLIN) {
			char ch;
			read(STDIN_FILENO, &ch, sizeof(char));
			fprintf(tty, "read\n");
			int res = print_25(&page, tty_page);
		}
		/*if (fds[SOCK].revents & POLLHUP) { // might be working if read some more books
			// fds[SOCK].events = 0;
			printf("registered POLLHUP\n");
		}*/
		if (fds[SOCK].revents & POLLIN) {
			usleep(1000);
			int ret;
			if ( (ret = recv_page_seg(sockfd, &page)) != 0) {
				if (ret == NOMEM) {
					fprintf(stderr, "error recv_page_seg: not enough mem\n");
					// free resourses
					return EXIT_FAILURE;
				}
				if (ret == CONCLOSED) {
					fds[SOCK].fd *= -1;
					printf("sock discarderd\n");
					close(sockfd);
				}
			}
		}
	}

	return 0;
}
