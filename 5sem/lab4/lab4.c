#include <stdio.h>
#include <pthread.h>
#include <stdbool.h>
#include <unistd.h>

void *thread_run(void *arg) {
	while (true) {
		usleep(1000);
		printf("some text\n");
	}
	return NULL;
}

int main() {
	pthread_t thrd;
	int err = pthread_create(&thrd, NULL, thread_run, NULL);
	if (err > 0) {
		printf("err code: %d\n", err);
	}
	void *res = NULL;
	sleep(2);
	pthread_cancel(thrd); //PTHREAD_CANCEL_ENABLE PTHREAD_CANCEL_DEFERRED
	pthread_join(thrd, &res);

	if ( res == PTHREAD_CANCELED ) {
		printf("thread is canceled\n");
	}

	return 0;
}