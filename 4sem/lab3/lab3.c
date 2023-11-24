#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	char *path = NULL;

	if (argc > 1) path = argv[1];

	if (path) printf("%s\n", path);

	//always successful
	uid_t usrid = getuid(); // /etc/passwd
	gid_t grpid = getgid(); // /etc/group
	printf("user id: %d, group id: %d\n", usrid, grpid);

	//usually same as uid/gid. if set-UID, set-GID are set (s in perms) they takes program owner ids
	uid_t eff_usrid = geteuid();
	gid_t eff_grpid = getegid();
	printf("effective user id: %d, effective group id: %d\n", eff_usrid, eff_grpid);

	/*
	any_file ->
	-> struct stat (st_uid - user-owner / st_gid - group-owner) ->
	-> field st_mode ->
	-> flag set-user-ID / set-group-ID (set uid / gid as effective uid / gid)
	*/

	FILE *file = NULL;
	if ((file = fopen(path, "r")) != NULL) {  //fopen -> open (same errors) -> umask (setting perms) (if file not exist, umask set 0666)
		printf("success\n");
		fclose(file);
	}
	else {
		printf("%d\n", errno);
		perror("1-st fopen");  //printing string from error list (sys_errlist[]) by thread-local errno index
	}


	//try to invoke error in setuid (EPERM)
	if (setuid(usrid + 1) != 0) {
		perror("setuid");
	}
	//following the lab tasks
	if (setuid(usrid) != 0) {
		perror("setuid");
	}


	if ((file = fopen(path, "r")) != NULL) {
		printf("success\n");
		fclose(file);
	}
	else {
		perror("2-nd fopen");
	}


	//just for fun

	/*int size = sys_nerr - 1;
	for (int i = 0; i < size - 1; i++) {
		printf("%s\n", sys_errlist[i]);
	}*/
	/*printf("\t strerror\n");
	for (int errnum = 0; errnum < 256; errnum++) {
		printf("%d: %s\n", errnum, strerror(errnum));
	}*/

	return 0;
}