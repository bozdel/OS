#include <stdio.h>
#include <pthread.h>

void print_status() {
	printf("i'm closed\n");
}

void *thread_run(void *arg) {
	pthread_cleanup_push(print_status, NULL);
	int a = 10;
	printf("%d\n", a);
	pthread_cleanup_pop(1);
	// printf("%d\n", a);
	return NULL;
}

int main() {
	pthread_t thrd;

	pthread_create(&thrd, NULL, thread_run, NULL);

	pthread_join(thrd, NULL);

	return 0;
}