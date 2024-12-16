#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#include "aoc.h"

typedef struct trailhead_info {
	int32_t score;
	int32_t rating;
} trailhead_info_t;

static trailhead_info_t count_paths(aoc_c32_2d_t matrix, int32_t x, int32_t y, char32_t height, aoc_bit_array_t visited) {
	aoc_bit_array_set(visited, aoc_index_2d(matrix.width, x, y), true);

	// char char32_print[MB_LEN_MAX] = { 0 };
	trailhead_info_t info = { 0 };
	for (int32_t dir = 0; dir < 4; ++dir) {
		int32_t x_next = x + aoc_dir4_x_diffs[dir];
		int32_t y_next = y + aoc_dir4_y_diffs[dir];
		if (!aoc_check_bounds(matrix, x_next, y_next))
			continue;
		
		size_t index_2d_next = aoc_index_2d(matrix.width, x_next, y_next);
		char32_t height_next = matrix.ws[index_2d_next];

		// aoc_c32_to_str(height_next, char32_print, err);
		// fprintf(stderr, "looking at (%" PRId32 "; %" PRId32 ") = %s (diff = %d)\n", x_next, y_next, char32_print, height_next - height);

		if (height_next - height != 1)
			continue;
		// fprintf(stderr, "is this anything?\n");

		if (height_next == U'9') {
			if (!aoc_bit_array_get(visited, index_2d_next)) {
				aoc_bit_array_set(visited, index_2d_next, true);
				++info.score;
			}
			++info.rating;
		} else {
			trailhead_info_t next = count_paths(matrix, x_next, y_next, height_next, visited);
			info.score += next.score;
			info.rating += next.rating;
		}
	}

	if (height == U'0')
		fprintf(stderr, "x: %" PRId32 " y: %" PRId32 " has score %" PRId32 " and rating %" PRId32 ".\n", x, y, info.score, info.rating);
	return info;
}

static int64_t solve(aoc_c32_2d_t matrix, int32_t part, aoc_err_t * err) {
	int64_t sum = 0;

	aoc_bit_array_t visited = aoc_bit_array_make(matrix.width * matrix.height, err);
	if (aoc_is_err(err))
		return -1;

	for (int32_t y = 0; y < matrix.height; ++y) {
		for (int32_t x = 0; x < matrix.width; ++x) {
			char32_t height = aoc_c32_2d_get(matrix, x, y);
			if (height == U'0') {
				trailhead_info_t info = count_paths(matrix, x, y, U'0', visited);
				if (part == 1)
					sum += info.score;
				else
					sum += info.rating;
				aoc_bit_array_reset(visited);
			}
		}
	}

	return sum;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_c32_2d(argc, argv, 2, solve);
}
