#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("missing args\n");
		return EXIT_FAILURE;
	}
	pid_t pid = fork();

	printf("my pid: %d\n", pid);

	int status = 0;

	char *envv[] = { NULL };

	switch (pid) {
		case -1:
			perror("fork: ");
			exit(EXIT_FAILURE);
			break;
		case 0: // child
			printf("time to dump\n");
			sleep(10);
			printf("time exceeded\n");
			execve(argv[1], argv + 1, envv);
			perror("execve: ");
			exit(EXIT_FAILURE);
			break;
		default: // parent
			if (wait(&status) == -1) {
				perror("wait: ");
				exit(EXIT_FAILURE);
			}
			break;
	}
	if (WIFEXITED(status)) {
		printf("exit status: %d\n", WEXITSTATUS(status));
	}
	if (WIFSIGNALED(status)) {
		printf("signal number: %d\n", WTERMSIG(status));
	}
	// add some status examing

	return 0; // maybe better to return EXIT_SUCCESS
}