#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "aoc.h"

static int64_t solve(aoc_c32_2d_t matrix, int32_t part, aoc_err_t * err) {
	char c32_print[MB_LEN_MAX + 1];
	for (int32_t y = 0; y < matrix.height; ++y) {
		for (int32_t x = 0; x < matrix.width; ++x) {
			char32_t c32 = aoc_c32_2d_get(matrix, x, y);

			aoc_c32_to_str(c32, c32_print, err);
			if (aoc_is_err(err)) return -1;

			fprintf(stderr, "[%s] ", c32_print);
		}
		fprintf(stderr, "\n");
	}

	int64_t antinodes = 0;
	aoc_bit_array_t is_antinode = aoc_bit_array_make(matrix.width * matrix.height, err);
	if (aoc_is_err(err)) return -1;

	for (int32_t y = 0; y < matrix.height; ++y) {
		for (int32_t x = 0; x < matrix.width; ++x) {
			char32_t antenna = aoc_c32_2d_get(matrix, x, y);

			if (antenna == U'.')
				continue;

			int32_t check_up = y / 2;
			int32_t check_left = x / 2;
			int32_t check_down = (matrix.height - 1 - y) / 2;
			int32_t check_right = (matrix.width - 1 - x) / 2;
			for (int32_t y_diff = -check_up; y_diff <= check_down; ++y_diff) {
				for (int32_t x_diff = -check_left; x_diff <= check_right; ++x_diff) {
					int32_t x2 = x + x_diff;
					int32_t y2 = y + y_diff;
					if (!(x == x2 && y == y2)) {
						char32_t antenna2 = aoc_c32_2d_get(matrix, x2, y2);
						if (antenna2 == antenna) {
							int32_t x_anti = x2 + x_diff;
							int32_t y_anti = y2 + y_diff;
							size_t index = aoc_index_2d(matrix.width, x_anti, y_anti);
							if (aoc_bit_array_get(is_antinode, index)) {
								continue;
							} else {
								++antinodes;
								aoc_bit_array_set(is_antinode, index, true);
							}
						}
					}
				}
			}
		}
	}
	for (size_t y = 0; y < matrix.height; ++y) {
		for (size_t x = 0; x < matrix.width; ++x) {
			bool antinode = aoc_bit_array_get(is_antinode, aoc_index_2d(matrix.width, x, y));
			fputc(antinode ? '#' : '.', stderr);
		}
		fputc('\n', stderr);
	}

	free(is_antinode.data);

	return antinodes;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_c32_2d(argc, argv, 1, solve);
}
