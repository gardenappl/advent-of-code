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
#define MAX_MOVES 6

#define NUM_KEY_A 0xA
#define DIR_KEY_A 4
#define DIR_KEYS 5
#define NUM_STR_LENGTH 4

typedef enum { BOTH, VH, HV } route_t;

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

static void get_dirs_for_key_pair_hv(char * dir_string, coords_t * pad, size_t from, size_t to) {
	int32_t from_x = pad[from].x;
	int32_t from_y = pad[from].y;
	int32_t to_x = pad[to].x;
	int32_t to_y = pad[to].y;
	
	size_t i = 0;
	while (from_x > to_x) {
		dir_string[i] = aoc_dir4_chars[AOC_DIR4_LEFT];
		++i;
		--from_x;
	}
	while (from_x < to_x) {
		dir_string[i] = aoc_dir4_chars[AOC_DIR4_RIGHT];
		++i;
		++from_x;
	}
	while (from_y > to_y) {
		dir_string[i] = aoc_dir4_chars[AOC_DIR4_UP];
		++i;
		--from_y;
	}
	while (from_y < to_y) {
		dir_string[i] = aoc_dir4_chars[AOC_DIR4_DOWN];
		++i;
		++from_y;
	}
	dir_string[i] = 'A';
	++i;
	dir_string[i] = '\0';
}

static void get_dirs_for_key_pair_vh(char * dir_string, coords_t * pad, size_t from, size_t to) {
	int32_t from_x = pad[from].x;
	int32_t from_y = pad[from].y;
	int32_t to_x = pad[to].x;
	int32_t to_y = pad[to].y;
	
	size_t i = 0;
	while (from_y > to_y) {
		dir_string[i] = aoc_dir4_chars[AOC_DIR4_UP];
		++i;
		--from_y;
	}
	while (from_y < to_y) {
		dir_string[i] = aoc_dir4_chars[AOC_DIR4_DOWN];
		++i;
		++from_y;
	}
	while (from_x > to_x) {
		dir_string[i] = aoc_dir4_chars[AOC_DIR4_LEFT];
		++i;
		--from_x;
	}
	while (from_x < to_x) {
		dir_string[i] = aoc_dir4_chars[AOC_DIR4_RIGHT];
		++i;
		++from_x;
	}
	dir_string[i] = 'A';
	++i;
	dir_string[i] = '\0';
}

static size_t numpad_char_to_key(char c) {
	return (c == 'A') ? NUM_KEY_A : (c - '0');
}

static size_t dirpad_char_to_key(char c) {
	switch (c) {
		case '^':
			return AOC_DIR4_UP;
		case '>':
			return AOC_DIR4_RIGHT;
		case 'v':
			return AOC_DIR4_DOWN;
		case '<':
			return AOC_DIR4_LEFT;
		default:
			return DIR_KEY_A;
	}
}

static route_t get_possible_routes_numpad(size_t p1, size_t p2) {
	if ((p1 == 0 || p1 == NUM_KEY_A) && p2 % 3 == 1)	
		return VH;
	if ((p2 == 0 || p2 == NUM_KEY_A) && p1 % 3 == 1)
		return HV;
	int32_t x1 = numeric_pad[p1].x;
	int32_t y1 = numeric_pad[p1].y;
	int32_t x2 = numeric_pad[p2].x;
	int32_t y2 = numeric_pad[p2].y;
	return (x1 == x2 || y1 == y2) ? VH : BOTH;
}

static route_t get_possible_routes_dirpad(size_t p1, size_t p2) {
	if ((p1 == AOC_DIR4_UP || p1 == DIR_KEY_A) && p2 == AOC_DIR4_LEFT)
		return VH;
	if ((p2 == AOC_DIR4_UP || p2 == DIR_KEY_A) && p1 == AOC_DIR4_LEFT)	
		return HV;
	int32_t x1 = dir_pad[p1].x;
	int32_t y1 = dir_pad[p1].y;
	int32_t x2 = dir_pad[p2].x;
	int32_t y2 = dir_pad[p2].y;
	return (x1 == x2 || y1 == y2) ? VH : BOTH;
}

static int64_t get_min_len_for_dir_str(char const * str, size_t dir_robots, int64_t * memo);

