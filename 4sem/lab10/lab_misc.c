/*
there should be program that will be called by lab10/11.
it should take arguments following this conditions:
1) if passed argument denoting further sending signal to it. program should switch to infinitily waiting state (sleep(inf) or semething else)
2) if passed argv - print argv
3) if passed envv - print envv
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

extern char **environ;

int main(int argc, char *argv[]) {
	printf("time to dump\n");
	sleep(10);
	printf("time exceeded\n");
	if (argc < 2) {
		printf("missing args\n");
		return EXIT_FAILURE;
	}
	if (strcmp(argv[1], "arg") == 0) {
		for (int i = 0; i < argc; i++) {
			printf("%s\n", argv[i]);
		}
	}
	if (strcmp(argv[1], "env") == 0) {
		for (char **envp = environ; *envp != NULL; envp++) {
			printf("%s\n", *envp);
		}
	}
	if (strcmp(argv[1], "slp") == 0) {
		sleep(100);
	}

	return 0;
}