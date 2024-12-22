#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"


static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	int64_t sum = 0;
	for (size_t i = 0; i < lines_n; ++i) {
		uint32_t n;
		if (sscanf(lines[i], "%"PRIu32, &n) != 1) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			return -1;
		}
		for (int step = 0; step < 2000; ++step) {
			n = ((n << 6) ^ n) & 0x00FFFFFF;
			n = ((n >> 5) ^ n) & 0x00FFFFFF;
			n = ((n << 11) ^ n) & 0x00FFFFFF;
		}
		fprintf(stderr, "%"PRIu32"\n", n);
		sum += n;
	}
	return sum;
}

int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
