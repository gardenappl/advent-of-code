#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#include "aoc.h"

typedef char32_t tile_t;

#define TILE_WALL U'#'
#define TILE_FLOOR U'.'
#define TILE_START U'S'
#define TILE_END U'E'

#define WEIGHT_WALK 1
#define WEIGHT_TURN 1000



static int64_t shortest_path(aoc_c32_2d_t maze, int32_t x, int32_t y, aoc_dir4_t dir, int64_t current_score, int64_t * dir_scores[4], int layers) {
	size_t x_y_i = aoc_index_2d(maze.width, x, y);
	int64_t min_score = dir_scores[dir][x_y_i];
	if (min_score > 0 && min_score <= current_score) {
		return -1;
	}
	dir_scores[dir][x_y_i] = current_score;
	if (maze.ws[x_y_i] == TILE_END) {
		// fprintf(stderr, "found it! %" PRId64 " layers: %d\n", current_score, layers);
		return current_score;
	}

	min_score = -1;

	// Forward
	int32_t next_x = x + aoc_dir4_x_diffs[dir];
	int32_t next_y = y + aoc_dir4_y_diffs[dir];
	tile_t next_tile = aoc_c32_2d_get(maze, next_x, next_y);
	if (next_tile != TILE_WALL) {
		min_score = shortest_path(maze, next_x, next_y, dir, current_score + WEIGHT_WALK, dir_scores, layers + 1);
	}

	// Clockwise or counter-clockwise
	aoc_dir4_t turn_dirs[2] = { (dir + 1) % 4, (dir + 3) % 4 };
	for (size_t turn = 0; turn < 2; ++turn) {
		int64_t score = shortest_path(maze, x, y, turn_dirs[turn], current_score + WEIGHT_TURN, dir_scores, layers + 1);
		if (score != -1 && (min_score == -1 || score < min_score))
			min_score = score;
	}
	return min_score;
}

static int64_t solve(aoc_c32_2d_t maze, int32_t part, aoc_ex_t * e) {
	if (!aoc_c32_2d_check_bounded(maze, TILE_WALL)) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_NO_WALL);
		return -1;
	}

	int32_t start_x, start_y;
	if (!aoc_c32_2d_find(maze, TILE_START, &start_x, &start_y)) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_MATRIX);
		return -1;
	}

	int64_t * dir_scores[4];
	int scores_init = 0;
	for (; scores_init < 4; ++scores_init) {
		dir_scores[scores_init] = calloc(maze.width * maze.height, sizeof(int64_t));
		if (!dir_scores[scores_init]) {
			aoc_throw_fail(e, AOC_MSG_OUT_OF_MEM);
			goto cleanup;
		}
		dir_scores[scores_init][aoc_index_2d(maze.width, start_x, start_y)] = -1;
	}

	int64_t min_score = shortest_path(maze, start_x, start_y, AOC_DIR4_RIGHT, 0, dir_scores, 0);

cleanup:
	for (--scores_init; scores_init >= 0; --scores_init) {
		free(dir_scores[scores_init]);
	}

	return min_score;
}

int main(int argc, char * argv[]) {
	aoc_main_c32_2d(argc, argv, 1, solve);
}
