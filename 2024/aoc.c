#include "aoc.h"

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define AOC_COMPARE_DEFINE_FOR(type)  int aoc_compare_##type(const void* a, const void* b) { \
	type arg1 = *(const type*)a; \
	type arg2 = *(const type*)b; \
 \
	if (arg1 < arg2) return -1; \
	if (arg1 > arg2) return 1; \
	return 0; \
}

AOC_COMPARE_DEFINE_FOR(int32_t)

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
