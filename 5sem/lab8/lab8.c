#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define INTERS_NUM 1000

typedef struct {
	int from; // inclusively
	int to; // exclusively
	double res;
} series_segment;

void *calc_seg(void *arg) {
	double tmp_res = 0.0;
	series_segment *seg =(series_segment*)arg;
	for (int i = seg->from; i < seg->to; i++) {
		tmp_res += 1.0 / (i * 4.0 + 1.0);
		tmp_res -= 1.0 / (i * 4.0 + 3.0);
	}
	tmp_res *= 4;
	seg->res = tmp_res;
	return NULL;
}

void init_segments(series_segment *segs, int size) {
	int seg_leng = INTERS_NUM / size;
	for (int i = 0; i < size; i++) {
	segs[i].from = i * seg_leng;
	segs[i].to = (i + 1) * seg_leng;
	segs[i].res = 0.0;
	}
	segs[size - 1].to = INTERS_NUM;
}

void print_segs(series_segment *segs, int size) {
	for (int i = 0; i < size; i++) {
	printf("[%d, %d): %f\n", segs[i].from, segs[i].to, segs[i].res);
	}
}

int main(int argc, char *argv[]) {
	int threads_num = 1;
	series_segment *segs = NULL;
	pthread_t *thrds = NULL;

	int opt = 0;
	while ( (opt = getopt(argc, argv, "n:")) != -1) {
		switch (opt) {
			case 'n':
				threads_num = atoi(optarg);
				break;
			default:
				printf("no thread number\n");
				break;
		}
	}
	if ( (segs = (series_segment*)malloc(threads_num * sizeof(series_segment))) == NULL ) {
		perror("allocing segs");
		return EXIT_FAILURE;
	}
	init_segments(segs, threads_num);

	if ( (thrds = (pthread_t*)malloc(threads_num * sizeof(pthread_t))) == NULL) {
		perror("allocing threads");
		free(segs);
		return EXIT_FAILURE;
	}
	for (int i = 0; i < threads_num; i++) {
		int err = pthread_create(&thrds[i], NULL, calc_seg, (void*)&segs[i]);
		if (err != 0) fprintf(stderr, "%s", strerror(err));
	}

	double result = 0.0;
	for (int i = 0; i < threads_num; i++) {
		int err = pthread_join(thrds[i], NULL);
		if (err != 0) fprintf(stderr, "%s", strerror(err));
		result += segs[i].res;
	}

	// print_segs(segs, threads_num);

	printf("%f\n", result);

	free(thrds);
	free(segs);
	return 0;
}
