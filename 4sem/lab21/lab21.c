#include <signal.h>
#include <stdio.h>

/*
signal requires some permissions to seng signals to another process
1) privileged process (CAP_KILL) can send signals to any process
2) init can take signal only if it can be handled
*/

int main() {
	return 0;
}