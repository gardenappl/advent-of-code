#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>

#include "aoc.h"

static bool word_search(aoc_matrix_t matrix, char const * word, size_t x, size_t y, size_t x_diff, size_t y_diff) {
	if (aoc_matrix_get(matrix, x, y) != *word)
		return false;
	do {
		x += x_diff;
		y += y_diff;
		word++;
	} while (*word && aoc_matrix_bounded(matrix, x, y) && aoc_matrix_get(matrix, x, y) == *word);
	return !(*word);
}

int64_t solve_for_matrix1(aoc_matrix_t matrix) {
	int32_t xmas_found = 0;
	for (size_t y = 0; y < matrix.height; y++) {
		for (size_t x = 0; x < matrix.width; x++) {
			for (size_t i = 0; i < 8; i++) {
				size_t x_diff = aoc_sq8_x_diffs[i];
				size_t y_diff = aoc_sq8_y_diffs[i];
				bool found = word_search(matrix, "XMAS", x, y, x_diff, y_diff);
				if (found)
					xmas_found++;
			}
		}
	}
	return xmas_found;
}

int64_t solve_for_matrix2(aoc_matrix_t matrix) {
	int32_t cross_mas_found = 0;
	for (size_t y = 1; y < matrix.height - 1; y++) {
		for (size_t x = 1; x < matrix.width - 1; x++) {
			int32_t mas_found = 0;
			if (aoc_matrix_get(matrix, x, y) == 'A') {
				for (size_t i = 0; i < 4; i++) {
					size_t x_diff = aoc_cross4_x_diffs[i];
					size_t y_diff = aoc_cross4_y_diffs[i];

					if (word_search(matrix, "MAS", x - x_diff, y - y_diff, x_diff, y_diff))
						mas_found++;
				}
				if (mas_found == 2) {
					cross_mas_found++;
				}
			}
		}
	}
	return cross_mas_found;
}

static char * solve1(FILE * input) {
	return aoc_solve_for_matrix(input, solve_for_matrix1);
}

static char * solve2(FILE * input) {
	return aoc_solve_for_matrix(input, solve_for_matrix2);
}

int main(int argc, char * argv[]) {
	aoc_main(argc, argv, solve1, solve2);
}
