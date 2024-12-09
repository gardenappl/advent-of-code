#include "aoc.h"

#include <assert.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#define AOC_COMPARE_DEFINE_FOR(type)  int aoc_compare_##type(void const * a, void const * b) { \
	type arg1 = *(type const *)a; \
	type arg2 = *(type const *)b; \
 \
	if (arg1 < arg2) return -1; \
	if (arg1 > arg2) return 1; \
	return 0; \
}


AOC_COMPARE_DEFINE_FOR(int32_t)

size_t aoc_sq8_x_diffs[] = { 0,  1,  1,  1,  0, -1, -1, -1 };
size_t aoc_sq8_y_diffs[] = { 1,  1,  0, -1, -1, -1,  0,  1 };
size_t aoc_cross4_x_diffs[] = {  1,  1, -1, -1 };
size_t aoc_cross4_y_diffs[] = { -1,  1,  1, -1 };

size_t aoc_count_lines(FILE * file) {
	fseek(file, 0, SEEK_SET);
	if (ferror(file)) return -1;

	size_t line_count = 0;

	for (char c = getc(file); c != EOF; c = getc(file))
		if (c == '\n' && line_count < SIZE_MAX)
			line_count++;
	if (ferror(file)) return -1;

	fseek(file, 0, SEEK_SET);
	return line_count;
}

void * aoc_read_file(FILE * file) {
	fseek(file, 0, SEEK_END);
	if (ferror(file)) return NULL;

	long size = ftell(file);
	if (ferror(file)) return NULL;

	fseek(file, 0, SEEK_SET);
	if (ferror(file)) return NULL;

	if (size > SIZE_MAX - 1)
		return NULL;
	char * buf = malloc((size_t)(size + 1));
	size_t read = fread(buf, (size_t)size, 1, file);
	if (read != 1) {
		free(buf);
		return NULL;
	}
	buf[((size_t)size) - 1] = 0;
	return buf;
}

bool aoc_s_matrix_init(char const * s, aoc_s_matrix * s_matrix) {
	char * next_line = strchr(s, '\n');
	if (!next_line) return EXIT_FAILURE;
	
	aoc_s_matrix m = { .s = s, .width = next_line - s };
	do {
		if ((next_line - s) != m.width)
			return EXIT_FAILURE;
		m.height++;
		s = next_line + 1;
	} while ((next_line = strchr(s, '\n')));

	if (strlen(s) != m.width)
		return EXIT_FAILURE;
	m.height++;
	*s_matrix = m;
	return EXIT_SUCCESS;
}

char aoc_s_matrix_get(aoc_s_matrix s_matrix, size_t x, size_t y) {
	return s_matrix.s[((s_matrix.width + 1) * y) + x];
}

bool aoc_s_matrix_bounded(aoc_s_matrix s_matrix, size_t x, size_t y) {
	return x >= 0 && y >= 0 && x < s_matrix.width && y < s_matrix.height;
}


// Main function boilerplate

int aoc_main(int argc, char * argv[], char * (*solve1)(FILE *), char * (*solve2)(FILE *)) {
	if (argc < 2) {
		char * prog_name = "PROGRAM_NAME";
		if (argc > 0)
			prog_name = argv[0];
		fprintf(stderr, "Usage: %s INPUT_FILE\n", prog_name);
		return AOC_EXIT_USAGE;
	}
	FILE* file = fopen(argv[1], "rb");
	if (!file) {
		perror("Could not open file");
		return AOC_EXIT_NO_INPUT;
	}

	bool fail = false;
	char* result = solve1(file);
	if (result) {
		fprintf(stderr, "Part 1 success!\n");
		puts(result);
	} else if (ferror(file)) {
		perror("File error");
		return AOC_EXIT_IO_ERROR;
	} else {
		fprintf(stderr, "Part 1 fail!\n");
		fail = true;
	}

	fseek(file, 0, SEEK_SET);
	if (ferror(file)) {
		perror("Could not seek?");
		return AOC_EXIT_IO_ERROR;
	}

	if (solve2) {
		result = solve2(file);
		if (result) {
			fprintf(stderr, "Part 2 success!\n");
			puts(result);
		} else if (ferror(file)) {
			perror("File error");
			return AOC_EXIT_IO_ERROR;
		} else {
			fprintf(stderr, "Part 2 fail!\n");
			fail = true;
		}
	}

	fclose(file);
	return fail ? AOC_EXIT_SOFTWARE_FAIL : EXIT_SUCCESS;
}


char * aoc_solve_for_matrix(FILE * input, int64_t (*solve_for_matrix)(aoc_s_matrix)) {
	char const * s = aoc_read_file(input);
	if (!s) return NULL;

	aoc_s_matrix s_matrix;
	bool err_ = aoc_s_matrix_init(s, &s_matrix);
	if (err_) {
		free((void *)s);
		return NULL;
	}
	int64_t result = solve_for_matrix(s_matrix);
	free((void *)s);

	char * buf = malloc(AOC_MSG_SIZE);
	snprintf(buf, AOC_MSG_SIZE, "%" PRId64, result);
	return buf;
}