static int64_t get_min_len_for_dir_pair(size_t d1, size_t d2, size_t dir_robots, int64_t * memo) {
	size_t memo_i = aoc_index_3d(DIR_KEYS, DIR_KEYS, d1, d2, dir_robots);
	int64_t result = memo[memo_i];
	if (result)
		return result;
	char str[MAX_MOVES_DIR + 1];
	switch (get_possible_routes_dirpad(d1, d2)) {
		case BOTH:
			get_dirs_for_key_pair_hv(str, dir_pad, d1, d2);
			result = get_min_len_for_dir_str(str, dir_robots, memo);

			get_dirs_for_key_pair_vh(str, dir_pad, d1, d2);
			result = aoc_min_int64_t(result, get_min_len_for_dir_str(str, dir_robots, memo));
			break;
		case VH:
			get_dirs_for_key_pair_vh(str, dir_pad, d1, d2);
			result = get_min_len_for_dir_str(str, dir_robots, memo);
			break;
		case HV:
			get_dirs_for_key_pair_hv(str, dir_pad, d1, d2);
			result = get_min_len_for_dir_str(str, dir_robots, memo);
			break;
	}
	memo[memo_i] = result;
	return result;
}

static int64_t get_min_len_for_dir_str(char const * str, size_t dir_robots, int64_t * memo) {
	if (dir_robots == 0) {
		// fprintf(stderr, "min len of %s is %zu\n", str, strlen(str));
		return strlen(str);
	}

	int64_t result = 0;

	size_t prev_key = DIR_KEY_A;
	char const * s = str;
	char c;
	while ((c = *s)) {
		size_t key = dirpad_char_to_key(c);
		result += get_min_len_for_dir_pair(prev_key, key, dir_robots - 1, memo);
		prev_key = key;
		++s;
	}
	// fprintf(stderr, "min len of %s (%zu robots!) is %zu\n", str, dir_robots, result);
	return result;
}


static int64_t get_min_len_for_num_str(char const * str, size_t dir_robots, int64_t * memo);

static int64_t get_min_len_for_num_pair(size_t n1, size_t n2, size_t dir_robots, int64_t * memo) {
	int64_t result;
	char str[MAX_MOVES_NUMERIC + 1];
	switch (get_possible_routes_numpad(n1, n2)) {
		case BOTH:
			get_dirs_for_key_pair_hv(str, numeric_pad, n1, n2);
			result = get_min_len_for_dir_str(str, dir_robots, memo);

			get_dirs_for_key_pair_vh(str, numeric_pad, n1, n2);
			result = aoc_min_int64_t(result, get_min_len_for_dir_str(str, dir_robots, memo));
			break;
		case VH:
			get_dirs_for_key_pair_vh(str, numeric_pad, n1, n2);
			result = get_min_len_for_dir_str(str, dir_robots, memo);
			break;
		case HV:
			get_dirs_for_key_pair_hv(str, numeric_pad, n1, n2);
			result = get_min_len_for_dir_str(str, dir_robots, memo);
			break;
	}
	return result;
}

static int64_t get_min_len_for_num_str(char const * str, size_t dir_robots, int64_t * memo) {
	int64_t result = 0;

	size_t prev_key = NUM_KEY_A;
	char const * s = str;
	char c;
	while ((c = *s)) {
		size_t key = numpad_char_to_key(c);
		result += get_min_len_for_num_pair(prev_key, key, dir_robots, memo);
		prev_key = key;
		++s;
	}
	return result;
}


static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	size_t dir_robots = (part == 1) ? 2 : 25;
	int64_t result = 0;

	int64_t * memo = assert_calloc(DIR_KEYS * DIR_KEYS * dir_robots, int64_t);

	for (size_t i = 0; i < lines_n; ++i) {
		if (line_lengths[i] != NUM_STR_LENGTH) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			return -1;
		}
		fprintf(stderr, "numpad string: '%s'\n", lines[i]);
		int64_t min_len = get_min_len_for_num_str(lines[i], dir_robots, memo);
		fprintf(stderr, "min len: %"PRId64"\n", min_len);
		result += min_len * atoi(lines[i]);
	}
	free(memo);
	return result;
}

int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 2, solve);
}
