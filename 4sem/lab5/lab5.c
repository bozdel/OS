#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define MAX_STR_LEN 50

typedef struct record {
	int shift;
	int leng;
} record;

// returns strings amount
int init_table(record *table, FILE *file) {
	char *buff = (char*)malloc((MAX_STR_LEN + 1) * sizeof(char));

	table[0].shift = 0;
	int i;
	for (i = 1; fgets(buff, MAX_STR_LEN + 1, file) != NULL; i++) {
		int cur_pos = ftell(file);
		table[i].shift = cur_pos;
		table[i - 1].leng = table[i].shift - table[i - 1].shift;
	}

	free(buff);

	return i - 1;
}

void print_str(record *table, int str_num, FILE *file) {
	str_num--;
	fseek(file, table[str_num].shift, SEEK_SET);
	char c;
	for (int i = 0; i < table[str_num].leng; i++) {
		fread(&c, sizeof(char), 1, file);
		fwrite(&c, sizeof(char), 1, stdout);
	}
}

int main(int argc, char *argv[]) {
	FILE *f = NULL;
	if (argc > 1) {
		if ( (f = fopen(argv[1], "r")) == NULL) {
			perror("wrong file");
		}
		struct stat st;
		fstat(fileno(f), &st);
		if (!S_ISREG(st.st_mode)) {
			fclose(f);
			printf("not a regular file\n");
			return EXIT_FAILURE;
		} 
	}
	else {
		printf("missing file\n");
		return EXIT_FAILURE;
	}


	int tb_size = 1000;
	record *table = (record*)malloc(sizeof(record) * tb_size);


	int strings = init_table(table, f);


	int str_num = 0;
	printf("enter string number: ");
	if(scanf("%d", &str_num) != 1) {
		fclose(f);
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
			print_str(table, str_num, f);
		}
		printf("enter string number: ");
		if (scanf("%d", &str_num) != 1) {
			fclose(f);
			printf("enter number\n");
			return EXIT_FAILURE;
		}
	}

	fclose(f);

	return 0;
}
