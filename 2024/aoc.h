#ifndef AOC_H
#define AOC_H

#include <stdnoreturn.h>
#include <assert.h>
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
#define AOC_MSG_OUT_OF_MEM "out of memory"
#define AOC_MSG_NO_WALL "field is missing wall"


/**
 * Turning non-string value of macro into string
 * https://gcc.gnu.org/onlinedocs/gcc-3.4.3/cpp/Stringification.html
 */
#define AOC_STR2(x) #x
#define AOC_STR(x) AOC_STR2(x)

/**
 * Overloading macro based on the number of arguments
 * https://stackoverflow.com/a/11763277
 * example use: #define FOO(...) GET_MACRO(__VA_ARGS__, FOO4, FOO3, FOO2)(__VA_ARGS__)
 */
#define GET_MACRO(_1,_2,_3,_4,NAME,...) NAME



/*
 *
 * Error handling
 *
 */

#define assert_calloc(num, type) \
	assert_calloc_(num, sizeof(type))

noreturn inline void oom_abort() {
	fputs(AOC_MSG_OUT_OF_MEM, stderr);
	abort();
}

inline void * assert_calloc_(size_t num, size_t size) {
	void * p = calloc(num, size);
	if (!p)
		oom_abort();
	return p;
}

#define assert_malloc(num, type) \
	assert_malloc_(num, sizeof(type))

inline void * assert_malloc_(size_t num, size_t size) {
	size_t size_total = num * size;
	if (size_total / size != num)
		oom_abort();
	void * p = malloc(size_total);
	if (!p)
		oom_abort();
	return p;
}


typedef struct aoc_ex * aoc_ex_t;
/**
 * @deprecated	Use aoc_ex_t * instead. 
 * @deprecated	Double pointers are nicer, catching an exception is as easy as checking `!*e`!
 */
typedef struct aoc_ex aoc_err_t;

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
#define aoc_throw_fail(e, msg) \
	aoc_throw(e, AOC_EX_SOFTWARE_FAIL, msg)

/**
 * Exception handling.
 * Sets e to an error, using a string literal as an error message.
 * The error code must be a non-zero value, should be one of AOC_EX_* codes.
 * The caller should clean up after themselves and return.
 */
#define aoc_throw(e, code, msg) \
	aoc_throw_(e, code, __FILE__ ":" AOC_STR(__LINE__) ": " msg)

void aoc_throw_(aoc_ex_t * e, int code, char const * msg);


/**
 * @deprecated	Since day 16.
 * For backwards compatibility.
 * Used by functions that only take singular pointer aka aoc_err_t *
 * MUST be called AFTER calling a function that accepts  aoc_ex_t *
 */
bool aoc_err_if_ex(aoc_err_t * err, aoc_ex_t * e);



/*
 *
 * Logic helpers
 *
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

#define aoc_check_bounds(matrix, x, y) \
	((x) >= 0 && (y) >= 0 && (x) < (matrix).width && (y) < (matrix).height)
#define aoc_div_ceil(a, b) \
	((a) / (b) + ((a) % (b) > 0))



/*
 *
 * File helpers
 *
 */

size_t aoc_count_lines(FILE * file);
void * aoc_read_file(FILE * file);
size_t aoc_get_until_newline(char ** buf, size_t * buf_size, FILE * file);



/*
 *
 * String helpers
 *
 */

/**
 * Counts number of occurences of c in a basic byte string.
 * To handle multi-byte strings, use aoc_c32_count.
 */
size_t aoc_count_chars(char const * s, char c);

/**
 * @deprecated	Since day 17. Use aoc_c32_split.
 * Warning: not the same as splitting a string, tokenizing will skip multiple whitespace characters and thus ignore empty fields.
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
 * Skips any non-digit characters before the first number, then only skips delimiters and whitespace.
 * @return	Amount of numbers successfully parsed to nums
 */
size_t aoc_numbers_line_parse(char const * s, char delimiter, int32_t * nums, size_t nums_buf_size);

/**
 * Get next char32_t in multibyte string s. Interface similar to strtol and friends.
 */
char32_t aoc_c32_get(char const * s, char const ** s_end, mbstate_t * state, aoc_ex_t * e);

/**
 * @deprecated	Since day 16 (due to old exception handling model, and juggling too many parameters). 
 * @deprecated	Currently no replacement, because I'm lazy.
 */
size_t aoc_c32_get_line(char32_t * ws_out, size_t * n, char const ** s_in, mbstate_t * state, aoc_err_t * err);

/**
 * Find the next occurence of a given char32_t in multibyte string s.
 * @param	s	Input string.
 * @param	c32	Character to find.
 * @param[out]	c	Result: pointer to first byte containing found character, or to the end of the string if c32 wasn't found.
 * @param[out]	c_next	Result: pointer to first byte containing following character, or to the end of the string if c32 wasn't found.
 * @param	state	Shift state.
 * @param	e	Exception.
 */
