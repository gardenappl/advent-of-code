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
#define C32_BOXL U'['
#define C32_BOXR U']'



static void move_robot_boxes(aoc_c32_2d_t warehouse, size_t * robot_x, size_t * robot_y, aoc_dir4_t dir) {
	size_t next_x = *robot_x + aoc_dir4_x_diffs[dir];
	size_t next_y = *robot_y + aoc_dir4_y_diffs[dir];

	size_t x = next_x, y = next_y;
	bool found_free_spot = false;
	bool push_blocks = false;
	while (true) {
		char32_t tile = aoc_c32_2d_get(warehouse, x, y);
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
		aoc_c32_2d_set(warehouse, *robot_x, *robot_y, C32_FLOOR);
		aoc_c32_2d_set(warehouse, next_x, next_y, C32_ROBOT);

		*robot_x = next_x;
		*robot_y = next_y;
	}
}


static bool push_large_box_check(aoc_c32_2d_t warehouse, size_t x, size_t y, aoc_dir4_t dir) {
	char32_t tile, tile2;
	size_t y_next;
	switch (dir) {
		case AOC_DIR4_LEFT:
			tile = aoc_c32_2d_get(warehouse, x - 1, y);
			if (tile == C32_WALL) {
				return false;
			} else if (tile == C32_BOXR) {
				return push_large_box_check(warehouse, x - 2, y, dir);
			} else {
				return true;
			}
		case AOC_DIR4_RIGHT:
			tile = aoc_c32_2d_get(warehouse, x + 2, y);
			if (tile == C32_WALL) {
				return false;
			} else if (tile == C32_BOXL) {
				return push_large_box_check(warehouse, x + 2, y, dir);
			} else {
				return true;
			}
		case AOC_DIR4_UP:
		case AOC_DIR4_DOWN:
			y_next = y + aoc_dir4_y_diffs[dir];
			tile = aoc_c32_2d_get(warehouse, x, y_next);
			tile2 = aoc_c32_2d_get(warehouse, x + 1, y_next);
			if (tile == C32_WALL || tile2 == C32_WALL) {
				return false;
			} else {
				if (tile == C32_BOXR) {
					if (!push_large_box_check(warehouse, x - 1, y_next, dir))
						return false;
				}
				if (tile == C32_BOXL) {
					if (!push_large_box_check(warehouse, x,     y_next, dir))
						return false;
				} else if (tile2 == C32_BOXL) {
					if (!push_large_box_check(warehouse, x + 1, y_next, dir))
						return false;

				}
				return true;
			}
	}
	return false;
}


static void push_large_box(aoc_c32_2d_t warehouse, size_t x, size_t y, aoc_dir4_t dir) {
	char32_t tile, tile2;
	size_t y_next;
	switch (dir) {
		case AOC_DIR4_LEFT:
			tile = aoc_c32_2d_get(warehouse, x - 1, y);
			if (tile == C32_BOXR) {
				push_large_box(warehouse, x - 2, y, dir);
			}
			aoc_c32_2d_set(warehouse, x - 1, y, C32_BOXL);
			aoc_c32_2d_set(warehouse, x,     y, C32_BOXR);
			// no need to overwrite (x + 1, y) where the robot/previous boxes will be
			break;
		case AOC_DIR4_RIGHT:
			tile = aoc_c32_2d_get(warehouse, x + 2, y);
			if (tile == C32_BOXL) {
				push_large_box(warehouse, x + 2, y, dir);
			}

			aoc_c32_2d_set(warehouse, x + 1, y, C32_BOXL);
			aoc_c32_2d_set(warehouse, x + 2, y, C32_BOXR);
			// no need to overwrite (x, y) where the robot/previous boxes will be
			break;
		case AOC_DIR4_UP:
		case AOC_DIR4_DOWN:
			y_next = y + aoc_dir4_y_diffs[dir];
			tile = aoc_c32_2d_get(warehouse, x, y_next);
			tile2 = aoc_c32_2d_get(warehouse, x + 1, y_next);
			if (tile == C32_BOXR) {
				push_large_box(warehouse, x - 1, y_next, dir);
			}
			if (tile == C32_BOXL) {
				push_large_box(warehouse, x,     y_next, dir);
			} else if (tile2 == C32_BOXL) {
				push_large_box(warehouse, x + 1, y_next, dir);
			}
			aoc_c32_2d_set(warehouse, x,     y_next, C32_BOXL);
			aoc_c32_2d_set(warehouse, x + 1, y_next, C32_BOXR);
			aoc_c32_2d_set(warehouse, x,     y,      C32_FLOOR);
			aoc_c32_2d_set(warehouse, x + 1, y,      C32_FLOOR);
			break;
	}
}


