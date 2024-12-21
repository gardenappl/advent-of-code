#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

#define PART1_OBSTACLES 1024
#define WIDTH 71
#define HEIGHT 71
#define X_END (WIDTH - 1)
#define Y_END (HEIGHT - 1)

static void shortest_path(int32_t * distances, int32_t x, int32_t y, int32_t current_dist, int32_t * min_dist) {
	// fprintf(stderr, "i'm walking 'ere: 'x: %"PRId32", y: %"PRId32", dist: %"PRId32"\n", x, y, current_dist);
	size_t x_y_i = aoc_index_2d(WIDTH, x, y);
	distances[x_y_i] = current_dist;

	if (current_dist >= *min_dist)
		return;

	if (x == X_END && y == Y_END) {
		*min_dist = current_dist;
		return;
	}

	int32_t next_dist = current_dist + 1;

	for (aoc_dir4_t dir = 0; dir < 4; ++dir) {
		int32_t x_next = x + aoc_dir4_x_diffs[dir];
		int32_t y_next = y + aoc_dir4_y_diffs[dir];
		if (x_next < 0 || x_next >= WIDTH || y_next < 0 || y_next >= HEIGHT)
			continue;

		size_t x_y_next_i = aoc_index_2d(WIDTH, x_next, y_next);
		if (distances[x_y_next_i] <= next_dist)
			continue;
		shortest_path(distances, x_next, y_next, next_dist, min_dist);
	}
}

static int64_t solve(char * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	size_t obstacles = (lines_n > PART1_OBSTACLES) ? PART1_OBSTACLES : lines_n;

	int32_t * distances = assert_calloc(WIDTH * HEIGHT, int32_t);

	for (size_t i = 0; i < obstacles; ++i) {
		int32_t x, y;
		if (sscanf(lines[i], "%" PRId32 ",%" PRId32, &x, &y) != 2) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			goto cleanup;
		}
		fprintf(stderr, "x: %" PRId32 ", y: %" PRId32 "\n", x, y);
		distances[aoc_index_2d(WIDTH, x, y)] = -1;
	}
	for (size_t y = 0; y < HEIGHT; ++y) {
		for (size_t x = 0; x < WIDTH; ++x) {
			size_t x_y_i = aoc_index_2d(WIDTH, x, y);
			if (distances[x_y_i] != -1)
				distances[x_y_i] = INT32_MAX;
		}
	}
	int32_t min_dist = INT32_MAX;
	shortest_path(distances, 0, 0, 0, &min_dist);
cleanup:
	free(distances);
	return min_dist;
}

int main(int argc, char * argv[]) {
	aoc_main_lines(argc, argv, 1, solve);
}
