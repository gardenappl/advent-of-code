#ifndef AOC_H
#define AOC_H

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define AOC_EXIT_USAGE 64
#define AOC_EXIT_NO_INPUT 66
#define AOC_EXIT_SOFTWARE_FAIL 70
#define AOC_EXIT_IO_ERROR 74

// Deprecated since day 7
#define AOC_MSG_SIZE 100



/*
 * Error handling
 */

typedef struct aoc_err aoc_err_t;

/**
 * Return true and initialize error struct, if errno is non-zero
 * @param[out]	err
 * @param	err_msg Error message string, which won't need to be free()d
 */
bool aoc_err_if_errno(aoc_err_t * err, char const * err_msg);

/**
 * Return true and initialize error struct, if errno is non-zero
 * @param[out]	err
 * @param	err_msg_buf  Error message string, which WILL be free()d
 */
bool aoc_err_if_errno_msg_buf(aoc_err_t * err, char * err_msg_buf);

/**
 * @param[out]	err
 * @param	err_msg  Error message string, which won't need to be free()d
 */
void aoc_err(aoc_err_t * err, char const * err_msg);

/**
 * @param[out]	err
 * @param	err_msg_buf  Error message string, which WILL be free()d
 */
void aoc_err_msg_buf(aoc_err_t * err, char * err_msg_buf);



/*
 * Logic helpers
 */

#define AOC_COMPARE_DECLARE_FOR(type)  int aoc_compare_##type(void const * a, void const * b);

AOC_COMPARE_DECLARE_FOR(int32_t)

#define AOC_MOVE_VALUE_DECLARE_FOR(type)  void aoc_move_value_##type(type * array, size_t index_from, size_t index_to);

AOC_MOVE_VALUE_DECLARE_FOR(int32_t)

inline size_t aoc_index_2d(size_t width, size_t x, size_t y) {
	return width * y + x;
}



/*
 * File helpers
 */

size_t aoc_count_lines(FILE * file);
void * aoc_read_file(FILE * file);
size_t aoc_get_until_newline(char ** buf, size_t * buf_size, FILE * file);



/*
 * String helpers
 */

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



/*
 * Matrix helpers
 */

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
bool aoc_matrix_is_bound(aoc_matrix_t matrix, size_t x, size_t y);
bool aoc_matrix_mkcopy(aoc_matrix_t src, aoc_matrix_t * dest);
inline void aoc_matrix_copy_data(aoc_matrix_t src, aoc_matrix_t const * dest) {
	memcpy(dest->s, src.s, (src.width + 1) * src.height);
}


/*
 * Main function boilerplate
 */

/**
 * \deprecated Since day 7
 */
int aoc_main(int argc, char * argv[], 
		char * (*solve1)(FILE *), 
		char * (*solve2)(FILE *));
/**
 * \deprecated Since day 7
 */
char * aoc_solve_for_matrix(FILE * input, int64_t (*solve_for_matrix)(aoc_matrix_t));

typedef int64_t (*aoc_solver_lines_t)(char const * const * lines, size_t lines_n, int32_t part, aoc_err_t * err);
int aoc_main_parse_lines(int argc, char ** argv, int32_t parts_implemented, aoc_solver_lines_t solve);

#endif /* end of include guard: AOC_H */
