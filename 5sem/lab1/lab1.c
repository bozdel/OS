#include <errno.h>
#include <stdio.h>
#include <pthread.h>

void *print_str() {
	pthread_t tid = pthread_self();
	for (int i = 0; i < 50; i++) {
		printf("tid: %ld; string: %d\n", (long)tid, i);
	}
}

int main() {
	pthread_t thread;
	printf("i'm parent, my tid is: %ld\n", pthread_self());
	errno = pthread_create(&thread, NULL, print_str, NULL);
	if (errno != 0) {
		perror("pthread_create");
	}
	print_str();
	return 0;
}