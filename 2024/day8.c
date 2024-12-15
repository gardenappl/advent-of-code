#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

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


	return matrix.width * matrix.height;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_c32_2d(argc, argv, 1, solve);
}
