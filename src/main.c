#include "../include/input.c"
#include "../include/files.c"
#include <stdio.h>

int main(int argc, char *argv[]) {
	char *filename; /* All declarations must be done at the top */
	int event;
	int cursor[] = {0, 0}; /* Row 0, column 0 */

	if (argc != 2) {
		printf("Usage: %s <file>", argv[0]);

		return 1;
	}

	filename = argv[1];
	
	enable_raw_mode();

	while (1 == 1) {
		event = read_key();
		if (event < 0x100) {
			insert_byte_at(filename, cursor, event);
			cursor[1] += 1;
		}
		if (event == KEY_UP && cursor[0] > 0) {
			cursor[0] -= 1;
		}
		if (event == KEY_DOWN) {
			cursor[0] += 1;
		}
		if (event == KEY_LEFT && cursor[1] > 0) {
			cursor[1] -= 1;
		}
		if (event == KEY_RIGHT) {
			cursor[1] += 1;
		}
	}

	disable_raw_mode();

	return 0;
}