void aoc_c32_find(char const * s, char32_t c32, char const ** c, char const ** c_end, mbstate_t * state, aoc_ex_t * e);

/**
 * Counts the occurences of a given char32_t in multibyte string s.
 * @param	s	Input string.
 * @param	c32	Character to count.
 * @param	state	Shift state.
 * @param	e	Exception.
 */
size_t aoc_c32_count(char const * s, char32_t c32, mbstate_t * state, aoc_ex_t * e);

/**
 * Modifies the string by replacing delimiters with null bytes.
 * Is aware of multibyte encodings with shift sequences
 * (throws an exception if inserting a null byte would result in an invalid NTMBS).
 * @param	s	String to modify
 * @param	delimiter	Char to split on
 * @param	substrings	Array of pointers to resulting substarts, the array is expected to be of size at least 1 + aoc_c32_count(s, c32...)
 * @param	substring_lengths	Array of pointers to sizes of resulting substarts, the array is expected to be of size at least 1 + aoc_c32_count(s, c32...)
 * @param	substrings_count	Size of substrings and substring_lengths arrays; maximum amount of substrings that will be created.
 * @param	e	Exception
 * @return	Amount of substrings
 */
size_t aoc_c32_split(char * s, char32_t delimiter, char ** substrings, size_t * substring_lengths, size_t substrings_count, aoc_ex_t * e);

/**
 * @deprecated	Since day 16 (due to old exception handling model). 
 * @deprecated	Since it's mostly only used for debugging, you can use aoc_c32_fprint instead.
 * @param	str	String array of size at least MB_CUR_MAX + 1 (or MB_LEN_MAX + 1)
 */
void aoc_c32_to_str(char32_t c, char * str, aoc_err_t * err);

void aoc_c32_fprint(char32_t c, FILE * file, aoc_ex_t * e);



/*
 *
 * Matrix helpers
 *
 */

extern int32_t aoc_sq8_x_diffs[8];
extern int32_t aoc_sq8_y_diffs[8];
extern int32_t aoc_cross4_x_diffs[4];
extern int32_t aoc_cross4_y_diffs[4];

typedef enum { AOC_DIR4_UP, AOC_DIR4_RIGHT, AOC_DIR4_DOWN, AOC_DIR4_LEFT } aoc_dir4_t;

extern int32_t aoc_dir4_x_diffs[4];
extern int32_t aoc_dir4_y_diffs[4];
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

/**
 * @deprecated	Since day 16 (due to old exception handling model). 
 * @deprecated	Use aoc_c32_2d_new instead.
 */
aoc_c32_2d_t aoc_c32_2d_parse(char const * s, aoc_err_t * err);
aoc_c32_2d_t aoc_c32_2d_new(char const * s, aoc_ex_t * e);

aoc_c32_2d_t aoc_c32_2d_parse_bounded(char const * const * lines, size_t lines_n, char boundary, aoc_ex_t * err);

aoc_c32_2d_t aoc_c32_2d_copy(aoc_c32_2d_t matrix);

bool aoc_c32_2d_check_bounded(aoc_c32_2d_t matrix, char32_t boundary);

bool aoc_c32_2d_find(aoc_c32_2d_t matrix, char32_t c32, int32_t * x, int32_t * y);

inline char32_t aoc_c32_2d_get(aoc_c32_2d_t matrix, size_t x, size_t y) {
	return matrix.ws[aoc_index_2d(matrix.width, x, y)];
}

inline void aoc_c32_2d_set(aoc_c32_2d_t matrix, size_t x, size_t y, char32_t c) {
	matrix.ws[aoc_index_2d(matrix.width, x, y)] = c;
}

void aoc_c32_2d_fprint(aoc_c32_2d_t matrix, FILE * file, aoc_ex_t * e);


/**
 * Finds the shortest path.
 * Done with DFS because I'm too lazy to implement a set.
 * @param	distances	Initial matrix of nodes. Visitable nodes are expected to have value INT_MAX32, unvisitable nodes are expected to have value -1
 */
