#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include "aoc.h"

int64_t solve_for_matrix1(aoc_s_matrix matrix) {
	size_t guard_x;
	size_t guard_y;
	size_t guard_dir = 0;
	for (size_t x = 0; x < matrix.width; x++) {
		for (size_t y = 0; y < matrix.height; y++) {
			if (aoc_s_matrix_get(matrix, x, y) == '^') {
				guard_x = x;
				guard_y = y;
				break;
			}
		}
	}

	size_t visited = 1;
	aoc_s_matrix_set(matrix, guard_x, guard_y, 'X');

	size_t next_guard_x = guard_x;
	size_t next_guard_y = guard_y - 1;
	while (aoc_s_matrix_bounded(matrix, next_guard_x, next_guard_y)) {
		char next_cell = aoc_s_matrix_get(matrix, next_guard_x, next_guard_y);
		switch (next_cell) {
			case '#':
				guard_dir = (guard_dir + 1) % 4;
				break;
			case '.':
				aoc_s_matrix_set(matrix, next_guard_x, next_guard_y, 'X');
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

static const char dir_codes[4] = { 0x01, 0x02, 0x04, 0x08 };

void trace_back(aoc_s_matrix matrix, size_t guard_x, size_t guard_y, size_t guard_dir, size_t * traced) {
	size_t opposite_dir = (guard_dir + 2) % 4;

	size_t behind_guard_x = guard_x;
	size_t behind_guard_y = guard_y;
	do {
		// trace to the right, if there's a wall to the left
		size_t wall_dir = (guard_dir + 3) % 4;
		size_t wall_x = behind_guard_x + aoc_dir4_x_diffs[wall_dir];
		size_t wall_y = behind_guard_y + aoc_dir4_y_diffs[wall_dir];
		if (aoc_s_matrix_bounded(matrix, wall_x, wall_y) && aoc_s_matrix_get(matrix, wall_x, wall_y) == '#') {
			trace_back(matrix, behind_guard_x, behind_guard_y, wall_dir, traced);
		}

		char behind_cell = aoc_s_matrix_get(matrix, behind_guard_x, behind_guard_y);
		if (behind_cell == '#')
			break;
		char behind_cell_dirs = behind_cell - '.';
		if (behind_cell_dirs & dir_codes[guard_dir])
			break;
		behind_cell = '.' + (behind_cell_dirs | dir_codes[guard_dir]);
		aoc_s_matrix_set(matrix, behind_guard_x, behind_guard_y, behind_cell);
		behind_guard_x = behind_guard_x + aoc_dir4_x_diffs[opposite_dir];
		behind_guard_y = behind_guard_y + aoc_dir4_y_diffs[opposite_dir];
		(*traced)++;
	} while (aoc_s_matrix_bounded(matrix, behind_guard_x, behind_guard_y));
}

int64_t solve_for_matrix2(aoc_s_matrix matrix) {
	bool * loop_blocks = calloc(matrix.width * matrix.height, sizeof(bool));
	size_t guard_x;
	size_t guard_y;
	size_t guard_dir = 0;
	for (size_t x = 0; x < matrix.width; x++) {
		for (size_t y = 0; y < matrix.height; y++) {
			if (aoc_s_matrix_get(matrix, x, y) == '^') {
				guard_x = x;
				guard_y = y;
				break;
			}
		}
	}

	size_t found_loops = 0;
	aoc_s_matrix_set(matrix, guard_x, guard_y, '.');
	_Static_assert((int)'.' + 15 < 256, "Charset has 15 characters after .");

	size_t next_guard_x = guard_x;
	size_t next_guard_y = guard_y - 1;
	while (aoc_s_matrix_bounded(matrix, next_guard_x, next_guard_y)) {
		// Set "trail" of cells behind the guard
		size_t traced = 0;
		trace_back(matrix, guard_x, guard_y, guard_dir, &traced);
		// if (traced > 1) {
			// fprintf(stderr, "\n%s\nTraced %zu cells!\n", matrix.s, traced);
			// getc(stdin);
		// }


		char next_cell = aoc_s_matrix_get(matrix, next_guard_x, next_guard_y);
		if (next_cell == '#') {
			guard_dir = (guard_dir + 1) % 4;
			// fprintf(stderr, "\n%s\nTurning!\n", matrix.s);
			// getc(stdin);
		} else {
			char cell = aoc_s_matrix_get(matrix, guard_x, guard_y);

			char next_cell_dirs = next_cell - '.';
			next_cell = '.' + (next_cell_dirs | dir_codes[guard_dir]);
			aoc_s_matrix_set(matrix, next_guard_x, next_guard_y, next_cell);

			guard_x = next_guard_x;
			guard_y = next_guard_y;

			// Check possible loop
			size_t guard_dir_turn = (guard_dir + 1) % 4;
			char cell_dirs = cell - '.';
			if (cell_dirs & dir_codes[guard_dir_turn]) {
				// aoc_s_matrix_set(matrix, next_guard_x, next_guard_y, 'O');
				// fprintf(stderr, "\n%s\nFOUND LOOP!\n", matrix.s);
				// aoc_s_matrix_set(matrix, next_guard_x, next_guard_y, next_cell);
				if (loop_blocks[aoc_index_2d(matrix.width, next_guard_x, next_guard_y)]) {
					fprintf(stderr, "Already found?!\n");
				} else {
					loop_blocks[aoc_index_2d(matrix.width, next_guard_x, next_guard_y)] = true;
					found_loops++;
				}
				// getc(stdin);
			}
		}


		next_guard_x = guard_x + aoc_dir4_x_diffs[guard_dir];
		next_guard_y = guard_y + aoc_dir4_y_diffs[guard_dir];
	}
	free(loop_blocks);
	fprintf(stderr, "\n%s\nEND\n", matrix.s);
	return found_loops;
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