static void move_robot_large_boxes(aoc_c32_2d_t warehouse, size_t * robot_x, size_t * robot_y, aoc_dir4_t dir) {
	size_t next_x = *robot_x + aoc_dir4_x_diffs[dir];
	size_t next_y = *robot_y + aoc_dir4_y_diffs[dir];

	char32_t tile = aoc_c32_2d_get(warehouse, next_x, next_y);
	switch (tile) {
		case C32_WALL:
			return;
		case C32_BOXL:
			if (push_large_box_check(warehouse, next_x, next_y, dir))
				push_large_box(warehouse, next_x, next_y, dir);
			else
				return;
			break;
		case C32_BOXR:
			if (push_large_box_check(warehouse, next_x - 1, next_y, dir))
				push_large_box(warehouse, next_x - 1, next_y, dir);
			else
				return;
			break;
	}
	aoc_c32_2d_set(warehouse, next_x, next_y, C32_ROBOT);
	aoc_c32_2d_set(warehouse, *robot_x, *robot_y, C32_FLOOR);
	*robot_x = next_x;
	*robot_y = next_y;
}


static int64_t solve(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err) {
	aoc_c32_2d_t warehouse = aoc_c32_2d_parse_bounded(lines, lines_n, '#', err);
	if (aoc_is_err(err)) {
		return -1;
	}

	aoc_ex_t e = NULL;
	if (part == 1) {
		aoc_c32_2d_fprint(warehouse, stderr, &e);
		if (aoc_err_if_ex(err, &e)) {
			goto clean_warehouse;
		}
	}

	size_t robot_x = 0, robot_y = 0;

	aoc_c32_2d_t widehouse;
	aoc_c32_2d_t * used_house;
	if (part == 2) {
		widehouse = (aoc_c32_2d_t){
			.ws = calloc(warehouse.width * 2 * warehouse.height, sizeof(char32_t)),
			.width = warehouse.width * 2,
			.height = warehouse.height
		};
		if (!widehouse.ws) {
			aoc_err(err, "out of memory");
			goto clean_warehouse;
		}

		for (int32_t y = 0; y < warehouse.height; ++y) {
			for (int32_t x = 0; x < warehouse.width; ++x) {
				char32_t wide_tile1;
				char32_t wide_tile2;

				char32_t tile = aoc_c32_2d_get(warehouse, x, y);
				switch (tile) {
					case C32_WALL:
						wide_tile1 = C32_WALL;
						wide_tile2 = C32_WALL;
						break;
					case C32_BOX:
						wide_tile1 = C32_BOXL;
						wide_tile2 = C32_BOXR;
						break;
					case C32_ROBOT:
						robot_x = x * 2;
						robot_y = y;
						wide_tile1 = C32_ROBOT;
						wide_tile2 = C32_FLOOR;
						break;
					default:
						wide_tile1 = C32_FLOOR;
						wide_tile2 = C32_FLOOR;
						break;
				}
				aoc_c32_2d_set(widehouse, x * 2, y, wide_tile1);
				aoc_c32_2d_set(widehouse, x * 2 + 1, y, wide_tile2);
			}
		}

		aoc_ex_t e = NULL;
		aoc_c32_2d_fprint(widehouse, stderr, &e);
		if (aoc_err_if_ex(err, &e)) {
			goto clean_widehouse;
		}
		used_house = &widehouse;
	} else if (part == 1) {
		for (size_t y = 0; y < warehouse.height; ++y) {
			for (size_t x = 0; x < warehouse.width; ++x) {
				char32_t c = aoc_c32_2d_get(warehouse, x, y);
				if (c == C32_ROBOT) {
					robot_x = x;
					robot_y = y;
				}
			}
		}
		used_house = &warehouse;
	}

	for (size_t line_n = warehouse.height; line_n < lines_n; ++line_n) {
		char const * line = lines[line_n];
		char instruction;
		while ((instruction = *(line++))) {
			// fprintf(stderr, "Robot x: %zu y: %zu, Instruction: %c\n", robot_x, robot_y, instruction);
			for (aoc_dir4_t dir = 0; dir < 4; ++dir) {
				if (instruction == aoc_dir4_chars[dir]) {
					if (part == 1)
						move_robot_boxes(warehouse, &robot_x, &robot_y, dir);
					else
						move_robot_large_boxes(widehouse, &robot_x, &robot_y, dir);
					break;
				}
			}
			// e = NULL;
			// aoc_c32_2d_fprint(*used_house, stderr, &e);
			// fgetc(stdin);
		}
	}

	fprintf(stderr, "\nAfter:\n");
	e = NULL;
	aoc_c32_2d_fprint(*used_house, stderr, &e);
	if (aoc_err_if_ex(err, &e)) {
		goto clean_widehouse;
	}

	size_t gps_sum = 0;
	for (size_t y = 0; y < used_house->height; ++y) {
		for (size_t x = 0; x < used_house->width; ++x) {
			char32_t c = aoc_c32_2d_get(*used_house, x, y);
			if (c == C32_BOX || c == C32_BOXL) {
				gps_sum += x + 100 * y;
			}
		}
	}


	
clean_widehouse:
	if (part == 2)
		free(widehouse.ws);
clean_warehouse:
	free(warehouse.ws);
	return gps_sum;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 2, solve);
}
