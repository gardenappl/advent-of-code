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
#define DIR_ROBOTS 2
#define NUM_LEY_LENGTH 4

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

static size_t get_dirs_for_key_pair_hv(char * dir_string, size_t i, coords_t * pad, size_t from, size_t to) {
	int32_t from_x = pad[from].x;
	int32_t from_y = pad[from].y;
	int32_t to_x = pad[to].x;
	int32_t to_y = pad[to].y;
	
	char * next = dir_string;
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
	return i;
}

static size_t get_dirs_for_key_pair_vh(char * dir_string, size_t i, coords_t * pad, size_t from, size_t to) {
	int32_t from_x = pad[from].x;
	int32_t from_y = pad[from].y;
	int32_t to_x = pad[to].x;
	int32_t to_y = pad[to].y;
	
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
	return i;
}

static size_t numpad_char_to_pos(char c) {
	return (c == 'A') ? NUM_KEY_A : (c - '0');
}

static size_t dirpad_char_to_pos(char c) {
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

static int64_t get_variants_count(char const * num_string, bool numpad) {
	int64_t variants = 1;
	size_t prev_pos = numpad ? NUM_KEY_A : DIR_KEY_A;
	char c;
	while ((c = *num_string)) {
		size_t pos = numpad ? numpad_char_to_pos(c) : dirpad_char_to_pos(c);
		if (numpad && get_possible_routes_numpad(prev_pos, pos) == BOTH)
			variants *= 2;
		else if (!numpad && get_possible_routes_dirpad(prev_pos, pos) == BOTH)
			variants *= 2;
		prev_pos = pos;
		++num_string;
	}
	return variants;
}

typedef struct alt_dir_strings {
	char * strings;
	size_t buf_sizes;
	size_t count;
	size_t num;
	struct alt_dir_strings * next_alts;
} alt_dir_strings_t;

static void get_dir_strings_for_dirpad_(char const * restrict last_string, size_t prev_pos, size_t str_i, alt_dir_strings_t * restrict alts) {
	size_t str_num = alts->num;

	char * string = alts->strings + aoc_index_2d(alts->buf_sizes, 0, str_num);

	char key;
	while ((key = *last_string)) {
		size_t pos = dirpad_char_to_pos(key);
		switch (get_possible_routes_dirpad(prev_pos, pos)) {
			case BOTH:
				++alts->num;
				char * alt_string = alts->strings + aoc_index_2d(alts->buf_sizes, 0, alts->num);
				strcpy(alt_string, string);

				size_t alt_str_i = get_dirs_for_key_pair_hv(alt_string, str_i, dir_pad, prev_pos, pos);
				get_dir_strings_for_dirpad_(last_string + 1, pos, alt_str_i, alts);
				
				// fallthrough
			case VH:
				str_i = get_dirs_for_key_pair_vh(string, str_i, dir_pad, prev_pos, pos);
				break;
			case HV:
				str_i = get_dirs_for_key_pair_hv(string, str_i, dir_pad, prev_pos, pos);
				break;
		}
		prev_pos = pos;
		++last_string;
	}
}

static void get_dir_strings_for_numpad_(char const * restrict num_string, size_t prev_pos, size_t str_i, alt_dir_strings_t * restrict alts) {
	size_t str_num = alts->num;

	char * string = alts->strings + aoc_index_2d(alts->buf_sizes, 0, str_num);

	char key;
	while ((key = *num_string)) {
		size_t pos = numpad_char_to_pos(key);
		switch (get_possible_routes_numpad(prev_pos, pos)) {
			case BOTH:
				++alts->num;
				char * alt_string = alts->strings + aoc_index_2d(alts->buf_sizes, 0, alts->num);
				strcpy(alt_string, string);

				size_t alt_str_i = get_dirs_for_key_pair_hv(alt_string, str_i, numeric_pad, prev_pos, pos);
				get_dir_strings_for_numpad_(num_string + 1, pos, alt_str_i, alts);
				
				// fallthrough
			case VH:
				str_i = get_dirs_for_key_pair_vh(string, str_i, numeric_pad, prev_pos, pos);
				break;
			case HV:
				str_i = get_dirs_for_key_pair_hv(string, str_i, numeric_pad, prev_pos, pos);
				break;
		}
		prev_pos = pos;
		++num_string;
	}
}

static alt_dir_strings_t get_dir_strings_for_numpad(char const * num_string) {
	size_t variants = get_variants_count(num_string, true);
	size_t buf_sizes = strlen(num_string) * MAX_MOVES_NUMERIC + 1;
	fprintf(stderr, "%zu variants, buf size: %zu\n", variants, buf_sizes);

	char * dir_strings = assert_calloc(variants * buf_sizes, char);
	alt_dir_strings_t alts = {
		.buf_sizes = buf_sizes,
		.num = 0,
		.count = variants,
		.strings = dir_strings,
		.next_alts = NULL
	};
	get_dir_strings_for_numpad_(num_string, NUM_KEY_A, 0, &alts);
	assert(alts.num == alts.count - 1);
	return alts;
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

static void get_dir_strings_for_dirpad_alts(alt_dir_strings_t * alts, size_t dir_robots) {
	alts->next_alts = assert_malloc(alts->count, alt_dir_strings_t);

	for (size_t i = 0; i < alts->count; ++i) {
		char * dir_string = alts->strings + aoc_index_2d(alts->buf_sizes, 0, i);
		size_t variants = get_variants_count(dir_string, false);
		size_t buf_sizes = alts->buf_sizes * MAX_MOVES_DIR + 1;
		fprintf(stderr, "%zu variants, buf size: %zu\n", variants, buf_sizes);

		char * dir_strings = assert_calloc(variants * buf_sizes, char);
		alt_dir_strings_t dir_alts = {
			.buf_sizes = buf_sizes,
			.num = 0,
			.count = variants,
			.strings = dir_strings,
			.next_alts = NULL
		};
		get_dir_strings_for_dirpad_(dir_string, DIR_KEY_A, 0, &dir_alts);
		assert(dir_alts.num == dir_alts.count - 1);
		alts->next_alts[i] = dir_alts;

		if (dir_robots > 1) {
			get_dir_strings_for_dirpad_alts(&alts->next_alts[i], dir_robots - 1);
		}
	}
}

static void free_alts(alt_dir_strings_t alts, size_t level, size_t levels) {
	// fprintf(stderr, "freeing %s\n", alts.strings);
	free(alts.strings);
	if (level < levels) {
		for (size_t i = 0; i < alts.count; ++i) {
			free_alts(alts.next_alts[i], level + 1, levels);
		}
		free(alts.next_alts);
	}
}

static void fprint(alt_dir_strings_t alts, size_t level, size_t levels, FILE * file) {
	for (size_t i = 0; i < alts.count; ++i) {
		char * string = alts.strings + aoc_index_2d(alts.buf_sizes, 0, i);
		for (size_t l = 0; l < level + 1; ++l)
			fputs("  ", file);
		fprintf(file, "* %s\n", string);

		if (level < levels) {
			fprint(alts.next_alts[i], level + 1, levels, file);
		}
	}
}

static int32_t min_length(alt_dir_strings_t alts, size_t level, size_t levels) {
	int32_t min = INT32_MAX;
	for (size_t i = 0; i < alts.count; ++i) {
		if (level < levels) {
			min = aoc_min_int32_t(min, min_length(alts.next_alts[i], level + 1, levels));
		} else {
			char * string = alts.strings + aoc_index_2d(alts.buf_sizes, 0, i);
			min = aoc_min_int32_t(min, strlen(string));
		}
	}
	return min;
}


static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	int64_t result = 0;

	for (size_t i = 0; i < lines_n; ++i) {
		if (line_lengths[i] != NUM_LEY_LENGTH) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			return -1;
		}
		fprintf(stderr, "numpad string: '%s'\n", lines[i]);

		alt_dir_strings_t alts = get_dir_strings_for_numpad(lines[i]);
		get_dir_strings_for_dirpad_alts(&alts, DIR_ROBOTS);

		fprint(alts, 0, DIR_ROBOTS, stderr);

		int32_t min = min_length(alts, 0, DIR_ROBOTS);
		fprintf(stderr, "min: %"PRId32"\n", min);
		result += min * atoi(lines[i]);

		free_alts(alts, 0, DIR_ROBOTS);
	}
	return result;
}

int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
