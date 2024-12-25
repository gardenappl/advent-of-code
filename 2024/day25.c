#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

#define ROWS 7
static_assert(ROWS <= INT_MAX, "unreasonable size");
#define COLUMNS 5
#define FILLED '#'
#define EMPTY '.'

typedef struct {
	char columns[COLUMNS];
} lock_or_key_t;

static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	lock_or_key_t locks[aoc_div_ceil(lines_n, ROWS + 1)];
	size_t locks_n = 0;

	lock_or_key_t keys[aoc_div_ceil(lines_n, ROWS + 1)];
	size_t keys_n = 0;

	for (size_t i = 0; i < lines_n; i += ROWS + 1) {
		if (line_lengths[i] < COLUMNS) {
			aoc_throw_invalid_file(e);
			return -1;
		}
		bool is_lock = true;
		for (size_t x = 0; x < COLUMNS; ++x) {
			if (lines[i][x] != FILLED) {
				is_lock = false;
				break;
			}
		}
		if (is_lock) {
			lock_or_key_t lock = { 0 };
			for (int y = 1; y < ROWS; ++y) {
				if (line_lengths[i + y] < COLUMNS) {
					aoc_throw_invalid_file(e);
					return -1;
				}
				for (size_t x = 0; x < COLUMNS; ++x) {
					if (lines[i + y][x] == FILLED)
						++lock.columns[x];
				}
			}
			locks[locks_n] = lock;
			++locks_n;
		} else {
			lock_or_key_t key = { 0 };
			for (int y = ROWS - 2; y >= 0; --y) {
				if (line_lengths[i + y] < COLUMNS) {
					aoc_throw_invalid_file(e);
					return -1;
				}
				for (size_t x = 0; x < COLUMNS; ++x) {
					if (lines[i + y][x] == FILLED)
						++key.columns[x];
				}
			}
			keys[keys_n] = key;
			++keys_n;
		}
	}

	int64_t fit_combos = 0;
	for (size_t lock_i = 0; lock_i < locks_n; ++lock_i) {
		for (size_t key_i = 0; key_i < keys_n; ++key_i) {
			for (size_t i = 0; i < COLUMNS; ++i) {
				if (locks[lock_i].columns[i] + keys[key_i].columns[i] > ROWS - 2)
					goto continue_next_combo;
			}
			fprintf(stderr, "key (%d,%d,%d,%d,%d) and lock (%d,%d,%d,%d,%d) fit.\n",
					keys[key_i].columns[0],
					keys[key_i].columns[1],
					keys[key_i].columns[2],
					keys[key_i].columns[3],
					keys[key_i].columns[4],
					locks[lock_i].columns[0],
					locks[lock_i].columns[1],
					locks[lock_i].columns[2],
					locks[lock_i].columns[3],
					locks[lock_i].columns[4]);
			++fit_combos;
continue_next_combo:;
		}
	}

	return fit_combos;
}


int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
