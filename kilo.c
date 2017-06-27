#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>

struct termios orig_termios;

void disableRawMode() {
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}
void enableRawMode() {
	tcgetattr(STDIN_FILENO, &orig_termios); //read a terminal's attributes into struct
	atexit(disableRawMode);

	struct termios raw = orig_termios;
	raw.c_iflag &= ~(ICRNL| IXON); 
	raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);//disable carriage-return/nl ctrl-m ctrl-s ctrl-q -- stops data from being transmitted to the terminal. 
	raw.c_oflag &= ~(OPOST); //output post processing - terminal reads \n as \r\n so now we're chillin.
	raw.c_cflag |= (CS8); // bitmask that sets character size to 8 bits per byte because fuck unicode
	raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG); //negate echo | canonical mode | ctrl-v (term waits for you to type annother character) | Ctrl-c ctrl-y ctrl-z 
	//set the terminal attributes - TCSAFLUSH == makes change until all queued output has been written and disregards queued input
	tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw); 
}

int main() {
	enableRawMode();
	
	char c;
	while (read(STDIN_FILENO, &c, 1) == 1 && c != 'q'){
		if(iscntrl(c)) {
			printf("%d\r\n", c);
		} else {
			printf("%d ('%c')\r\n", c, c);
		}
	}
		
	return 0;
}
