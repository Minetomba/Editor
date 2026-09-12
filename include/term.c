#define _POSIX_C_SOURCE 200809L
#include <termios.h>
#include <unistd.h>
#include <stdlib.h>

static struct termios orig_termios;

void disable_raw_mode() {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
	return;
}

void enable_raw_mode() {
    struct termios raw;

    if (tcgetattr(STDIN_FILENO, &orig_termios) == -1) {
        exit(1);
    }
    atexit(disable_raw_mode);

    raw = orig_termios;
    raw.c_lflag &= ~(ICANON | ECHO);
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 1;

    if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
        exit(1);
    }
	return;
}