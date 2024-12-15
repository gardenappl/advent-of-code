#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "aoc.h"

static void mark_antinodes(aoc_bit_array_t is_antinode, int32_t width,
		int32_t x, int32_t y, 
		int32_t x2, int32_t y2, 
		int32_t x_diff, int32_t y_diff) {
	int32_t x_anti = x2 + x_diff;
	int32_t y_anti = y2 + y_diff;
	aoc_bit_array_set(is_antinode, aoc_index_2d(width, x_anti, y_anti), true);
}

static void mark_antinodes_resonating(aoc_bit_array_t is_antinode, aoc_c32_2d_t matrix,
		int32_t x, int32_t y, 
		int32_t x2, int32_t y2, 
		int32_t x_diff, int32_t y_diff) {
	int32_t x_anti = x2;
	int32_t y_anti = y2;
	do {
		aoc_bit_array_set(is_antinode, aoc_index_2d(matrix.width, x_anti, y_anti), true);
		x_anti += x_diff;
		y_anti += y_diff;
	} while (aoc_check_bounds(matrix, x_anti, y_anti));

	x_anti = x;
	y_anti = y;
	do {
		aoc_bit_array_set(is_antinode, aoc_index_2d(matrix.width, x_anti, y_anti), true);
		x_anti -= x_diff;
		y_anti -= y_diff;
	} while (aoc_check_bounds(matrix, x_anti, y_anti));
}

static bool search_antennas(aoc_bit_array_t is_antinode, aoc_c32_2d_t matrix, int32_t x, int32_t y, int32_t part) {
	char32_t antenna = aoc_c32_2d_get(matrix, x, y);

	if (antenna == U'.')
		return false;

	int32_t check_up = y;
	int32_t check_left = x;
	int32_t check_down = matrix.height - 1 - y;
	int32_t check_right = matrix.width - 1 - x;
	if (part == 1) {
		check_up /= 2;
		check_left /= 2;
		check_down /= 2;
		check_right /= 2;
	}
	for (int32_t y_diff = -check_up; y_diff <= check_down; ++y_diff) {
		for (int32_t x_diff = -check_left; x_diff <= check_right; ++x_diff) {
			int32_t x2 = x + x_diff;
			int32_t y2 = y + y_diff;
			if (!(x == x2 && y == y2)) {
				char32_t antenna2 = aoc_c32_2d_get(matrix, x2, y2);
				if (antenna2 == antenna) {
					if (part == 1)
						mark_antinodes(is_antinode, matrix.width,
								x, y, x2, y2, x_diff, y_diff);
					else
						mark_antinodes_resonating(is_antinode, matrix,
								x, y, x2, y2, x_diff, y_diff);
				}
			}
		}
	}
	return true;
}

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

	aoc_bit_array_t is_antinode = aoc_bit_array_make(matrix.width * matrix.height, err);
	if (aoc_is_err(err)) return -1;

	for (int32_t y = 0; y < matrix.height; ++y) {
		for (int32_t x = 0; x < matrix.width; ++x) {
			search_antennas(is_antinode, matrix, x, y, part);
		}
	}
	int64_t antinodes = 0;
	for (size_t y = 0; y < matrix.height; ++y) {
		for (size_t x = 0; x < matrix.width; ++x) {
			bool antinode = aoc_bit_array_get(is_antinode, aoc_index_2d(matrix.width, x, y));
			if (antinode)
				++antinodes;
			fputc(antinode ? '#' : '.', stderr);
		}
		fputc('\n', stderr);
	}

	free(is_antinode.data);

	return antinodes;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_c32_2d(argc, argv, 2, solve);
}
