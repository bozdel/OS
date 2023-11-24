#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>

/*
---------mem manipulation by fork-----------
2 ways
1) kenrel makes text segment of each process RO, so they can have common segment.
fork makes mem page table for each process with entry linked to the block of physical memory page

2) copy-on-write ??? (used in modern UNIX systems)
???

------------fork with files-----------------
fork copies fds like a dup() (fds linked to same file)

------------fork with io--------------------
printf use line buffering, but output redirection block buffering. so (printf -> fork -> file) != (printf -> fork -> console)
1)can be fixed by setbuf/setvbuf for stdout
2)fflush before fork
3)use _exit for child processes (_exit won't flush buffer of that process)

where is stdio buffer?

write - is kernel-buffered io func, fork won't copy it to children. so it will be printed as usual

----------------exits-----------------------
defference exit() / _exit()
_exit can take 0<->255 values (parent can take only 8 bits of argument), but >128 cause some problems with CL-scripts

exit - invoke exit-handler, flush stdio buffer, invoke _exit
*/

/*
---------------fork alternatives------------
missing in SUSv4
vfork. deprecated; a bit faster (in modern systems) (not guaranteed due to lack of specs); use only if provides significant increase in speed;
don't use exit() (stdio will flushed and closed)
1) share mem, makes parent wait between vfork call and successful exec* or _exit calls
2) allowed to open fds because process fds table stored in kernel space. check SUSv3, i'm not shure about it:)
*/

/*
---------------------wait-------------------
wait. unpredictable order of handling completed child processes (by SUSv3) depend on implementation (even in different versions of linux)
blocking waiting(?), unlike non-blocking waiting waitpid

waitpid
int options:
bit flags

int *wstatus:
only macros
*/

/*
--------------orphans / zombies-------------
orphans (parent die before child)

zombie/defunct (child complete before wait() in parent)
kernel: child -> zombie
release of all ress, except: child id, exit status, res usage stats in kenrel process table entry(?)
cannot be killed by SIGKILL
removed only by wait(). (if parent don't call wait, init: zombie -> orphans, then wait() them)
*/

/*
-------------handling children--------------
when process blocking (wait) and bad performance (waitpid) doesn't suit
SIGCHLD is sent to parent every time a child finished work
1) but signal, which cause handler work, becomes temporarily blocked
2) there is no queue for SIGCHLD signal
solution: infinity cicle with waitpid(-1, NULL, WNOHANG) > 0 (0 - no changed children (zombies), -1 - probably ECHILD)
for avoiding case where already exist finished child before starting handler, you can start it before createing any children
system call in handler can change value of global errno.
errno can be stored to local var in handler before any action, and be retrieved right before the exit
*/



int main(int argc, char *argv[]) {
	

	// RLIMIT_NPROC max amount of processes in system exceeded. one of the reasons why fork returns -1

	// race condition due to unpredictability of the order in which resourses are obtained
	// semaphors/file-blocking/pipes/signals

	// just demonstration of stdio behavior with fork and redirection

	if (argc > 1) {
		printf("%s\n", argv[1]);
		write(STDOUT_FILENO, "goodbye!\n", 9);
		if (fork() == -1) {
			perror("fork: ");
			exit(EXIT_FAILURE);
		}
		return 0;
	}


	pid_t pid = fork();

	printf("my process id is: %d\n", pid);

	char *newargv[] = {"cat", "big_file", NULL};
	char *envv[] = { NULL };

	switch (pid) {
		case -1:
			perror("error in fork\n");
			break;
		case 0: // child
			// cl doesn't trace child process output??
			execve("/bin/cat", newargv, envv);
			perror("execve: ");
			break;
		default: // parent
// #define WAIT
#ifdef WAIT
			int status = 0;
			if (wait(&status) == -1) {
				perror("wait: ");
			}
#endif
			printf("i'm a parent process\n");
			break;
	}



	return 0;
}
