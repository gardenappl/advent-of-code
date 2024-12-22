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
	int32_t x1 = numeric_pad[p1].x;
	int32_t y1 = numeric_pad[p1].y;
	int32_t x2 = numeric_pad[p2].x;
	int32_t y2 = numeric_pad[p2].y;
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

static void get_dir_strings_for_dirpad_(char const * restrict prev_string, size_t str_i, alt_dir_strings_t * restrict alts) {
	size_t str_num = alts->num;

	char * string_start = alts->strings + aoc_index_2d(alts->buf_sizes, 0, str_num);
	char * string = alts->strings + aoc_index_2d(alts->buf_sizes, str_i, str_num);

	size_t prev_dirpad_pos = DIR_KEY_A;
	char dirpad_key;
	while ((dirpad_key = *prev_string)) {
		size_t dirpad_pos = dirpad_char_to_pos(dirpad_key);
		switch (get_possible_routes_dirpad(prev_dirpad_pos, dirpad_pos)) {
			case BOTH:
				++alts->num;
				char * alt_string_start = alts->strings + aoc_index_2d(alts->buf_sizes, 0, alts->num);
				strcpy(alt_string_start, string_start);
				char * alt_string = alts->strings + aoc_index_2d(alts->buf_sizes, str_i, alts->num);

				char * next_alt_string = get_dirs_for_key_pair_hv(alt_string, dir_pad, prev_dirpad_pos, dirpad_pos);
				ptrdiff_t alt_str_i = next_alt_string - alt_string;
				get_dir_strings_for_dirpad_(prev_string + 1, alt_str_i, alts);
				
				// fallthrough
			case VH:
				string = get_dirs_for_key_pair_vh(string, dir_pad, prev_dirpad_pos, dirpad_pos);
				break;
			case HV:
				string = get_dirs_for_key_pair_hv(string, dir_pad, prev_dirpad_pos, dirpad_pos);
				break;
		}
		prev_dirpad_pos = dirpad_pos;
		++prev_string;
	}
}

static void get_dir_strings_for_numpad_(char const * restrict prev_string, size_t prev_pos, size_t str_i, alt_dir_strings_t * restrict alts) {
	size_t str_num = alts->num;

	char * string_start = alts->strings + aoc_index_2d(alts->buf_sizes, 0, str_num);
	char * string = alts->strings + aoc_index_2d(alts->buf_sizes, str_i, str_num);

	char key;
	while ((key = *prev_string)) {
		// fprintf(stderr, "prev: ('%zu')'%s', s: '%s', i: %zu, num: %zu\n", prev_pos, prev_string, string_start, str_i, str_num);
		size_t pos = numpad_char_to_pos(key);
		char * next_string;
		switch (get_possible_routes_numpad(prev_pos, pos)) {
			case BOTH:
				++alts->num;
				char * alt_string_start = alts->strings + aoc_index_2d(alts->buf_sizes, 0, alts->num);
				// fprintf(stderr, "copying '%s' to i: %zu\n",string_start, alts->num);
				strcpy(alt_string_start, string_start);
				// fprintf(stderr, "got: '%s'\n", alt_string_start);
				char * alt_string = alts->strings + aoc_index_2d(alts->buf_sizes, str_i, alts->num);
				// fprintf(stderr, "next: '%s'\n", alt_string);

				char * next_alt_string = get_dirs_for_key_pair_hv(alt_string, numeric_pad, prev_pos, pos);
				// fprintf(stderr, "becomes: '%s'\n", alt_string_start);
				ptrdiff_t alt_str_i = next_alt_string - alt_string_start;
				// fprintf(stderr, "and iiiiIIIiiii '%zu' will always love you\n", alt_str_i);
				get_dir_strings_for_numpad_(prev_string + 1, pos, alt_str_i, alts);
				
				// fallthrough
			case VH:
				next_string = get_dirs_for_key_pair_vh(string, numeric_pad, prev_pos, pos);
				break;
			case HV:
				next_string = get_dirs_for_key_pair_hv(string, numeric_pad, prev_pos, pos);
				break;
		}
		str_i += next_string - string;
		string = next_string;
		prev_pos = pos;
		++prev_string;
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


static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	for (size_t i = 0; i < lines_n; ++i) {
		if (line_lengths[i] != NUM_LEY_LENGTH) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			goto cleanup;
		}
		fprintf(stderr, "numpad string: '%s'\n", lines[i]);

		alt_dir_strings_t alts = get_dir_strings_for_numpad(lines[i]);
		for (size_t alt_i = 0; alt_i < alts.count; ++alt_i) {
			fprintf(stderr, "alt: '%s'\n", alts.strings + aoc_index_2d(alts.buf_sizes, 0, alt_i));
		}
	}
cleanup:
	return -1;
}

int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
