#include <errno.h>
#include <stdio.h>
#include <pthread.h>
#include <string.h>

#define MTX_IND(i) ( (2 * i) % 3 )

static pthread_mutex_t mtx[3];
static pthread_mutexattr_t attr;

void *print_str(void *arg) {
	pthread_mutex_lock(&mtx[MTX_IND(0)]);
	int id = (int)arg;
	pthread_t tid = pthread_self();
	for (int i = 0; i < 50; i++) {
		// int mtx_ind = (2 * i) % 3;
		printf("id: %d; tid: %ld; string: %d\n", id, (long)tid, i);
		pthread_mutex_lock(&mtx[MTX_IND(i + 1)])
		pthread_mutex_unlock(&mtx[MTX_IND(i)]);
	}
}

int main() {
	int s = 0;
	for (int i = 0; i < 3; i++) {
		s = pthread_mutex_init(&mtx[i], NULL);
		if (s != 0) {
			fprintf(stderr, "error: mtx init\n");
		}
	}
	// just 2 (seq: 1 0 2 ...)
	pthread_mutex_lock(&mtx[MTX_IND(2)]);

	pthread_t thread;
	printf("i'm parent, my tid is: %ld\n", pthread_self());
	errno = pthread_create(&thread, NULL, print_str, (void*)1);
	if (errno != 0) {
		perror("pthread_create");
	}
	sleep(1);
	for (int i = 0; i < 50; i++) {
		printf("parent, id: %d\n", i);
		pthread_mutex_lock(&mtx[MTX_IND(i + 1 + 2)])
		pthread_mutex_unlock(&mtx[MTX_IND(i + 2)]);
	}
	// print_str(0);

	pthread_join(thread, NULL);

	return 0;
}

/*int main() {
	int s = pthread_mutexattr_init(&attr);
	if (s != 0) {
		fprintf(stderr, "attr init\n");
		// return EXIT_FAILURE;
	}
	s = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_ERRORCHECK);
	if (s != 0) {
		fprintf(stderr, "set type\n");
		// return EXIT_FAILURE;
	}
	s = pthread_mutex_init(&A, &attr);
	if (s != 0) {
		fprintf(stderr, "mtx init\n");
		// return EXIT_FAILURE;
	}
	s = pthread_mutex_init(&B, &attr);
	if (s != 0) {
		fprintf(stderr, "mtx init\n");
		// return EXIT_FAILURE;
	}

	pthread_mutex_lock(&A);

	pthread_t thread;
	printf("i'm parent, my tid is: %ld\n", pthread_self());
	errno = pthread_create(&thread, NULL, print_str, (void*)1);
	if (errno != 0) {
		perror("pthread_create");
	}
	print_str(0);

	s = pthread_mutexattr_destroy(&attr);
	if (s != 0) {
		fprintf(stderr, "attr destroy\n");
		// return EXIT_FAILURE;
	}

	pthread_join(thread, NULL);

	return 0;
}*/