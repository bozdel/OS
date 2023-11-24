#include <unistd.h>
// #include <strings.h> there is index (as man says). but on linux it works good even without it
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define OVERWRITE 1


extern char **environ;

/*
putenv takes char *string as arg, but arg won't copy to somewhere in eviron vice versa one of pointer from env vec will refer to this string
*/

/*
clearenv not in SUSv3 (undefined behavior is used). you can unsetenv for each element in eviron (SUSv3 approoved:D)
*/


int myexecvpe(char const *file, char *const argv[], char *envv[]) {

	// unsetting current environ
	clearenv();

	// unsetting current environ
	/*char env_varname[100] = { 0 };
	while (*environ) {
		char *namep = env_varname;
		char *env_varp = *environ;
		while ( (*namep++ = *env_varp++) != '=' ) ;
		*(namep - 1) = '\0';
		if (unsetenv(env_varname) == -1) {
			perror("unsetenv: ");
			return -1;
		}
	}*/


	// setting new environ
	char *tmpp = NULL;
	char *name = NULL;
	char *value = NULL;
	for (char **p = envv; *p != NULL; p++) {

		tmpp = (char*)realloc(tmpp, strlen(*p + 1) * sizeof(char));
		strcpy(tmpp, *p);

		name = strtok(tmpp, "=");
		value = strtok(NULL, "=");
		
		if (setenv(name, value, OVERWRITE) == -1) {
			perror("setenv: ");
			return -1;
		}
	}

	free(tmpp);

	return execvp(file, argv);
}

int main(int argc, char *argv[]) {
	if (argc < 2) {
		printf("missing args\n");
		return EXIT_FAILURE;
	}

	char *envv[] = {"var1=value1", "var2=value2", "var3=value3", "var4=value4", NULL};

	if (myexecvpe(argv[1], argv + 1, envv) == -1) {
		printf("error in myexecvpe\n");
		return EXIT_FAILURE;
	}

	return 0;
}