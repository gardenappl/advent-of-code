#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include "aoc.h"

static int64_t solve(aoc_c32_2d_t matrix, int32_t part, aoc_err_t * err) {
	return matrix.width * matrix.height;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_c32_2d(argc, argv, 1, solve);
}
