#include <stdio.h>
#include <sys/ioctl.h>
#include <unistd.h>

int get_term_size(int *rows, int *cols) {
	struct winsize ws;

	if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1) return -1;
	if (ws.ws_row == 0 || ws.ws_col == 0) return -1;

	*rows = ws.ws_row - 1;
	*cols = ws.ws_col;

	return 0;
}

int render(const char *filename, int cursor[2]) {
	FILE *f;
	int c;
	int row = 0;
	int col = 0;
	int pipe_drawn = 0;
	int term_rows, term_cols;
	int top;
	int last;

	if (get_term_size(&term_rows, &term_cols) != 0) {
		term_rows = 24;
		term_cols = 80;
	}

	top = 0;

	if (cursor[0] >= term_rows) {
		top = cursor[0] - term_rows + 1;
	}

	last = top + term_rows - 1;

	printf("\x1b[2J\x1b[H");

	f = fopen(filename, "rb");
	if (!f) return -1;

	while (row < top) {
		c = fgetc(f);
		if (c == EOF) break;
		if (c == '\n') row += 1;
	}

	col = 0;

	while (row <= last) {
		if (!pipe_drawn && row == cursor[0] && col == cursor[1]) {
			putchar('|');
			pipe_drawn = 1;
		}

		c = fgetc(f);
		if (c == EOF) break;

		putchar(c);

		if (c == '\n') {
			row += 1;
			col = 0;
		} else {
			col += 1;
		}
	}

	if (!pipe_drawn && row == cursor[0] && col == cursor[1]) {
		putchar('|');
		pipe_drawn = 1;
	}
	if (!pipe_drawn && cursor[0] == row && cursor[1] >= col) {
		putchar('|');
	}

	fclose(f);
	fflush(stdout);

	return 0;
}