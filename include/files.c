/* compile.sh - Copyright (c) 2026 Minetomba minetomba@proton.me */
/* SPDX-License-Identifier: GPL-3.0-only */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHUNK 65536

static int compute_offset(FILE *src, int row, int col, long *out_offset) {
	long offset = 0;
	int current_row = 0;
	int c;

	if (row < 0 || col < 0) return -1;

	while (current_row < row) {
		c = fgetc(src);
		if (c == EOF) {
			*out_offset = -2;

			return 0;
		}
		offset++;

		if (c == '\n') current_row++;
	}

	while (col > 0) {
		c = fgetc(src);

		if (c == EOF || c == '\n') break;

		offset++;
		col--;
	}

	*out_offset = offset;

	return 0;
}

static int copy_bytes(FILE *in, FILE *out, long n) {
	unsigned char buf[CHUNK];

	while (n > 0) {
		size_t want = (n > (long)sizeof(buf)) ? sizeof(buf) : (size_t)n;
		size_t got = fread(buf, 1, want, in);

		if (got != want) return -1;
		if (fwrite(buf, 1, got, out) != got) return -1;

		n -= (long)got;
	}

	return 0;
}

int insert_byte_at(const char *path, const int pos[2], unsigned char byte) {
	FILE *src = NULL;
	FILE *tmp = NULL;
	char tmp_path[1024];
	long size, offset, remaining;
	int rc = -1;

	if (strlen(path) + 5 >= sizeof(tmp_path)) return -1;

	strcpy(tmp_path, path);
	strcat(tmp_path, ".tmp");

	src = fopen(path, "rb");

	if (!src) return -1;
	if (fseek(src, 0, SEEK_END) != 0) goto cleanup;

	size = ftell(src);

	if (size < 0) goto cleanup;
	if (fseek(src, 0, SEEK_SET) != 0) goto cleanup;
	if (compute_offset(src, pos[0], pos[1], &offset) != 0) goto cleanup;
	if (offset == -2) offset = size;
	if (offset < 0) goto cleanup;
	if (fseek(src, 0, SEEK_SET) != 0) goto cleanup;

	tmp = fopen(tmp_path, "wb");

	if (!tmp) goto cleanup;
	if (copy_bytes(src, tmp, offset) != 0) goto cleanup;
	if (fputc(byte, tmp) == EOF) goto cleanup;

	remaining = size - offset;

	if (copy_bytes(src, tmp, remaining) != 0) goto cleanup;

	if (fclose(tmp) != 0) {
		tmp = NULL;
		goto cleanup;
	}

	tmp = NULL;

	if (fclose(src) != 0) {
		src = NULL;
		goto cleanup;
	}

	src = NULL;

	if (rename(tmp_path, path) != 0) {
		remove(path);
	
		if (rename(tmp_path, path) != 0) goto cleanup;
	}

	rc = 0;

cleanup:

	if (tmp) fclose(tmp);
	if (src) fclose(src);
	if (rc != 0) remove(tmp_path);
	
	return rc;
}

int line_length(const char *path, int row) {
	FILE *f = fopen(path, "rb");
	int current_row = 0, len = 0, c;

	if (!f) return -1;

	while (current_row < row) {
		c = fgetc(f);
		if (c == EOF) { fclose(f); return -1; }
		if (c == '\n') current_row++;
	}

	while ((c = fgetc(f)) != EOF && c != '\n') len++;

	fclose(f);

	return len;
}

int get_line_count(const char *filename) {
	FILE *f;
	int c;
	int count = 1;

	f = fopen(filename, "rb");
	if (!f) return -1;

	while ((c = fgetc(f)) != EOF) {
		if (c == '\n') count += 1;
	}

	fclose(f);

	return count;
}

int delete_byte_at(const char *path, const int pos[2]) {
	FILE *src = NULL;
	FILE *tmp = NULL;
	char tmp_path[1024];
	long size, offset, remaining;
	int rc = -1;

	if (strlen(path) + 5 >= sizeof(tmp_path)) return -1;

	strcpy(tmp_path, path);
	strcat(tmp_path, ".tmp");

	src = fopen(path, "rb");

	if (!src) return -1;
	if (fseek(src, 0, SEEK_END) != 0) goto cleanup;

	size = ftell(src);

	if (size < 0) goto cleanup;
	if (fseek(src, 0, SEEK_SET) != 0) goto cleanup;

	if (compute_offset(src, pos[0], pos[1], &offset) != 0) goto cleanup;
	if (offset == -2) goto cleanup;
	if (offset < 0) goto cleanup;
	if (offset >= size) goto cleanup;
	if (fseek(src, 0, SEEK_SET) != 0) goto cleanup;

	tmp = fopen(tmp_path, "wb");

	if (!tmp) goto cleanup;
	if (copy_bytes(src, tmp, offset) != 0) goto cleanup;
	if (fseek(src, offset + 1, SEEK_SET) != 0) goto cleanup;

	remaining = size - offset - 1;

	if (copy_bytes(src, tmp, remaining) != 0) goto cleanup;
	if (fclose(tmp) != 0) { tmp = NULL; goto cleanup; }

	tmp = NULL;

	if (fclose(src) != 0) { src = NULL; goto cleanup; }

	src = NULL;

	if (rename(tmp_path, path) != 0) {
		remove(path);
		if (rename(tmp_path, path) != 0) goto cleanup;
	}

	rc = 0;

cleanup:

	if (tmp) fclose(tmp);
	if (src) fclose(src);
	if (rc != 0) remove(tmp_path);

	return rc;
}