inline void aoc_shortest_path(int32_t * distances, int32_t x, int32_t y, 
		int32_t x_end, int32_t y_end, 
		int32_t width, int32_t height,
		int32_t (*get_next_dist)(int32_t current_dist, int32_t x, int32_t y, size_t dir),
		int32_t current_dist, int32_t * min_dist) {
	size_t x_y_i = aoc_index_2d(width, x, y);
	distances[x_y_i] = current_dist;

	if (current_dist >= *min_dist)
		return;

	if (x == x_end && y == y_end) {
		*min_dist = current_dist;
		return;
	}

	for (size_t dir = 0; dir < 4; ++dir) {
		int32_t next_dist = get_next_dist(current_dist, x, y, dir);
		int32_t x_next = x + aoc_dir4_x_diffs[dir];
		int32_t y_next = y + aoc_dir4_y_diffs[dir];
		if (x_next < 0 || x_next >= width || y_next < 0 || y_next >= height)
			continue;

		size_t x_y_next_i = aoc_index_2d(width, x_next, y_next);
		if (distances[x_y_next_i] <= next_dist)
			continue;
		aoc_shortest_path(distances, x_next, y_next, x_end, y_end, width, height, 
				get_next_dist, next_dist, min_dist);
	}
}

void aoc_make_c32_2d_into_path_matrix(aoc_c32_2d_t matrix, int32_t * x_start, int32_t * y_start, int32_t * x_end, int32_t * y_end, aoc_ex_t * e);



/**
 *
 * Bit array
 *
 */


typedef struct {
	char * data;
	size_t bits_count;
} aoc_bit_array_t;

/**
 * @deprecated	Since day 16 (due to old exception handling model). 
 * @deprecated	Use aoc_bit_array_new instead.
 */
aoc_bit_array_t aoc_bit_array_make(size_t bits_count, aoc_err_t * err);
/**
 * Throws an exception if calloc() fails, which is definitely overkill.
 * I was probably too eager to use my "exception handler" here.
 */
aoc_bit_array_t aoc_bit_array_new(size_t bits_count, aoc_ex_t * e);
aoc_bit_array_t aoc_bit_array_new_(size_t bits_count);
#define aoc_bit_array_new_1(s) \
	aoc_bit_array_new_(s)
#define aoc_bit_array_new_2(s, e) \
	aoc_bit_array_new(s, e)
#define aoc_bit_array_new(...) \
	GET_MACRO(__VA_ARGS__, _4, _3, aoc_bit_array_new_2, aoc_bit_array_new_1)(__VA_ARGS__)

/**
 * @deprecated	Since day 16 (due to old exception handling model). 
 * @deprecated	Currently no replacement, because I'm lazy.
 */
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
 * @deprecated	Renamed to aoc_bit_array_2d_fprint
 * @deprecated  to be more consistent with standard C library
 * @deprecated  and to allow exception handling.
 */
void aoc_bit_array_2d_print(aoc_bit_array_t bit_array, size_t width, char false_c, char true_c, FILE * file);

void aoc_bit_array_2d_fprint(aoc_bit_array_t bit_array, size_t width, char false_c, char true_c, FILE * file, aoc_ex_t * e);



/*
 *
 * Main function boilerplate
 *
 */

/**
 * @deprecated	Since day 7
 */
#define AOC_MSG_SIZE 100

/**
 * @deprecated	Since day 7
 */
int aoc_main(int argc, char * argv[], 
		char * (*solve1)(FILE *), 
		char * (*solve2)(FILE *));

/**
 * @deprecated	Since day 7
 */
char * aoc_solve_for_matrix(FILE * input, int64_t (*solve_for_matrix)(aoc_matrix_t));


/**
 * @deprecated	Since day 19 (due to wrong const-ness). 
 * @deprecated	Use aoc_solver_const_lines_t instead.
 */
typedef int64_t (*aoc_solver_lines_t)(char * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e);

typedef int64_t (*aoc_solver_const_lines_t)(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e);

/**
 * @deprecated	Since day 16 (due to old exception handling model, and it skipping lines). 
 * @deprecated	Use aoc_main_const_lines instead.
 */
int aoc_main_parse_lines(int argc, char ** argv, int32_t parts_implemented, 
		int64_t (*solve)(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err));

/**
 * @deprecated	Since day 19 (due to wrong const-ness). 
 * @deprecated	Use aoc_main_const_lines instead.
 */
int aoc_main_lines(int argc, char ** argv, int32_t parts_implemented, aoc_solver_lines_t solve);

int aoc_main_const_lines(int argc, char ** argv, int32_t parts_implemented, aoc_solver_const_lines_t solve);


typedef int64_t (*aoc_solver_c32_2d_t)(aoc_c32_2d_t matrix, int32_t part, aoc_ex_t * e);

/**
 * @deprecated	Since day 16 (due to old exception handling model). 
 * @deprecated	Use aoc_main_c32_2d instead.
 */
int aoc_main_parse_c32_2d(int argc, char ** argv, int32_t parts_implemented, 
		int64_t (*solve)(aoc_c32_2d_t matrix, int32_t part, aoc_err_t * err));

int aoc_main_c32_2d(int argc, char ** argv, int32_t parts_implemented, aoc_solver_c32_2d_t solve);

#endif /* end of include guard: AOC_H */
