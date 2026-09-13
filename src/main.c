/* compile.sh - Copyright (c) 2026 Minetomba minetomba@proton.me */
/* SPDX-License-Identifier: GPL-3.0-only */
#include "../include/input.c"
#include "../include/files.c"
#include "../include/renderer.c"
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
	char *filename; /* All declarations must be done at the top */
	int event;
	int cursor[] = {0, 0}; /* Row 0, column 0 */
	int del_pos[2];
	int prev_len;

	if (argc != 2) {
		printf("Usage: %s <file>", argv[0]);

		return 1;
	}

	filename = argv[1];
	
	enable_raw_mode();

	while (1 == 1) {
		render(filename, cursor);
		event = read_key();
		if (event < 0x100) {
			insert_byte_at(filename, cursor, event);
			cursor[1] += 1;
		}
		if (event == KEY_UP && cursor[0] > 0) {
			cursor[0] -= 1;
			if (cursor[1] > line_length(filename, cursor[0])) cursor[1] = line_length(filename, cursor[0]);
		}
		if (event == KEY_DOWN && cursor[0] + 1 < get_line_count(filename)) {
			cursor[0] += 1;
			if (cursor[1] > line_length(filename, cursor[0])) cursor[1] = line_length(filename, cursor[0]);
		}
		if (event == KEY_LEFT && cursor[1] > 0) {
			cursor[1] -= 1;
		}
		if (event == KEY_RIGHT && cursor[1] < line_length(filename, cursor[0])) {
			cursor[1] += 1;
		}
		if (event == KEY_ENTER) {
			insert_byte_at(filename, cursor, '\n');
			cursor[1] = 0;
			cursor[0] += 1;
		}
		if (event == KEY_BACKSPACE) {
			if (cursor[1] > 0) {
				del_pos[0] = cursor[0];
				del_pos[1] = cursor[1] - 1;
				if (delete_byte_at(filename, del_pos) == 0) {
					cursor[1] -= 1;
				}
			} else if (cursor[0] > 0) {
				prev_len = line_length(filename, cursor[0] - 1);
				if (prev_len >= 0) {
					del_pos[0] = cursor[0] - 1;
					del_pos[1] = prev_len;
					if (delete_byte_at(filename, del_pos) == 0) {
						cursor[0] -= 1;
						cursor[1] = prev_len;
					}
				}
			}
		}
	}

	disable_raw_mode();

	return 0;
}
