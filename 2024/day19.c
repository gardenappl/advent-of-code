#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

static int64_t can_create(char const * pattern, int64_t * arrangements_from_pos, size_t pattern_length, size_t pattern_pos, char const * const * towels, size_t const * towel_str_lengths, size_t towels_n, bool part1) {
	int64_t arrangements = 0;

	for (size_t i = 0; i < towels_n; ++i) {
		int32_t next_pos = pattern_pos + towel_str_lengths[i];
		if (next_pos > pattern_length) {
			continue;
		}
		if (strncmp(pattern + pattern_pos, towels[i], towel_str_lengths[i]) == 0) {
			if (next_pos == pattern_length) {
				if (part1)
					return 1;
				++arrangements;
			} else {
				int64_t arrangements_from_next = arrangements_from_pos[next_pos];
				if (arrangements_from_next > 0) {
					arrangements += arrangements_from_next;
				} else if (arrangements_from_next == 0) {
					arrangements_from_next = can_create(pattern, arrangements_from_pos, 
							pattern_length, next_pos,
							towels, towel_str_lengths, towels_n, part1);
					if (part1 && arrangements_from_next > 0)
						return 1;
					arrangements += arrangements_from_next;
				}
			}
		}
	}
	if (arrangements != 0)
		arrangements_from_pos[pattern_pos] = arrangements;
	else
		arrangements_from_pos[pattern_pos] = -1;
	return arrangements;
}

static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	int64_t result = 0;
	if (lines_n < 3) {
		aoc_throw(e, AOC_EX_DATA_ERR, "wrong number of lines");
		return -1;
	}

	char * towels_line = assert_malloc(line_lengths[0] + 1, char);
	strcpy(towels_line, lines[0]);

	size_t estimated_size = (line_lengths[0] + 2) / 3;
	char ** towels_ = assert_calloc(estimated_size, char *);
	size_t * towel_str_lengths_ = assert_calloc(estimated_size, size_t);
	size_t towels_n = aoc_c32_split(towels_line, U',', towels_, towel_str_lengths_, estimated_size, e);
	if (*e)
		goto cleanup_towels;

	for (size_t i = 0; i < towels_n; ++i) {
		while (isspace(*towels_[i])) {
			++towels_[i];
			--towel_str_lengths_[i];
		}
		fprintf(stderr, "Towel: '%s' (length: %zu)\n", towels_[i], towel_str_lengths_[i]);
	}
	// fgetc(stdin);
	char const * const * towels = (char const * const *)towels_;
	size_t const * towel_str_lengths = (size_t const *)towel_str_lengths_;

	estimated_size = 0;
	for (size_t i = 2; i < lines_n; ++i) {
		if (line_lengths[i] > estimated_size)
			estimated_size = line_lengths[i];
	}

	int64_t * arrangements_from_pos = assert_calloc(estimated_size, int64_t);

	for (size_t i = 2; i < lines_n; ++i) {
		// char const * pattern = (char const *)lines[i];
		char const * pattern = lines[i];
		fprintf(stderr, "Pattern #%zu (length: %zu, '%s')...", (i - 1), line_lengths[i], pattern);
		int64_t arrangements = can_create(pattern, arrangements_from_pos, line_lengths[i], 
				0, towels, towel_str_lengths, towels_n, part == 1);
		result += arrangements;
		fprintf(stderr, "%"PRId64"\n", arrangements);
		memset(arrangements_from_pos, 0, estimated_size * sizeof(int64_t));
	}
	free(arrangements_from_pos);
cleanup_towels:
	free(towels_);
	free(towel_str_lengths_);
	return result;
}

int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 2, solve);
}
