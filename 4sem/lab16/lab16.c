#include <stdio.h>
#include <termios.h>
#include <stdlib.h> // atexit

struct termios old_mode;

void clear_screen() {
	fwrite("\e[2J", 4, 1, stdin);
	fwrite("\e[H", 3, 1, stdin);
}

int set_new_mode() {
	tcgetattr(fileno(stdin), &old_mode);

	struct termios mode;
	mode = old_mode;

	mode.c_lflag &= ~(ICANON); //turning off canonical mode
	mode.c_cc[VMIN] = 1; //setting 0 to minimal input character amount
	// mode.c_cc[VTIME] = 0; //setting time to 0 of waiting of putting next character

	tcsetattr(fileno(stdin), TCSANOW, &mode);
}

void set_old_mode() {
	tcsetattr(fileno(stdin), TCSANOW, &old_mode); // STDIO_FILENO == fileno(stdin)
}

int main() {
	set_new_mode();
	atexit(set_old_mode);

	char answer = '?';
	printf("would you want to pass the lab? (y/n): ");
	scanf("%c", &answer);

	clear_screen();

	printf("\nyour answer is: %c\n", answer);

	return 0;
}