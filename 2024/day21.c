#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

typedef struct {
	int32_t x;
	int32_t y;
} coords_t;

#define MAX_MOVES_NUMERIC 6
#define MAX_MOVES_DIR 4
#define NUM_KEY_A 0xA
#define DIR_KEY_A 4
#define DIR_ROBOTS 3
#define NUM_LEY_LENGTH 4

static coords_t numeric_pad[11] = {
	(coords_t){ 1, 3 }, // 0
	(coords_t){ 0, 2 }, // 1
	(coords_t){ 1, 2 }, // 2
	(coords_t){ 2, 2 }, // 3
	(coords_t){ 0, 1 }, // 4
	(coords_t){ 1, 1 }, // 5
	(coords_t){ 2, 1 }, // 6
	(coords_t){ 0, 0 }, // 7
	(coords_t){ 1, 0 }, // 8
	(coords_t){ 2, 0 }, // 9
	(coords_t){ 2, 3 }, // A
};

static coords_t dir_pad[11] = {
	(coords_t){ 1, 0 }, // UP
	(coords_t){ 2, 1 }, // RIGHT
	(coords_t){ 1, 1 }, // DOWN
	(coords_t){ 0, 1 }, // LEFT
	(coords_t){ 2, 0 }, // A
};

static char * get_dirs_for_key_pair_hv(char * dir_string, coords_t * pad, size_t from, size_t to) {
	int32_t from_x = pad[from].x;
	int32_t from_y = pad[from].y;
	int32_t to_x = pad[to].x;
	int32_t to_y = pad[to].y;
	
	char * next = dir_string;
	while (from_x > to_x) {
		*next = aoc_dir4_chars[AOC_DIR4_LEFT];
		++next;
		--from_x;
	}
	while (from_x < to_x) {
		*next = aoc_dir4_chars[AOC_DIR4_RIGHT];
		++next;
		++from_x;
	}
	while (from_y > to_y) {
		*next = aoc_dir4_chars[AOC_DIR4_UP];
		++next;
		--from_y;
	}
	while (from_y < to_y) {
		*next = aoc_dir4_chars[AOC_DIR4_DOWN];
		++next;
		++from_y;
	}
	*next = 'A';
	++next;
	*next = '\0';
	return next;
}

static char * get_dirs_for_key_pair_vh(char * dir_string, coords_t * pad, size_t from, size_t to) {
	int32_t from_x = pad[from].x;
	int32_t from_y = pad[from].y;
	int32_t to_x = pad[to].x;
	int32_t to_y = pad[to].y;
	
	char * next = dir_string;
	while (from_y > to_y) {
		*next = aoc_dir4_chars[AOC_DIR4_UP];
		++next;
		--from_y;
	}
	while (from_y < to_y) {
		*next = aoc_dir4_chars[AOC_DIR4_DOWN];
		++next;
		++from_y;
	}
	while (from_x > to_x) {
		*next = aoc_dir4_chars[AOC_DIR4_LEFT];
		++next;
		--from_x;
	}
	while (from_x < to_x) {
		*next = aoc_dir4_chars[AOC_DIR4_RIGHT];
		++next;
		++from_x;
	}
	*next = 'A';
	++next;
	*next = '\0';
	return next;
}

static void get_dir_string_for_dirpad(char const * prev_dir_string, char * dir_string) {
	size_t prev_dirpad_pos = DIR_KEY_A;

	char dirpad_key;
	while ((dirpad_key = *prev_dir_string)) {
		size_t dirpad_pos;
		switch (dirpad_key) {
			case '^':
				dirpad_pos = AOC_DIR4_UP;
				break;
			case '>':
				dirpad_pos = AOC_DIR4_RIGHT;
				break;
			case 'v':
				dirpad_pos = AOC_DIR4_DOWN;
				break;
			case '<':
				dirpad_pos = AOC_DIR4_LEFT;
				break;
			default:
				dirpad_pos = DIR_KEY_A;
		}
		dir_string = get_dir_string_for_keys(dir_string, dir_pad, prev_dirpad_pos, dirpad_pos);
		prev_dirpad_pos = dirpad_pos;
		++prev_dir_string;
	}
}

