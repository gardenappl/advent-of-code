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


#define MAX_STONE_CALCULATED 10000


static int64_t stone_steps(long stone, int steps, int64_t * stones_calculated) {
	int64_t stone_count;

	if (stone < MAX_STONE_CALCULATED) {
		stone_count = stones_calculated[aoc_index_2d(MAX_STONE_CALCULATED, stone, steps - 1)];
		if (stone_count != 0)
			return stone_count;
	}
	stone_count = 1;

	while (steps > 0) {
		if (stone == 0) {
			stone = 1;
		} else {
			int digits = count_digits(stone);
			if (digits % 2 == 0) {
				if (steps == 1) {
					++stone_count;
					break;
				}
				// fprintf(stderr, "split %ld into ", stone);

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

				// fprintf(stderr, "%ld and %ld\n", stone2, stone);
				int64_t stone_count2 = 0;
				if (stone2 < MAX_STONE_CALCULATED) {
					stone_count2 = stones_calculated[aoc_index_2d(MAX_STONE_CALCULATED, stone2, steps - 2)];
				}
				if (stone_count2 == 0) {
					stone_count2 = stone_steps(stone2, steps - 1, stones_calculated);
					if (stone2 < MAX_STONE_CALCULATED) {
						stones_calculated[aoc_index_2d(MAX_STONE_CALCULATED, stone2, steps - 2)] = stone_count2;
					}
				}
				stone_count += stone_count2;
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

	int steps = part == 1 ? 25 : 75;

	int64_t * stones_calculated = calloc(MAX_STONE_CALCULATED * steps, sizeof(int64_t));
	if (!stones_calculated) {
		aoc_err(err, "could not allocate memo array");
		return -1;
	}
	int64_t stones_count_total = 0;

	while (true) {
		long stone = strtoll(line, &next, 10);
		if (aoc_err_if_errno(err, "Number too big"))
			return -1;

		if (next == line)
			break;
		line = next;

		int64_t stones_count = stone_steps(stone, steps, stones_calculated);
		fprintf(stderr, "stone (%ld) becomes %" PRId64 " stones.\n", stone, stones_count);
		stones_count_total += stones_count;
	}
	free(stones_calculated);
	return stones_count_total;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 2, solve);
}
