#ifndef AOC_H
#define AOC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define AOC_EXIT_USAGE 64
#define AOC_EXIT_NO_INPUT 66
#define AOC_EXIT_SOFTWARE_FAIL 70
#define AOC_EXIT_IO_ERROR 74

#define AOC_MSG_SIZE 100

// Logic helpers

#define AOC_COMPARE_DECLARE_FOR(type)  int aoc_compare_##type(void const * a, void const * b);

AOC_COMPARE_DECLARE_FOR(int32_t)

#define AOC_MOVE_VALUE_DECLARE_FOR(type)  void aoc_move_value_##type(type * array, size_t index_from, size_t index_to);

AOC_MOVE_VALUE_DECLARE_FOR(int32_t)

inline size_t aoc_index_2d(size_t width, size_t x, size_t y) {
	return width * y + x;
}


// File helpers

size_t aoc_count_lines(FILE * file);
void * aoc_read_file(FILE * file);
size_t aoc_get_until_newline(char ** buf, size_t * buf_size, FILE * file);


// String helpers

size_t aoc_count_chars(char const * s, char c);
size_t aoc_tokenize(char * s, char delimiter, char *** token_starts, size_t * tokens_buf_size);

typedef struct {
	char * buf;

	char ** token_starts;
	size_t tokens_buf_size;

	int32_t * numbers;
	size_t numbers_count;
} aoc_numbers_line;

void aoc_numbers_line_parse_new(char * s, aoc_numbers_line * num_line, char delimiter);
void aoc_numbers_line_free(aoc_numbers_line num_line);


// Matrix helpers

typedef struct {
	char * s;
	size_t width;
	size_t height;
} aoc_matrix_t;

extern size_t aoc_sq8_x_diffs[8];
extern size_t aoc_sq8_y_diffs[8];
extern size_t aoc_cross4_x_diffs[4];
extern size_t aoc_cross4_y_diffs[4];
extern size_t aoc_dir4_x_diffs[4];
extern size_t aoc_dir4_y_diffs[4];

bool aoc_matrix_init(char * s, aoc_matrix_t * matrix);
char aoc_matrix_get(aoc_matrix_t matrix, size_t x, size_t y);
void aoc_matrix_set(aoc_matrix_t matrix, size_t x, size_t y, char c);
bool aoc_matrix_bounded(aoc_matrix_t matrix, size_t x, size_t y);


// Main function boilerplate

int aoc_main(int argc, char * argv[], 
		char * (*solve1)(FILE * const), 
		char * (*solve2)(FILE * const));
char * aoc_solve_for_matrix(FILE * input, int64_t (*solve_for_matrix)(aoc_matrix_t));

#endif /* end of include guard: AOC_H */
