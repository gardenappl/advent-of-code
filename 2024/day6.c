#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include "aoc.h"

int64_t solve_for_matrix1(aoc_matrix_t matrix) {
	size_t guard_x;
	size_t guard_y;
	size_t guard_dir = 0;
	for (size_t x = 0; x < matrix.width; x++) {
		for (size_t y = 0; y < matrix.height; y++) {
			if (aoc_matrix_get(matrix, x, y) == '^') {
				guard_x = x;
				guard_y = y;
				break;
			}
		}
	}

	size_t visited = 1;
	aoc_matrix_set(matrix, guard_x, guard_y, 'X');

	size_t next_guard_x = guard_x;
	size_t next_guard_y = guard_y - 1;
	while (aoc_matrix_is_bound(matrix, next_guard_x, next_guard_y)) {
		char next_cell = aoc_matrix_get(matrix, next_guard_x, next_guard_y);
		switch (next_cell) {
			case '#':
				guard_dir = (guard_dir + 1) % 4;
				break;
			case '.':
				aoc_matrix_set(matrix, next_guard_x, next_guard_y, 'X');
				visited++;
				// fprintf(stderr, "Visited: %zu\n%s\n", visited, matrix.s);
				// fallthrough
			case 'X':
				guard_x = next_guard_x;
				guard_y = next_guard_y;
				break;
		}
		next_guard_x = guard_x + aoc_dir4_x_diffs[guard_dir];
		next_guard_y = guard_y + aoc_dir4_y_diffs[guard_dir];
	}
	return visited;
}

static const char dir_flags[4] = { 0x01, 0x02, 0x04, 0x08 };


static int64_t count_loops(aoc_matrix_t matrix, aoc_matrix_t * matrix_block_copy, size_t guard_x, size_t guard_y, size_t guard_dir, bool main_path, bool * loop_blocks_checked) {
	int64_t loop_count = 0;

	while (true) {
		char cell = aoc_matrix_get(matrix, guard_x, guard_y);

		char cell_dir_flags = cell - '.';

		// Check if the cell was already walked over in this direction
		// This indicates a loop
		// Shouldn't happen on main path, so no need to return loop_count or deal with loop_blocks_checked
		if (cell_dir_flags & dir_flags[guard_dir]) {
			return 1;
		}
		// Mark current cell as having been walked over in this direction
		cell = '.' + (cell_dir_flags | dir_flags[guard_dir]);
		aoc_matrix_set(matrix, guard_x, guard_y, cell);

		// Try to move
		size_t next_guard_x = guard_x + aoc_dir4_x_diffs[guard_dir];
		size_t next_guard_y = guard_y + aoc_dir4_y_diffs[guard_dir];

		// Walked out
		if (!aoc_matrix_is_bound(matrix, next_guard_x, next_guard_y)) {
			return loop_count;
		}

		char next_cell = aoc_matrix_get(matrix, next_guard_x, next_guard_y);
		if (next_cell == '#' || next_cell == 'O') {
			guard_dir = (guard_dir + 1) % 4;
			// fprintf(stderr, "\n%s\nTurning!\n", matrix.s);
			// getc(stdin);
		} else {
			// Check for loops
			if (main_path && !loop_blocks_checked[aoc_index_2d(matrix.width, next_guard_x, next_guard_y)]) {
				// Try to place a block
				aoc_matrix_set(*matrix_block_copy, next_guard_x, next_guard_y, 'O');

				bool found_loop = count_loops(*matrix_block_copy, NULL, guard_x, guard_y, guard_dir, false, loop_blocks_checked);
				// if (found_loop) {
				// 	fprintf(stderr, "\n%s\nFound loop!\n", matrix_block_copy->s);
				// 	getc(stdin);
				// }
				loop_count += found_loop;

				// Reset the copy matrix
				aoc_matrix_copy_data(matrix, matrix_block_copy);

				// Don't try this obstacle anymore
				loop_blocks_checked[aoc_index_2d(matrix.width, next_guard_x, next_guard_y)] = true;
			}
			guard_x = next_guard_x;
			guard_y = next_guard_y;
		}
	}
}


int64_t solve_for_matrix2(aoc_matrix_t matrix) {
	size_t guard_x;
	size_t guard_y;
	for (size_t x = 0; x < matrix.width; x++) {
		for (size_t y = 0; y < matrix.height; y++) {
			if (aoc_matrix_get(matrix, x, y) == '^') {
				guard_x = x;
				guard_y = y;
				break;
			}
		}
	}
	if (!aoc_matrix_is_bound(matrix, guard_x, guard_y))
		return -1; // TODO: proper error reporting with this "framework"

	bool * loop_blocks = calloc(matrix.width * matrix.height, sizeof(bool));
	if (!loop_blocks)
		return -1; 

	aoc_matrix_t matrix_block_copy;
	if (aoc_matrix_mkcopy(matrix, &matrix_block_copy) != EXIT_SUCCESS) {
		free(loop_blocks);
		return -1;
	}

	aoc_matrix_set(matrix, guard_x, guard_y, '.');
	_Static_assert((int)'.' + 15 < 256, "Charset has 15 characters after .");

	int64_t loop_count = count_loops(matrix, &matrix_block_copy, guard_x, guard_y, 0, true, loop_blocks);

	free(loop_blocks);
	free(matrix_block_copy.s);
	fprintf(stderr, "\n%s\nEND\n", matrix.s);
	return loop_count;
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
