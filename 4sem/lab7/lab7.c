#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#define MAX_STR_LEN 50

typedef struct record {
	int shift;
	int leng;
} record;

// returns strings amount
int init_table(record *table, char *memmap) {
	int rec_num = 0;
	char delim = '\n';
	char *string = strtok(memmap, &delim);
	table[0].shift = 0;
	table[0].leng = strlen(string);
	rec_num++;

	while ( (string = strtok(NULL, &delim)) != NULL ) {
		table[rec_num].leng = strlen(string);
		table[rec_num].shift = table[rec_num - 1].shift + table[rec_num - 1].leng + 1;
		rec_num++;
	}

	return rec_num;
}

void print_str(record *table, int str_num, char *memmap) {
	str_num--;
	int shift = table[str_num].shift;
	printf("%s\n", memmap + shift);
}

/*void parse(char *string) {
	char delim = '\n';
	char *token = strtok(string, &delim);
	printf("%s (%ld)\n\n", token, strlen(token));
	while ( (token = strtok(NULL, &delim)) != NULL) {
		printf("%s (%ld)\n\n", token, strlen(token));
	}
}*/

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("missing file\n");
		return EXIT_FAILURE;
	}

	int fd;
	char *memmap = NULL;

	if ((fd = open(argv[1], O_RDONLY)) == -1) {
		perror("open failed");
		return EXIT_FAILURE;
	}

	struct stat st;
	fstat(fd, &st);
	size_t leng = st.st_size;

	if (leng == 0) {
		printf("file is empty\n");
		close(fd);
		return EXIT_FAILURE;
	}
	if (!S_ISREG(st.st_mode)) {
		close(fd);
		printf("not a regular file\n");
		return EXIT_FAILURE;
	} 

	if ((memmap = mmap(NULL, leng, PROT_READ | PROT_WRITE, MAP_PRIVATE, fd, 0)) == NULL) {
		perror("mmap failed");
		return EXIT_FAILURE;
	}
	close(fd);

	int tb_size = 1000;
	record *table = (record*)malloc(sizeof(record) * tb_size);


	int strings = init_table(table, memmap);
	
	
	int str_num = 0;
	printf("enter string number: ");
	if(scanf("%d", &str_num) != 1) {
		munmap(memmap, leng);
		printf("enter number\n");
		return EXIT_FAILURE;
	}
	while (str_num != 0) {
		if (str_num > strings) {
			printf("error: max string number - %d\n", strings);
		}
		else if (str_num < 0) {
			printf("you are fool\n");
		}
		else {
			print_str(table, str_num, memmap);
		}
		printf("enter string number: ");
		if (scanf("%d", &str_num) != 1) {
			munmap(memmap, leng);
			printf("enter number\n");
			return EXIT_FAILURE;
		}
	}

	return 0;
}