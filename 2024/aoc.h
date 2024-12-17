#ifndef AOC_H
#define AOC_H

#include <uchar.h>
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// See sysexits.h
// used as exception codes, and as program exit codes
#define AOC_EX_USAGE 64
#define AOC_EX_DATA_ERR 65
#define AOC_EX_NO_INPUT 66
#define AOC_EX_SOFTWARE_FAIL 70
#define AOC_EX_IO_ERROR 74

#define AOC_MSG_INVALID_FILE "invalid file"
#define AOC_MSG_INVALID_MATRIX "invalid matrix"
#define AOC_MSG_IO_ERROR "i/o error"


#define AOC_STR2(x) #x
#define AOC_STR(x) AOC_STR2(x)


/*
 * Error handling
 */

typedef struct aoc_ex aoc_err_t;
typedef struct aoc_ex * aoc_ex_t;

/**
 * @deprecated	Since day 16. Use aoc_throw instead
 * @param[out]	err
 * @param	err_msg  Error message string, which won't need to be free()d
 */
bool aoc_err_if_errno(aoc_err_t * err, char const * err_msg);

/**
 * @deprecated	Since day 16. Use aoc_throw instead
 * @param[out]	err
 * @param	err_msg  Error message string, which won't need to be free()d
 */
void aoc_err(aoc_err_t * err, char const * err_msg);


/**
 * @deprecated	Since day 16. With the new aoc_throw API, you can simply check if *e != NULL
 */
bool aoc_is_err(aoc_err_t * err);


/**
 * Exception handling.
 * Sets e to an error, using a string literal as an error message.
 * Uses the AOC_EX_SOFTWARE_FAIL error code.
 * The caller should clean up after themselves and return.
 */
#define aoc_throw_fail(e, msg)  aoc_throw(e, AOC_EX_SOFTWARE_FAIL, msg)

/**
 * Exception handling.
 * Sets e to an error, using a string literal as an error message.
 * The error code must be a non-zero value, should be one of AOC_EX_* codes.
 * The caller should clean up after themselves and return.
 */
#define aoc_throw(e, code, msg)  aoc_throw_(e, code, __FILE__ ":" AOC_STR(__LINE__) ": " msg)

void aoc_throw_(aoc_ex_t * e, int code, char const * msg);

/**
 * For backwards compatibility.
 * Used for CALLING functions that only take singular pointer.
 * MUST be called BEFORE calling a function that accepts aoc_err_t *
 */
aoc_err_t * aoc_ex_to_err(aoc_ex_t * e);

/**
 * For backwards compatibility.
 * Used for CALLING functions that only take singular pointer.
 * MUST be called AFTER calling a function that accepts aoc_err_t *
 */
bool aoc_throw_on_err(aoc_ex_t * e, aoc_err_t * err);

/**
 * For backwards compatibility.
 * Used FROM functions that only take singular pointer to aoc_err.
 * MUST be called AFTER calling a function that accepts aoc_ex_t * t
 */
bool aoc_err_if_ex(aoc_err_t * err, aoc_ex_t * e);


/*
 * Logic helpers
 */

#define AOC_DECLARE_HELPERS_FOR(type) \
	int aoc_compare_##type(void const * a, void const * b); \
	\
	void aoc_move_value_##type(type * array, size_t index_from, size_t index_to); \
	\
	inline type aoc_min_##type(type a, type b) { \
		return (a < b) ? a : b; \
	} \
	\
	inline type aoc_max_##type(type a, type b) { \
		return (a > b) ? a : b; \
	} \
	\
	inline void aoc_swap_##type(type * a, type * b) { \
		*a ^= *b; \
		*b ^= *a; \
		*a ^= *b; \
	} \
	\
	inline type aoc_mod_##type(type a, type b) { \
		type rem = a % b; \
		return (rem >= 0) ? rem : (b + rem); \
	}

AOC_DECLARE_HELPERS_FOR(int32_t)
AOC_DECLARE_HELPERS_FOR(int64_t)

inline size_t aoc_index_2d(size_t width, size_t x, size_t y) {
	return width * y + x;
}

