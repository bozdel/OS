#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
	char **strings;
	int thrd_num;
	int size;
} strings;

void init_str(char **string, int size) {
	char *tmp = (char*)malloc(size * sizeof(char));
	for (int i = 0; i < size - 1; i++) {
		tmp[i] = rand() % 25 + 'a';
	}
	tmp[size - 1] = '\0';
	*string = tmp;
}

strings init_strings(int size, int num) {
	strings strs;
	strs.thrd_num = num;
	strs.size = size;
	strs.strings = (char**)malloc(size * sizeof(char*));
	for (int i = 0; i < size; i++) {
		int str_size = rand() % 10 + 3;
		init_str(&(strs.strings[i]), str_size);
	}
	return strs;
}

void free_strings(strings strs) {
	for (int i = 0; i < strs.size; i++) {
		free(strs.strings[i]);
	}
}

void print_strings(strings strs) {
	for (int i = 0; i < strs.size; i++) {
		printf("thread num: %d string: %s\n", strs.thrd_num, strs.strings[i]);
	}
	printf("\n");
}

void *threadrun(void *arg) {
	strings *strs = (strings *)arg;
	print_strings(*strs);
	return NULL;
}

#define THREAD_NUM 4

int main() {
	pthread_t tids[THREAD_NUM];
	strings *strs_arr = (strings*)malloc(THREAD_NUM * sizeof(strings));
	for (int i = 0; i < THREAD_NUM; i++) {
		strs_arr[i] = init_strings(rand() % 10, i);
	}
	for (int i = 0; i < THREAD_NUM; i++) {
		int err = pthread_create(&tids[i], NULL, threadrun, &strs_arr[i]);
	}

	for (int i = 0; i < THREAD_NUM; i++) {
		pthread_join(tids[i], NULL);
	}

	return 0;
}