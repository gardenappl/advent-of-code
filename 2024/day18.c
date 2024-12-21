#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

#define PART1_OBSTACLES 1024
#define WIDTH 71
#define HEIGHT 71
#define X_START 0
#define Y_START 0
#define X_END (WIDTH - 1)
#define Y_END (HEIGHT - 1)

static void shortest_path_(int32_t * distances, int32_t x, int32_t y, int32_t current_dist, int32_t * min_dist) {
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
		shortest_path_(distances, x_next, y_next, next_dist, min_dist);
	}
}

static int32_t shortest_path(int32_t * distances) {
	for (size_t x_y_i= 0; x_y_i < WIDTH * HEIGHT; ++x_y_i) {
		if (distances[x_y_i] != -1)
			distances[x_y_i] = INT32_MAX;
	}
	int32_t min_dist = INT32_MAX;
	shortest_path_(distances, X_START, Y_START, 0, &min_dist);
	return min_dist;
}

static bool walk_back(int32_t * distances, int32_t x, int32_t y, int32_t current_dist) {
	if (x == X_START && y == Y_START) {
		return true;
	}

	int32_t next_dist = current_dist - 1;
	if (next_dist == -1)
		return false;

	for (aoc_dir4_t dir = 0; dir < 4; ++dir) {
		int32_t x_next = x + aoc_dir4_x_diffs[dir];
		int32_t y_next = y + aoc_dir4_y_diffs[dir];
		if (x_next < 0 || x_next >= WIDTH || y_next < 0 || y_next >= HEIGHT)
			continue;

		size_t x_y_next_i = aoc_index_2d(WIDTH, x_next, y_next);
		if (distances[x_y_next_i] != next_dist)
			continue;
		bool walked = walk_back(distances, x_next, y_next, next_dist);
		if (walked)
			return true;
		// TODO: somehow gets stuck sometimes if I try any more recursions
		break; 
	}
	return false;
}

static int64_t solve(char * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	int32_t result = 0;

	int32_t * distances = assert_calloc(WIDTH * HEIGHT, int32_t);

	size_t obstacles = (lines_n > PART1_OBSTACLES) ? PART1_OBSTACLES : lines_n;
	for (size_t i = 0; i < obstacles; ++i) {
		int32_t x, y;
		if (sscanf(lines[i], "%" PRId32 ",%" PRId32, &x, &y) != 2) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			goto cleanup;
		}
		if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			goto cleanup;
		}
		fprintf(stderr, "x: %" PRId32 ", y: %" PRId32 "\n", x, y);
		distances[aoc_index_2d(WIDTH, x, y)] = -1;
	}

	int32_t min_dist = shortest_path(distances);
	bool can_walk_back = walk_back(distances, X_END, Y_END, min_dist);
	if (!can_walk_back) {
		aoc_throw_fail(e, "bruh");
		return result;
	}

	if (part == 1) {
		result = min_dist;
	} else {
		fprintf(stderr, "Shortest path has length %"PRId32"\n", min_dist);

		int32_t x = X_END, y = Y_END;
		for (size_t i = PART1_OBSTACLES; i < lines_n; ++i) {
			if (sscanf(lines[i], "%" PRId32 ",%" PRId32, &x, &y) != 2) {
				aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
				goto cleanup;
			}
			fprintf(stderr, "x: %" PRId32 ", y: %" PRId32 "\n", x, y);
			if (x < 0 || y < 0 || x >= WIDTH || y >= HEIGHT) {
				aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
				goto cleanup;
			}
			if (x == X_END && y == Y_END)
				break;
			distances[aoc_index_2d(WIDTH, x, y)] = -1;
			bool can_walk_back = walk_back(distances, X_END, Y_END, min_dist);
			if (can_walk_back) {
				fputs("old path still works\n", stderr);
				continue;
			}
			fputs("trying new path...", stderr);
			// reset and find another path
			min_dist = shortest_path(distances);
			if (min_dist == INT32_MAX) {
				fputs("blocked!\n", stderr);
				break;
			}
			fprintf(stderr, "found! new dist is: %"PRId32"\n", min_dist);
		}
		result = 100 * x + y;
	}
cleanup:
	free(distances);
	return result;
}

int main(int argc, char * argv[]) {
	aoc_main_lines(argc, argv, 2, solve);
}