#define aoc_check_bounds(matrix, x, y)  ((x) >= 0 && (y) >= 0 && (x) < (matrix).width && (y) < (matrix).height)
#define aoc_div_ceil(a, b)  ((a) / (b) + ((a) % (b) > 0))



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
/**
 * Warning: not the same as splitting a string, tokenizing will skip multiple whitespace characters and ignore empty fields.
 * Also it relies on strtok which is not thread-safe. Whoops.
 */
size_t aoc_tokenize(char * s, char delimiter, char *** token_starts, size_t * tokens_buf_size);

/**
 * @deprecated	Since day 7. It was a bad idea to mutate the buf, only because I
 * @deprecated	thought the stdlib tokenizer may have been convenient.
 * @deprecated	Also I should've added a _t suffix.
 * @deprecated	Conveniently this makes it easy to define a replacement type, if I decide to do that.
 * @deprecated	Current alternative is just aoc_numbers_line_parse and aoc_numbers_line_estimate_size
 */
typedef struct {
	char * buf;

	char ** token_starts;
	size_t tokens_buf_size;

	int32_t * numbers;
	size_t numbers_count;
} aoc_numbers_line;

/**
 * @deprecated	Since day 7.
 */
void aoc_numbers_line_parse_new(char * s, aoc_numbers_line * num_line, char delimiter);

/**
 * @deprecated	Since day 7.
 */
void aoc_numbers_line_free(aoc_numbers_line num_line);

/**
 * @return	Estimated buffer size for aoc_numbers_line_parse
 */
size_t aoc_numbers_line_estimate_size(size_t longest_line_size);

/**
 * @return	Amount of numbers successfully parsed to nums
 */
size_t aoc_numbers_line_parse(char const * s, char delimiter, int32_t * nums, size_t nums_buf_size);

/**
 * Get next char32_t in multibyte string s. Interface similar to strtol and friends.
 */
char32_t aoc_c32_get(char const * s, char const ** s_end, mbstate_t * state, aoc_ex_t * e);

size_t aoc_c32_get_line(char32_t * ws_out, size_t * n, char const ** s_in, mbstate_t * state, aoc_err_t * err);

/**
 * @param	str	String array of size at least MB_CUR_MAX + 1 (or MB_LEN_MAX + 1)
 */
void aoc_c32_to_str(char32_t c, char * str, aoc_err_t * err);


/*
 * Matrix helpers
 */


extern size_t aoc_sq8_x_diffs[8];
extern size_t aoc_sq8_y_diffs[8];
extern size_t aoc_cross4_x_diffs[4];
extern size_t aoc_cross4_y_diffs[4];

typedef enum { AOC_DIR4_UP, AOC_DIR4_RIGHT, AOC_DIR4_DOWN, AOC_DIR4_LEFT } aoc_dir4_t;

extern size_t aoc_dir4_x_diffs[4];
extern size_t aoc_dir4_y_diffs[4];
extern char aoc_dir4_chars[4];


/**
 * @deprecated	Since day 8. Use aoc_c32_2d_t instead if you want convenience and Unicode support.
 * @deprecated	aoc_matrix_t is neither convenient (due to having extraneous '\0' bytes at the end of each line)
 * @deprecated	nor does it handle multibyte strings.
 */
typedef struct {
	char * s;
	size_t width;
	size_t height;
} aoc_matrix_t;


/**
 * @deprecated	Since day 8. Use aoc_c32_2d_t instead.
 */
bool aoc_matrix_init(char * s, aoc_matrix_t * matrix);

/**
 * @deprecated	Since day 8. Use aoc_c32_2d_t instead.
 */
char aoc_matrix_get(aoc_matrix_t matrix, size_t x, size_t y);

/**
 * @deprecated	Since day 8. Use aoc_c32_2d_t instead.
 */
void aoc_matrix_set(aoc_matrix_t matrix, size_t x, size_t y, char c);

/**
 * @deprecated	Since day 8. Use aoc_check_bounds macro.
 */
bool aoc_matrix_is_bound(aoc_matrix_t matrix, size_t x, size_t y);

/**
 * @deprecated	Since day 8. Use aoc_check_bounds macro.
 */
