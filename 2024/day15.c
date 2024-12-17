#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

#define C32_ROBOT U'@'
#define C32_WALL U'#'
#define C32_FLOOR U'.'
#define C32_BOX U'O'

static int64_t solve(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err) {
	aoc_c32_2d_t warehouse = aoc_c32_2d_parse_bounded(lines, lines_n, '#', err);
	if (aoc_is_err(err)) {
		return -1;
	}

	size_t robot_x = 0, robot_y = 0;

	char c32_str[MB_LEN_MAX + 1];
	for (size_t y = 0; y < warehouse.height; ++y) {
		for (size_t x = 0; x < warehouse.width; ++x) {
			char32_t c = aoc_c32_2d_get(warehouse, x, y);
			aoc_c32_to_str(c, c32_str, err);
			fprintf(stderr, "%s ", c32_str);
			if (c == C32_ROBOT) {
				robot_x = x;
				robot_y = y;
			}
		}
		fprintf(stderr, "\n");
	}

	for (size_t line_n = warehouse.height; line_n < lines_n; ++line_n) {
		char const * line = lines[line_n];
		char instruction;
		while ((instruction = *(line++))) {
			// fprintf(stderr, "Robot x: %zu y: %zu, Instruction: %c\n", robot_x, robot_y, instruction);
			for (size_t dir = 0; dir < 4; ++dir) {
				if (instruction == aoc_dir4_chars[dir]) {
					// Move robot and blocks
					
					char32_t tile;
					size_t next_x = robot_x + aoc_dir4_x_diffs[dir];
					size_t next_y = robot_y + aoc_dir4_y_diffs[dir];

					size_t x = next_x, y = next_y;
					bool found_free_spot = false;
					bool push_blocks = false;
					while (true) {
						tile = aoc_c32_2d_get(warehouse, x, y);
						switch (tile) {
							case C32_FLOOR:
								found_free_spot = true;
								// fallthrough
							case C32_WALL:
								goto after_spot_search;
							case C32_BOX:
								push_blocks = true;
						}
						x += aoc_dir4_x_diffs[dir];
						y += aoc_dir4_y_diffs[dir];
					}
				after_spot_search:
					if (found_free_spot) {
						if (push_blocks) {
							aoc_c32_2d_set(warehouse, x, y, C32_BOX);
						}
						aoc_c32_2d_set(warehouse, robot_x, robot_y, C32_FLOOR);
						aoc_c32_2d_set(warehouse, next_x, next_y, C32_ROBOT);
						robot_x = next_x;
						robot_y = next_y;
					}
					break;
				}
			}
		}
	}

	fprintf(stderr, "\nAfter:\n");

	size_t gps_sum = 0;
	for (size_t y = 0; y < warehouse.height; ++y) {
		for (size_t x = 0; x < warehouse.width; ++x) {
			char32_t c = aoc_c32_2d_get(warehouse, x, y);
			if (c == C32_BOX) {
				gps_sum += x + 100 * y;
			}

			aoc_c32_to_str(c, c32_str, err);
			fprintf(stderr, "%s ", c32_str);
		}
		fprintf(stderr, "\n");
	}


	
clean_warehouse:
	free(warehouse.ws);
	return gps_sum;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 1, solve);
}
