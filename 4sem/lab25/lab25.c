#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <ctype.h>
#include <sys/mman.h>

#define R 0
#define W 1

int main(int argc, char *argv[]) {
	pid_t pid1;
	pid_t pid2;
	char *string = NULL;
	if (argc < 2) {
		printf("missing string\n");
		return EXIT_FAILURE;
	}
	string = argv[1];


	int fds[2];
	if (pipe(fds) == -1) {
		perror("pipe failed");
		return EXIT_FAILURE;
	}

	/*char *memmap = NULL;
	if ((memmap = mmap(NULL, 4096, PROT_READ | PROT_WRITE, MAP_SHARED, fds[R], 0)) == NULL) {
		perror("mmap failed");
		return EXIT_FAILURE;
	}*/

	pid1 = fork();
	switch (pid1) {
		case -1:
			perror("fork failed");
			return EXIT_FAILURE;
		case 0: // 1-st child
			close(fds[R]);
			
			write(fds[W], string, strlen(string));
			/*int leng = strlen(string);
			for (int i = 0; i < leng; i++) {
				sleep(1);
				write(fds[W], string + i, 1);
			}*/

			return 0;
		default: //parent
			break;
	}

	pid2 = fork();
	switch (pid2) {
		case -1:
			perror("fork failed");
			return EXIT_FAILURE;
		case 0: // 2-nd child
			close(fds[W]);
			// close(fds[R]);

			char c;
			/*sleep(1);
			while (read(fds[R], &c, 1)) {
				usleep(100000);
				write(STDOUT_FILENO, &c, 1);
				write(STDOUT_FILENO, " ", 1);
			}*/
			/*for (int i = 0; memmap[i] != 0; i++) {
				write(STDOUT_FILENO, memmap + i, 1);
			}*/

			//lab
			while (read(fds[R], &c, 1)) {
				c = toupper(c);
				write(STDOUT_FILENO, &c, 1);
			}
			printf("\ni'm exiting\n");
			return 0;
		default: // parent
			break;
	}
	
	
	// close R, W in child 2

	/*//why didn't print signal if uncommented?
	close(fds[R]); // what happen if commented?
	char c;
	for (int i = 0; i < 5; i++) {
		c = rand() % 20 + ('a');
		sleep(1);
		write(fds[W], &c, 1);
		printf("written %c in pipe\n", c);
	}*/

	close(fds[W]);
	// didn't work if fds[W] isn't closed
	int i = 0;
	char c;
	while (read(fds[R], &c, 1)) {
		if (i % 2 == 0) {
			usleep(100000);
			write(STDOUT_FILENO, " ", 1);
		}
		i++;
		write(STDOUT_FILENO, &c, 1);
	}



	close(fds[W]);
	close(fds[R]);
	int status1 = 0;
	int status2 = 0;
	if (wait(&status1) == -1) {
		perror("wait failed");
		return EXIT_FAILURE;
	}
	if (wait(&status2) == -1) {
		perror("wait failed");
		return EXIT_FAILURE;
	}
	if (WIFEXITED(status1)) {
		printf("exit status: %d\n", WEXITSTATUS(status1));
	}
	if (WIFSIGNALED(status1)) {
		printf("signal number: %d\n", WTERMSIG(status1));
	}

	if (WIFEXITED(status2)) {
		printf("exit status: %d\n", WEXITSTATUS(status2));
	}
	if (WIFSIGNALED(status2)) {
		printf("signal number: %d\n", WTERMSIG(status2));
	}

	

	return 0;
}