static void get_dir_strings_for_numpad(char const * num_string, char ** dir_strings, size_t dir_robots) {
	char ** dir_string_next = assert_malloc(dir_robots, char *);
	for (size_t i = 0; i < dir_robots; ++i)
		dir_string_next[i] = dir_strings[i];

	size_t prev_numpad_pos = NUM_KEY_A;
	char numpad_key;
	while ((numpad_key = *num_string)) {
		size_t numpad_pos = isdigit(numpad_key) ? (numpad_key - '0') : NUM_KEY_A;
		dir_string_next[0] = get_dir_string_for_keys(dir_string_next[0], numeric_pad, prev_numpad_pos, numpad_pos);
		// fprintf(stderr, "dir string so far: '%s'\n", dir_strings[0]);
		prev_numpad_pos = numpad_pos;
		++num_string;
	}
	// fprintf(stderr, "dir string: '%s'\n", dir_strings[0]);
	for (size_t i = 1; i < dir_robots; ++i) {
		get_dir_string_for_dirpad(dir_strings[i - 1], dir_strings[i]);
	}
	free(dir_string_next);
}


static void debug_dir_execute(char const * dirpad_string, char * dirpad_output) {
	int32_t x = dir_pad[DIR_KEY_A].x;
	int32_t y = dir_pad[DIR_KEY_A].y;
	char dirpad_key;
	while ((dirpad_key = *dirpad_string)) {
		size_t dirpad_pos;
		switch (dirpad_key) {
			case '^':
				dirpad_pos = AOC_DIR4_UP;
				break;
			case '>':
				dirpad_pos = AOC_DIR4_RIGHT;
				break;
			case 'v':
				dirpad_pos = AOC_DIR4_DOWN;
				break;
			case '<':
				dirpad_pos = AOC_DIR4_LEFT;
				break;
			default:
				dirpad_pos = DIR_KEY_A;
		}
		if (dirpad_pos == DIR_KEY_A) {
			for (size_t i = 0; i < 4; ++i) {
				if (dir_pad[i].x == x && dir_pad[i].y == y) {
					*dirpad_output = aoc_dir4_chars[i];
					break;
				}
			}
			if (dir_pad[DIR_KEY_A].x == x && dir_pad[DIR_KEY_A].y == y) {
				*dirpad_output = 'A';
			}
			++dirpad_output;
		} else {
			x += aoc_dir4_x_diffs[dirpad_pos];
			y += aoc_dir4_y_diffs[dirpad_pos];
		}
		++dirpad_string;
	}
	*dirpad_output = '\0';
}


static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	char ** dir_strings = assert_malloc(DIR_ROBOTS, char *);
	size_t dir_string_max_size = NUM_LEY_LENGTH * MAX_MOVES_NUMERIC;
	for (size_t i = 0; i < DIR_ROBOTS; ++i) {
		dir_strings[i] = assert_malloc(dir_string_max_size + 1, char);
		dir_string_max_size *= MAX_MOVES_DIR;
	}

	int64_t complexity_sum = 0;

	for (size_t i = 0; i < lines_n; ++i) {
		if (line_lengths[i] != NUM_LEY_LENGTH) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			goto cleanup;
		}
		fprintf(stderr, "numpad string: '%s'\n", lines[i]);
		get_dir_strings_for_numpad(lines[i], dir_strings, DIR_ROBOTS);

		for (size_t dir_i = 0; dir_i < DIR_ROBOTS; ++dir_i)
			fprintf(stderr, "DirRobot %zu got: '%s' (len: %zu)\n", dir_i + 1, dir_strings[dir_i], strlen(dir_strings[dir_i]));
		complexity_sum += atoi(lines[i]) * strlen(dir_strings[DIR_ROBOTS - 1]);
	}

	debug_dir_execute(dir_strings[2], dir_strings[1]);
	fprintf(stderr, "MINE:  '%s'\n", dir_strings[1]);
	debug_dir_execute(dir_strings[1], dir_strings[0]);
	fprintf(stderr, "MINE:  '%s'\n", dir_strings[0]);
	fprintf(stderr, "MINE:  '%s'\n", dir_strings[2]);

	debug_dir_execute("<v<A>>^AvA^A<vA<AA>>^AAvA<^A>AAvA^A<vA>^AA<A>A<v<A>A>^AAAvA<^A>A", dir_strings[1]);
	fprintf(stderr, "DEBUG: '%s'\n", dir_strings[1]);
	debug_dir_execute(dir_strings[1], dir_strings[0]);
	fprintf(stderr, "DEBUG: '%s'\n", dir_strings[0]);
	get_dir_string_for_dirpad(dir_strings[1], dir_strings[2]);
	fprintf(stderr, "DEBUG: '%s'\n", dir_strings[2]);

cleanup:
	for (size_t i = 0; i < DIR_ROBOTS; ++i) {
		free(dir_strings[i]);
	}
	free(dir_strings);
	return complexity_sum;
}

int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
