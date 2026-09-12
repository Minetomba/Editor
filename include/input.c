#define _POSIX_C_SOURCE 200809L

#include <termios.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <sys/select.h>

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
	raw.c_cc[VMIN]  = 1;
	raw.c_cc[VTIME] = 0;

	if (tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == -1) {
		exit(1);
	}

	return;
}

int read_byte() {
	unsigned char c;
	
	ssize_t n;
	while (1) {
		n = read(STDIN_FILENO, &c, 1);
		
		if (n == 1) return (int)c;
		if (n == 0) return -1;
		if (errno == EINTR) continue;

		return -1;
	}
}

int wait_for_input(int ms) {
	fd_set fds;
	struct timeval tv;
	int r;
	
	FD_ZERO(&fds);
	FD_SET(STDIN_FILENO, &fds);
	
	tv.tv_sec = ms / 1000;
	tv.tv_usec = (ms % 1000) * 1000;
	r = select(STDIN_FILENO + 1, &fds, NULL, NULL, &tv);
	
	if (r < 0) {
		if (errno == EINTR) return 0;
	
		return 0;
	}

	return r > 0;
}

int read_byte_timeout(int ms) {
	if (!wait_for_input(ms)) return -1;
	return read_byte();
}

enum {
	KEY_ESC = 0x100, /* Special keys will live above 0xff */
	KEY_UP,
	KEY_DOWN,
	KEY_LEFT,
	KEY_RIGHT,
	KEY_BACKSPACE,
	KEY_OTHER
};

int read_key() {
	int c = read_byte();

	if (c == 0x7f || c == 0x08) return KEY_BACKSPACE;
	if (c == -1) return KEY_OTHER;
	if (c != 0x1b) return c;
	
	c = read_byte_timeout(50);

	if (c == -1) return KEY_ESC;
	if (c != '[' && c != 'O') return KEY_OTHER;

	c = read_byte_timeout(50);

	if (c == -1) return KEY_OTHER;

	switch (c) {
		case 'A': return KEY_UP;
		case 'B': return KEY_DOWN;
		case 'C': return KEY_RIGHT;
		case 'D': return KEY_LEFT;

		default: return KEY_OTHER;
	}

}