bool aoc_matrix_mkcopy(aoc_matrix_t src, aoc_matrix_t * dest);

/**
 * @deprecated	Since day 8. Use aoc_check_bounds macro.
 */
inline void aoc_matrix_copy_data(aoc_matrix_t src, aoc_matrix_t const * dest) {
	memcpy(dest->s, src.s, (src.width + 1) * src.height);
}


/**
 * Unline aoc_matrix_t, doesn't have zero-delimiters between 'lines'. And handles Unicode, yay?
 */
typedef struct {
	char32_t * ws;
	int32_t width;
	int32_t height;
} aoc_c32_2d_t;

aoc_c32_2d_t aoc_c32_2d_parse(char const * s, aoc_err_t * err);
aoc_c32_2d_t aoc_c32_2d_parse_bounded(char const * const * lines, size_t lines_n, char boundary, aoc_err_t * err);
inline char32_t aoc_c32_2d_get(aoc_c32_2d_t matrix, size_t x, size_t y) {
	return matrix.ws[aoc_index_2d(matrix.width, x, y)];
}
inline void aoc_c32_2d_set(aoc_c32_2d_t matrix, size_t x, size_t y, char32_t c) {
	matrix.ws[aoc_index_2d(matrix.width, x, y)] = c;
}
void aoc_c32_2d_fprint(aoc_c32_2d_t matrix, FILE * file, aoc_ex_t * e);



/**
 * Bit array
 */


typedef struct {
	char * data;
	size_t bits_count;
} aoc_bit_array_t;

aoc_bit_array_t aoc_bit_array_make(size_t bits_count, aoc_err_t * err);
aoc_bit_array_t aoc_bit_array_copy(aoc_bit_array_t bit_array, aoc_err_t * err);
inline bool aoc_bit_array_get(aoc_bit_array_t bit_array, size_t bit_index) {
	size_t char_index = bit_index / CHAR_BIT;
	size_t bit_offset = bit_index % CHAR_BIT;
	return bit_array.data[char_index] & (1 << bit_offset);
}
inline void aoc_bit_array_set(aoc_bit_array_t bit_array, size_t bit_index, bool b) {
	size_t char_index = bit_index / CHAR_BIT;
	size_t bit_offset = bit_index % CHAR_BIT;
	char c = bit_array.data[char_index];
	if (b == true)
		c |= (1 << bit_offset);
	else
		c &= ~(1 << bit_offset);
	bit_array.data[char_index] = c;
}
inline void aoc_bit_array_reset(aoc_bit_array_t bit_array) {
	memset(bit_array.data, 0, aoc_div_ceil(bit_array.bits_count, CHAR_BIT));
}

/**
 * @deprecated	Renamed to aoc_bit_array_2d_printf,
 * @deprecated  to be more consistent with standard C library
 * @deprecated  and to allow exception handling.
 */
void aoc_bit_array_2d_print(aoc_bit_array_t bit_array, size_t width, char false_c, char true_c, FILE * file);

void aoc_bit_array_2d_printf(aoc_bit_array_t bit_array, size_t width, char false_c, char true_c, FILE * file, aoc_ex_t e);



/*
 * Main function boilerplate
 */

// Deprecated since day 7
#define AOC_MSG_SIZE 100

/**
 * @deprecated Since day 7
 */
int aoc_main(int argc, char * argv[], 
		char * (*solve1)(FILE *), 
		char * (*solve2)(FILE *));
/**
 * @deprecated Since day 7
 */
char * aoc_solve_for_matrix(FILE * input, int64_t (*solve_for_matrix)(aoc_matrix_t));

typedef int64_t (*aoc_solver_lines_t)(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err);
typedef int64_t (*aoc_solver_c32_2d_t)(aoc_c32_2d_t matrix, int32_t part, aoc_err_t * err);


int aoc_main_parse_lines(int argc, char ** argv, int32_t parts_implemented, aoc_solver_lines_t solve);
int aoc_main_parse_c32_2d(int argc, char ** argv, int32_t parts_implemented, aoc_solver_c32_2d_t solve);

#endif /* end of include guard: AOC_H */
