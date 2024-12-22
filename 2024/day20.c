#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#include "aoc.h"

#define CHEAT_MIN 100

static int64_t solve(aoc_c32_2d_t maze, int32_t part, aoc_ex_t * e) {
	if (!aoc_c32_2d_check_bounded(maze, U'#')) {
		aoc_throw(e, AOC_EX_DATA_ERR, "matrix not bounded");
		return -1;
	}
	int32_t x_start, y_start, x_end, y_end;
	aoc_make_c32_2d_into_path_matrix(maze, &x_start, &y_start, &x_end, &y_end, e);
	if (*e) {
		return -1;
	}
	int32_t * path_matrix = (int32_t *)maze.ws;

	int32_t x = x_start, y = y_start;
	int32_t current_dist = 0;
	while (x != x_end || y != y_end) {
		// fprintf(stderr, "x: %"PRId32" y: %"PRId32"\n", x, y);
		int32_t valid_dir = -1;
		int32_t x_next;
		int32_t y_next;
		aoc_c32_2d_set(maze, x, y, current_dist);
		for (int32_t dir = 0; dir < 4; ++dir) {
			int32_t x_dir = x + aoc_dir4_x_diffs[dir];
			int32_t y_dir = y + aoc_dir4_y_diffs[dir];
			size_t x_y_dir_i = aoc_index_2d(maze.width, x_dir, y_dir);
			if (path_matrix[x_y_dir_i] == -1)
				continue;
			if (path_matrix[x_y_dir_i] == INT32_MAX) {
				if (valid_dir != -1) {
					fprintf(stderr, "x: %"PRId32" y: %"PRId32"\n", x_dir, y_dir);
					aoc_throw(e, AOC_EX_DATA_ERR, "multiple valid dirs");
					return -1;
				}
				valid_dir = dir;
				x_next = x_dir;
				y_next = y_dir;
			}
		}
		if (valid_dir == -1) {
			aoc_throw(e, AOC_EX_DATA_ERR, "no valid dirs");
			return -1;
		}
		for (int32_t sq = 0; sq < 8; ++sq) {
			int32_t x_sq = x + aoc_sq8_x_diffs[sq];
			int32_t y_sq = y + aoc_sq8_y_diffs[sq];
			size_t x_y_sq_i = aoc_index_2d(maze.width, x_sq, y_sq);
			if (path_matrix[x_y_sq_i] == -1)
				continue;
			if (path_matrix[x_y_sq_i] == current_dist - 1 || path_matrix[x_y_sq_i] == current_dist - 2)
				continue;
			if (abs(x_sq - x_next) > 1 || abs(y_sq - y_next) > 1) {
				fprintf(stderr, "x: %"PRId32" y: %"PRId32"\n", x, y);
				fprintf(stderr, "x: %"PRId32" y: %"PRId32"\n", x_sq, y_sq);
				aoc_throw(e, AOC_EX_DATA_ERR, "diagonal shortcuts!");
				return -1;
			}
		}
		valid_dir = -1;
		++current_dist;
		x = x_next;
		y = y_next;
	}
	aoc_c32_2d_set(maze, x_end, y_end, current_dist);

	// for (size_t y = 0; y < maze.height; ++y) {
	// 	for (size_t x = 0; x < maze.width; ++x) {
	// 		fprintf(stderr, "%d ", aoc_c32_2d_get(maze, x, y));
	// 	}
	// 	fputc('\n', stderr);
	// }

	int64_t cheats_found = 0;

	x = x_start;
	y = y_start;
	current_dist = 0;
	while (x != x_end || y != y_end) {
		int32_t x_next = -1, y_next = -1;
		for (int32_t dir = 0; dir < 4; ++dir) {
			int32_t x_dir = x + aoc_dir4_x_diffs[dir];
			int32_t y_dir = y + aoc_dir4_y_diffs[dir];
			size_t x_y_dir_i = aoc_index_2d(maze.width, x_dir, y_dir);

			if (path_matrix[x_y_dir_i] == current_dist + 1) {
				x_next = x_dir;
				y_next = y_dir;
			}

			int32_t x_cheat = x_dir + aoc_dir4_x_diffs[dir];
			int32_t y_cheat = y_dir + aoc_dir4_y_diffs[dir];
			if (!aoc_check_bounds(maze, x_cheat, y_cheat))
				continue;
			size_t x_y_cheat_i = aoc_index_2d(maze.width, x_cheat, y_cheat);
			if (path_matrix[x_y_cheat_i] - current_dist >= 2 + CHEAT_MIN) {
				// fprintf(stderr, "saves %"PRId32"\n", (path_matrix[x_y_cheat_i] - current_dist - 2));
				++cheats_found;
			}
		}
		if (x_next == -1) {
			aoc_throw_fail(e, "wut");
			return -1;
		}
		x = x_next;
		y = y_next;
		++current_dist;
	}

	return cheats_found;
}

int main(int argc, char * argv[]) {
	aoc_main_c32_2d(argc, argv, 1, solve);
}
