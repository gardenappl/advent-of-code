#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"


static int count_digits(long num) {
	int digits = 0;
	while (num > 0) {
		num /= 10;
		++digits;
	}
	return digits;
}


static int64_t stone_steps(long stone, int steps) {
	int64_t stone_count = 1;
	while (steps > 0) {
		if (stone == 0) {
			stone = 1;
		} else {
			int digits = count_digits(stone);
			if (digits % 2 == 0) {
				fprintf(stderr, "split %ld into ", stone);

				long stone_split = stone;
				long digits_split = digits / 2;
				while (digits_split > 0) {
					stone_split /= 10;
					--digits_split;
				}
				long stone2 = stone_split;

				digits_split = digits / 2;
				while (digits_split > 0) {
					stone_split *= 10;
					--digits_split;
				}
				stone -= stone_split;

				fprintf(stderr, "%ld and %ld\n", stone2, stone);
				stone_count += stone_steps(stone2, steps - 1);
			} else {
				stone *= 2024;
			}
		}
		--steps;
	}
	return stone_count;
}

static int64_t solve(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err) {
	if (lines_n != 1) {
		aoc_err(err, "Wrong number of lines!");
		return -1;
	}

	char const * line = *lines;
	if (*line == '\0')
		return 0;

	char * next = NULL;

	int64_t stones_count_total = 0;

	while (true) {
		long stone = strtoll(line, &next, 10);
		if (aoc_err_if_errno(err, "Number too big"))
			return -1;

		if (next == line)
			return stones_count_total;
		line = next;

		int64_t stones_count = stone_steps(stone, 25);
		fprintf(stderr, "stone (%ld) becomes %" PRId64 " stones.\n", stone, stones_count);
		stones_count_total += stones_count;
	}
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 1, solve);
}
