#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>

#define MILSEC 1000

void *thread_run(void *arg) {
	char *str = (char*)arg;
	
	int leng = strlen(str);
	// printf("i'm here\n");
	usleep(leng * MILSEC);

	printf("%s\n", str);
	// printf("i'm broken\n");
	return NULL;
}

int main(int argc, char *argv[]) {
	printf("argc: %d\n", argc);
	pthread_t *thrds = (pthread_t*)malloc(argc * sizeof(pthread_t));
	for (int i = 0; i < argc - 1; i++) {
		pthread_create(&thrds[i], NULL, thread_run, argv[i + 1]);
	}

	for (int i = 0; i < argc - 1; i++) {
		pthread_join(thrds[i], NULL);
	}
	return 0;
}