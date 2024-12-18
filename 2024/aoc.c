#include "aoc.h"

#include <locale.h>
#include <uchar.h>
#include <wchar.h>
#include <assert.h>
#include <errno.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <limits.h>
#include <threads.h>

/*
 * Error handling
 */

extern void oom_abort();
extern void * assert_calloc_(size_t num, size_t size);
extern void * assert_malloc_(size_t num, size_t size);

struct aoc_ex {
	int code;
	bool check_errno;
	bool error_msg_const;
	char * error_msg;
};

static void free_err_msg(struct aoc_ex err) {
	if (err.error_msg && !err.error_msg_const)
		free(err.error_msg);
}

static void free_ex(struct aoc_ex * ex) {
	if (!ex)
		return;
	free_err_msg(*ex);
	free(ex);
}

static bool print_ex(struct aoc_ex * exception) {
	if (!exception || exception->code == EXIT_SUCCESS)
		return false;
	if (exception->check_errno) {
		perror(exception->error_msg);
		fputc('\n', stderr);
	} else {
		fputs(exception->error_msg, stderr);
		fputc('\n', stderr);
	}
	return true;
}


/**
 * Old error API
 */

bool aoc_err_if_errno(aoc_err_t * err, char const * err_msg) {
	if (errno) {
		aoc_err(err, err_msg);
		err->check_errno = true;
		return true;
	}
	return false;
}

void aoc_err(aoc_err_t * err, char const * error_msg) {
	free_err_msg(*err);
	aoc_err_t err_ = {
		.code = EXIT_FAILURE,
		.error_msg = (char *)error_msg,
		.error_msg_const = true
	};
	*err = err_;
}

bool aoc_is_err(aoc_err_t * err) {
	return err && err->code != EXIT_SUCCESS;
}


/**
 * New exception API
 */

static struct aoc_ex * make_ex_if_needed(struct aoc_ex ** e, int exit_code) {
	struct aoc_ex * exception = *e;
	if (!exception)
		exception = calloc(1, sizeof(struct aoc_ex));
	if (!exception)
		exit(exit_code);
	return exception;
}

void aoc_throw_(struct aoc_ex ** e, int code, char const * msg) {
	if (!e) {
		// don't report exception, just exit
		return;
	} 
	// fprintf(stderr, "'%s' %i %p\n", msg, code, e);
	struct aoc_ex * exception = make_ex_if_needed(e, code);
	exception->code = code;
	exception->error_msg = (char *)msg;
	exception->error_msg_const = true;
	*e = exception;
}

/**
 * Old and new API compat
 */

aoc_err_t * aoc_ex_to_err(struct aoc_ex ** e) {
	return make_ex_if_needed(e, EXIT_FAILURE);
}

bool err_then_aoc_throw(aoc_err_t ** e, aoc_err_t * err) {
	if (!e) {
		// don't report exception, just exit (if needed)
		if (err) {
			free(err);
			return true;
		}
		return false;
	}
	if (*e == err)
		return true;

	if (err) {
		if (err->code && !(*e)) {
			*e = err;
		} else {
			free(err);
		}
	}
	return *e != NULL;
}

bool aoc_err_if_ex(aoc_err_t * err, aoc_ex_t * e) {
	if (*e) {
		*err = **e;
		free(*e);
	}
	return err->code;
}



/**
 * Logic helpers
 */

#define AOC_DEFINE_HELPERS_FOR(type) \
	int aoc_compare_##type(void const * a, void const * b) { \
		type arg1 = *(type const *)a; \
		type arg2 = *(type const *)b; \
	\
		if (arg1 < arg2) return -1; \
		if (arg1 > arg2) return 1; \
		return 0; \
	} \
	\
	void aoc_move_value_##type(type * array, size_t index_from, size_t index_to) { \
		assert(index_to < index_from); \
		type prev_val = array[index_to]; \
		array[index_to] = array[index_from]; \
		for (size_t i = index_to; i < index_from; i++) { \
			type val = array[i + 1]; \
			array[i + 1] = prev_val; \
			prev_val = val; \
		} \
	} \
	\
	extern type aoc_min_##type(type a, type b); \
	extern type aoc_max_##type(type a, type b); \
	extern void aoc_swap_##type(type * a, type * b); \
	extern type aoc_mod_##type(type a, type b);

AOC_DEFINE_HELPERS_FOR(int32_t)
AOC_DEFINE_HELPERS_FOR(int64_t)


extern size_t aoc_index_2d(size_t width, size_t x, size_t y);


/*
 * File helpers
 */

size_t aoc_count_lines(FILE * file) {
	fseek(file, 0, SEEK_SET);
	if (ferror(file)) return -1;

	size_t line_count = 0;

	for (char c = getc(file); c != EOF; c = getc(file))
		if (c == '\n' && line_count < SIZE_MAX)
			line_count++;
	if (ferror(file)) return -1;

	fseek(file, 0, SEEK_SET);
	return line_count;
}

void * aoc_read_file(FILE * file) {
	fseek(file, 0, SEEK_END);
	if (ferror(file)) return NULL;

	long size = ftell(file);
	if (ferror(file)) return NULL;

	fseek(file, 0, SEEK_SET);
	if (ferror(file)) return NULL;

	if (size > SIZE_MAX - 1)
		return NULL;
	char * buf = malloc((size_t)(size + 1));
	size_t read = fread(buf, (size_t)size, 1, file);
	if (read != 1) {
		free(buf);
		return NULL;
	}
	buf[((size_t)size) - 1] = 0;
	return buf;
}

size_t aoc_get_until_newline(char ** restrict buf, size_t * restrict buf_size, FILE * restrict file) {
	if (*buf_size == 0 || !(*buf)) {
		*buf_size = 1;
		*buf = malloc(*buf_size);
	}
	size_t i = 0;
	int c;
	while ((c = getc(file)) != EOF) {
		if (c == '\n')
			break;
		if (i >= *buf_size) {
			*buf_size *= 2;
			*buf = realloc(*buf, *buf_size);
		}
		(*buf)[i] = c;
		i++;
	}
	if (ferror(file)) {
		free(buf);
		return -1;
	}
	(*buf)[i] = 0;
	return i;
}


/*
 * String helpers
 */

size_t aoc_count_chars(char const * s, char c) {
	size_t count = 0;
	for (; *s; s++) {
		if (*s == c)
			count++;
	}
	return count;
}

size_t aoc_tokenize(char * restrict s, char delimiter, char *** restrict token_starts, size_t * restrict tokens_buf_size) {
	size_t fields_n = aoc_count_chars(s, delimiter) + 1;
	char delim_string[2] = { delimiter, 0 };

	if (*token_starts == NULL || *tokens_buf_size == 0) {
		*tokens_buf_size = fields_n;
		*token_starts = malloc(sizeof(char*) * fields_n);
	} else if (*tokens_buf_size < fields_n) {
		*tokens_buf_size = fields_n;
		*token_starts = realloc(*token_starts, sizeof(char*) * fields_n);
	}

	size_t tokens_count = 0;
	char * next_token = strtok(s, delim_string);
	while (next_token) {
		(*token_starts)[tokens_count] = next_token;
		tokens_count++;
		next_token = strtok(NULL, delim_string);
	}
	return tokens_count;
}

void aoc_numbers_line_parse_new(char * s, aoc_numbers_line * num_line, char delimiter) {
	num_line->buf = s;
	size_t old_numbers_buf_size = num_line->tokens_buf_size;
	size_t token_count = aoc_tokenize(num_line->buf, delimiter, &num_line->token_starts, &num_line->tokens_buf_size);

	if (num_line->numbers == NULL || (old_numbers_buf_size != num_line->tokens_buf_size)) {
		num_line->numbers = realloc(num_line->numbers, sizeof(int32_t) * num_line->tokens_buf_size);
	}
	num_line->numbers_count = token_count;
	for (size_t i = 0; i < token_count; i++) {
		num_line->numbers[i] = atol(num_line->token_starts[i]);
	}
}

void aoc_numbers_line_free(aoc_numbers_line num_line) {
	free(num_line.token_starts);
	free(num_line.numbers);
}

size_t aoc_numbers_line_estimate_size(size_t longest_line_size) {
	return (longest_line_size + 1) / 2;
}

size_t aoc_numbers_line_parse(char const * s, char delimiter, int32_t * nums, size_t nums_buf_size) {
	if (nums_buf_size == 0)
		return 0;

	size_t nums_n = 0;
	do {
		char * s_parse_end;
		nums[nums_n] = (int32_t)strtol(s, &s_parse_end, 10);
		if (s == s_parse_end)
			break;
		++nums_n;
		s = s_parse_end;
	} while (nums_n < nums_buf_size);

	return nums_n;
}


char32_t aoc_c32_get(char const * str, char const ** str_end, mbstate_t * state, aoc_ex_t * e) {
	char32_t c;
	size_t rs = mbrtoc32(&c, str, MB_LEN_MAX, state);
	switch (rs) {
		case (size_t)-1:
			aoc_throw_fail(e, "encoding error");
			return c;
		case (size_t)-2:
			aoc_throw_fail(e, "should not need more than MB_LEN_MAX to parse mb string");
			return c;
		case (size_t)-3:
			aoc_throw_fail(e, "32-bit surrogate pair? shouldn't happen, unless it's not UTF-32...");
			return c;
	}
	if (str_end)
		*str_end += rs;
	return c;			
}


size_t aoc_c32_get_line(char32_t * ws_out, size_t * n, char const ** s_in, mbstate_t * state, aoc_err_t * err) {
	size_t len = 0;

	char32_t c32;
	char32_t * c32_out = ws_out ? ws_out : &c32;
	while (n == NULL || *n > 1) {
		*c32_out = aoc_c32_get(*s_in, s_in, state, &err);
		if (aoc_is_err(err))
			return 0;

		if (*c32_out == U'\n')
			*c32_out = U'\0';
		if (*c32_out == U'\0')
			return len;

		if (ws_out)
			++c32_out;
		++len;
		if (n)
			--(*n);
	}
	// Truncate if at end of buffer
	if (ws_out)
		*c32_out = U'\0';

	return len;
}

void aoc_c32_find(char const * restrict s, char32_t c32, 
		char const ** restrict c, char const ** restrict c_next, 
		mbstate_t * state, aoc_ex_t * e) {
	*c_next = s;
	while (**c_next) {
		*c = *c_next;
		char32_t cur_c32 = aoc_c32_get(*c, c_next, state, e);
		if (*e)
			return;
		if (cur_c32 == c32)
			return;
	}
}

size_t aoc_c32_count(char const * s, char32_t c32, mbstate_t * state, aoc_ex_t * e) {
	size_t count = 0;
	char const * c;
	char const * c_next = s;
	while (*c_next) {
		c = c_next;
		aoc_c32_find(c, c32, &c, &c_next, state, e);
		if (*e)
			return count;
		++count;
	}
	return count - 1;
}

size_t aoc_c32_split(char * s, char32_t delimiter, char ** substrings, size_t * substring_sizes, size_t substrings_count, aoc_ex_t * e) {
	substrings[0] = s;

	char * c;
	char * c_next = s;
	size_t i;
	for (i = 0; i < substrings_count - 1; ++i) {
		c_next = substrings[i];
		mbstate_t state = { 0 }, next_state = { 0 };
		while (*c_next) {
			c = c_next;
			char32_t c32 = aoc_c32_get(c, (char const **)&c_next, &next_state, e);
			
			if (*e)
				return i;
			if (c32 == delimiter) {
				if (!mbsinit(&state)) {
					aoc_throw(e, AOC_EX_DATA_ERR, "line in text file has invalid end for a standalone NTMBS");
					return i;
				} else if (!mbsinit(&next_state)) {
					aoc_throw(e, AOC_EX_DATA_ERR, "line in text file has invalid start for a standalone NTMBS");
					return i;
				}
				// Can split string
				substrings[i + 1] = c_next;
				substring_sizes[i] = c - substrings[i];
				*c = '\0';
				goto continue_substring;
			}
			state = next_state;
		}
	continue_substring: ;
	}
	substring_sizes[i] = strlen(c_next);
	return i + 1;
}

void aoc_c32_to_str(char32_t c, char * str, aoc_err_t * err) {
	mbstate_t state;
	memset(&state, 0, sizeof(state));

	size_t written = c32rtomb(str, c, &state);
	switch (written) {
		case 0:
			aoc_err(err, "32-bit surrogate pair?");
			return;
		case (size_t)-1:
			aoc_err_if_errno(err, "can't convert c32");
			return;
		default:
			str[written] = '\0';
			return;
	}
}

void aoc_c32_fprint(char32_t c, FILE * file, aoc_ex_t * e) {
	char c_str[MB_LEN_MAX];
	aoc_err_t * err = aoc_ex_to_err(e);
	aoc_c32_to_str(c, c_str, err);
	if (err_then_aoc_throw(e, err))
		return;
	fputs(c_str, file);
	if (ferror(file))
		aoc_throw(e, AOC_EX_IO_ERROR, AOC_MSG_IO_ERROR);
}


/*
 * Matrix helpers
 */

size_t aoc_sq8_x_diffs[] = { 0,  1,  1,  1,  0, -1, -1, -1 };
size_t aoc_sq8_y_diffs[] = { 1,  1,  0, -1, -1, -1,  0,  1 };
size_t aoc_cross4_x_diffs[] = {  1,  1, -1, -1 };
size_t aoc_cross4_y_diffs[] = { -1,  1,  1, -1 };
size_t aoc_dir4_x_diffs[] = {  0,  1,  0, -1 };
size_t aoc_dir4_y_diffs[] = { -1,  0,  1,  0 };
char aoc_dir4_chars[] = { '^', '>', 'v', '<' };


bool aoc_matrix_init(char * s, aoc_matrix_t * matrix) {
	char * next_line = strchr(s, '\n');
	if (!next_line) return EXIT_FAILURE;
	
	aoc_matrix_t m = { .s = s, .width = next_line - s };
	do {
		if ((next_line - s) != m.width)
			return EXIT_FAILURE;
		m.height++;
		s = next_line + 1;
	} while ((next_line = strchr(s, '\n')));

	if (strlen(s) != m.width)
		return EXIT_FAILURE;
	m.height++;
	*matrix = m;
	return EXIT_SUCCESS;
}

char aoc_matrix_get(aoc_matrix_t matrix, size_t x, size_t y) {
	return matrix.s[aoc_index_2d(matrix.width + 1, x, y)];
}

void aoc_matrix_set(aoc_matrix_t matrix, size_t x, size_t y, char c) {
	matrix.s[aoc_index_2d(matrix.width + 1, x, y)] = c;
}

bool aoc_matrix_is_bound(aoc_matrix_t matrix, size_t x, size_t y) {
	return x < matrix.width && y < matrix.height;
}

bool aoc_matrix_mkcopy(aoc_matrix_t src, aoc_matrix_t * dest) {
	char * new_s = malloc((src.width + 1) * src.height);
	if (!new_s)
		return EXIT_FAILURE;
	dest->s = new_s;
	aoc_matrix_copy_data(src, dest);
	dest->width = src.width;
	dest->height = src.height;
	return EXIT_SUCCESS;
}

extern void aoc_matrix_copy_data(aoc_matrix_t src, aoc_matrix_t const * dest);

aoc_c32_2d_t aoc_c32_2d_new(char const * s, aoc_ex_t * e) {
	aoc_c32_2d_t matrix = { 0 };
	mbstate_t state = { 0 };

	size_t height = aoc_c32_count(s, U'\n', &state, e) + 1;
	if (*e)
		return matrix;
	if (height > INT32_MAX) {
		aoc_throw(e, AOC_EX_DATA_ERR, "too big");
		return matrix;
	}
	matrix.height = height;

	char const * first_line = s;
	aoc_err_t * err = aoc_ex_to_err(e);
	state = (mbstate_t){ 0 };
	size_t width = aoc_c32_get_line(NULL, NULL, &first_line, &state, err);
	if (err_then_aoc_throw(e, err))
		return matrix;
	if (width > INT32_MAX) {
		aoc_throw(e, AOC_EX_DATA_ERR, "too big");
		return matrix;
	}
	matrix.width = width;

	int32_t matrix_chars_ = matrix.width * matrix.height + 1;
	if (matrix_chars_ > SIZE_MAX) {
		aoc_throw(e, AOC_EX_DATA_ERR, "too big");
		return matrix;
	}
	size_t matrix_chars = matrix_chars_;

	matrix.ws = assert_malloc(matrix_chars, char32_t);

	state = (mbstate_t){ 0 };
	int32_t line_n = 0;
	while (line_n < matrix.height) {
		err = aoc_ex_to_err(e);
		size_t line_len = aoc_c32_get_line(matrix.ws + matrix.width * line_n, &matrix_chars, &s, &state, err);
		if (err_then_aoc_throw(e, err)) 
			goto undo_matrix;

		if (line_len != matrix.width) {
			// fprintf(stderr, "Expected %" PRId32 ", got %zu\n", matrix.width, line_len);
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_MATRIX);
			goto undo_matrix;
		}
		++line_n;
	}
	if (*s != '\0') {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_MATRIX);
		goto undo_matrix;
	}
	return matrix;

undo_matrix:
	free(matrix.ws);
	return (aoc_c32_2d_t){ 0 };
}

aoc_c32_2d_t aoc_c32_2d_parse(char const * s, aoc_err_t * err) {
	struct aoc_ex * ex = NULL;
	aoc_c32_2d_t result = aoc_c32_2d_new(s, &ex);
	aoc_err_if_ex(err, &ex);
	return result;
}


aoc_c32_2d_t aoc_c32_2d_parse_bounded(char const * const * lines, size_t lines_n, char boundary, aoc_ex_t * e) {
	aoc_c32_2d_t matrix = { 0 };

	if (lines_n == 0) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
		return matrix;
	}
	char const * line = lines[0];

	// Parse first line, shouldn't have anything other than basic 8-bit chars
	char c;
	while ((c = *line)) {
		if (c != boundary) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_MATRIX);
			return matrix;
		}
		++matrix.width;
		++line;
	}
	if (matrix.width == 0) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_MATRIX);
		return matrix;
	}

	// First characters should also be just 8-bit chars
	matrix.height = 1;
	for (size_t i = 1; i < lines_n; ++i) {
		line = lines[i];

		if (line[0] == boundary)
			++matrix.height;
		else
			break;
	}
	
	matrix.ws = calloc(matrix.width * matrix.height, sizeof(char32_t));
	if (!matrix.ws) {
		aoc_throw_fail(e, AOC_MSG_OUT_OF_MEM);
		return matrix;
	}

	mbstate_t state = { 0 };

	char boundary_c32_parse[2] = { boundary, '\0' };
	char32_t boundary_c32;
	if (mbrtoc32(&boundary_c32, boundary_c32_parse, 2, &state) != 1) {
		aoc_throw_fail(e, "could not parse bounary char as char32_t?");
		goto undo_matrix;
	}

	size_t matrix_i = 0;
	for (size_t y = 0; y < matrix.height; ++y) {
		line = lines[y];
		state = (mbstate_t){ 0 };

		for (size_t x = 0; x < matrix.width; ++x) {
			char32_t c32 = aoc_c32_get(line, &line, &state, e);
			if (*e)
				goto undo_matrix;

			if (c32 == U'\0') {
				aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_MATRIX);
				goto undo_matrix;
			}
			matrix.ws[matrix_i] = c32;
			++matrix_i;
		}
		if (matrix.ws[matrix_i - 1] != boundary_c32) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_MATRIX);
			goto undo_matrix;
		}
	}
	// Parse first line, shouldn't have anything other than basic 8-bit chars
	line = lines[matrix.height - 1];
	while ((c = *line)) {
		if (c != '#') {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_MATRIX);
			return matrix;
		}
		++line;
	}

	return matrix;

undo_matrix:
	free(matrix.ws);
	return (aoc_c32_2d_t) { 0 };
}

bool aoc_c32_2d_check_bounded(aoc_c32_2d_t matrix, char32_t boundary) {
	for (size_t x = 0; x < matrix.width; ++x) {
		if (aoc_c32_2d_get(matrix, x, 0) != boundary)
			return false;
		if (aoc_c32_2d_get(matrix, x, matrix.height - 1) != boundary)
			return false;
	}
	for (size_t y = 0; y < matrix.height; ++y) {
		if (aoc_c32_2d_get(matrix, 0, y) != boundary)
			return false;
		if (aoc_c32_2d_get(matrix, matrix.width - 1, y) != boundary)
			return false;
	}
	return true;
}

bool aoc_c32_2d_find(aoc_c32_2d_t matrix, char32_t c32, int32_t * x, int32_t * y) {
	for (int32_t y_ = 0; y_ < matrix.height; ++y_) {
		for (int32_t x_ = 0; x_ < matrix.width; ++x_) {
			if (aoc_c32_2d_get(matrix, x_, y_) == c32) {
				*x = x_;
				*y = y_;
				return true;
			}
		}
	}
	return false;
}

extern char32_t aoc_c32_2d_get(aoc_c32_2d_t matrix, size_t x, size_t y);
extern void aoc_c32_2d_set(aoc_c32_2d_t matrix, size_t x, size_t y, char32_t c);

void aoc_c32_2d_fprint(aoc_c32_2d_t matrix, FILE * file, aoc_ex_t * e) {
	char c32_str[MB_LEN_MAX + 1];
	for (size_t y = 0; y < matrix.height; ++y) {
		for (size_t x = 0; x < matrix.width; ++x) {
			char32_t c = aoc_c32_2d_get(matrix, x, y);

			aoc_err_t * err = aoc_ex_to_err(e);
			aoc_c32_to_str(c, c32_str, err);
			if (err_then_aoc_throw(e, err))
				return;

			fprintf(file, "%s", c32_str);
		}
		fprintf(file, "\n");
	}
	if (ferror(file))
		aoc_throw(e, AOC_EX_IO_ERROR, "could not print matrix");
}



/**
 * Bit array
 */


aoc_bit_array_t aoc_bit_array_make(size_t bits_count, struct aoc_ex * err) {
	return aoc_bit_array_new(bits_count, &err);
}

aoc_bit_array_t (aoc_bit_array_new)(size_t bits_count, struct aoc_ex ** e) {
	aoc_bit_array_t bit_array = {
		.data = calloc(aoc_div_ceil(bits_count, CHAR_BIT), sizeof(char)),
		.bits_count = bits_count
	};
	if (!bit_array.data) {
		aoc_throw_fail(e, AOC_MSG_OUT_OF_MEM);
	}
	return bit_array;
}

aoc_bit_array_t aoc_bit_array_new_(size_t bits_count) {
	return (aoc_bit_array_t){
		.data = assert_calloc(aoc_div_ceil(bits_count, CHAR_BIT), char),
		.bits_count = bits_count
	};
}

aoc_bit_array_t aoc_bit_array_copy(aoc_bit_array_t bit_array, aoc_err_t * err) {
	size_t words_count = aoc_div_ceil(bit_array.bits_count, CHAR_BIT);
	aoc_bit_array_t bit_array2 = {
		.data = (char *)malloc(words_count),
		.bits_count = bit_array.bits_count
	};
	if (!bit_array2.data) {
		aoc_err(err, "Could not allocate bit array");
		return bit_array2;
	}
	memcpy(bit_array2.data, bit_array.data, words_count);
	return bit_array2;
}

extern bool aoc_bit_array_get(aoc_bit_array_t bit_array, size_t bit_index);
extern void aoc_bit_array_set(aoc_bit_array_t bit_array, size_t bit_index, bool b);
extern void aoc_bit_array_reset(aoc_bit_array_t bit_array);

void aoc_bit_array_2d_print(aoc_bit_array_t bit_array, size_t width, char false_c, char true_c, FILE * file) {
	size_t line_bits = 0;
	for (size_t i = 0; i < bit_array.bits_count; ++i) {
		bool b = aoc_bit_array_get(bit_array, i);

		fputc(b ? true_c : false_c, file);

		++line_bits;
		if (line_bits == width) {
			line_bits = 0;
			fputc('\n', file);
		}
	}
	fputc('\n', file);
}

void aoc_bit_array_2d_fprint(aoc_bit_array_t bit_array, size_t width, char false_c, char true_c, FILE * file, aoc_ex_t * e) {
	aoc_bit_array_2d_print(bit_array, width, false_c, true_c, file);
	if (ferror(file))
		aoc_throw(e, AOC_EX_IO_ERROR, AOC_MSG_IO_ERROR);
}


/*
 *
 * Main function boilerplate
 *
 */

char * aoc_solve_for_matrix(FILE * input, int64_t (*solve_for_matrix)(aoc_matrix_t)) {
	char * s = aoc_read_file(input);
	if (!s) return NULL;

	aoc_matrix_t matrix;
	bool err_ = aoc_matrix_init(s, &matrix);
	if (err_) {
		free((void *)s);
		return NULL;
	}
	int64_t result = solve_for_matrix(matrix);
	free((void *)s);

	char * buf = malloc(AOC_MSG_SIZE);
	snprintf(buf, AOC_MSG_SIZE, "%" PRId64, result);
	return buf;
}



int aoc_main_(int argc, char ** argv, int parts_implemented, void * env,
		void (*prepare)(void * env, FILE * file, char const * file_name, aoc_ex_t * e),
		int64_t (*solve)(void * env, int part, aoc_ex_t * e),
		void (*cleanup)(void * env)) {
	if (argc < 2) {
		char * prog_name = "PROGRAM_NAME";
		if (argc > 0)
			prog_name = argv[0];
		fprintf(stderr, "Usage: %s INPUT_FILE\n", prog_name);
		return AOC_EX_USAGE;
	}
	char const * file_name = argv[1];
	FILE * file = fopen(file_name, "r");
	if (!file) {
		perror("Could not open file");
		return AOC_EX_NO_INPUT;
	}

	struct aoc_ex * ex = NULL;
	prepare(env, file, file_name, &ex);
	if (print_ex(ex))
		goto cleanup;

	for (int part = 1; part <= parts_implemented; ++part) {
		int64_t result = solve(env, part, &ex);

		if (print_ex(ex)) {
			fprintf(stderr, "Part %" PRId32 " failure!\n", part);
			break;
		} else {
			fprintf(stderr, "Part %" PRId32 " success!\n%" PRId64 "\n", part, result);
		}
	}
	if (cleanup)
		cleanup(env);
cleanup:;
	free(env);
	int exit_code = ex ? ex->code : EXIT_SUCCESS;
	free_ex(ex);
	return exit_code;
}



typedef char * (*old_solver_file_t)(FILE * file);

typedef struct {
	char const * file_name;
	old_solver_file_t solve1;
	old_solver_file_t solve2;
} file_env_t;


static void file_prepare(void * env, FILE * file, char const * file_name, aoc_ex_t * e) {
	file_env_t * file_env = (file_env_t *)env;
	fclose(file);
	file_env->file_name = file_name;
}

static int64_t file_solve(void * env, int part, aoc_ex_t * e) {
	file_env_t * file_env = (file_env_t *)env;

	FILE * file = fopen(file_env->file_name, "r");
	char * result_s;
	if (part == 1) {
		result_s = file_env->solve1(file);
	} else {
		result_s = file_env->solve2(file);
	}
	fclose(file);
	if (!result_s) {
		aoc_throw_fail(e, "Fail!");
		return -1;
	}
	int64_t result;
	if (sscanf(result_s, "%" PRId64, &result) != 1) {
		aoc_throw_fail(e, "Could not parse output!");
		goto cleanup;
	}
cleanup:
	free(result_s);
	return result;
}

int aoc_main(int argc, char ** argv, old_solver_file_t solve1, old_solver_file_t solve2) {
	file_env_t * file_env = assert_calloc(1, file_env_t);
	file_env->solve1 = solve1;
	file_env->solve2 = solve2;
	return aoc_main_(argc, argv, solve2 ? 2 : 1, file_env,
			file_prepare, file_solve, NULL);
}


typedef struct {
	char * const * lines;
	size_t lines_n;
	size_t const * line_lengths;
	aoc_solver_lines_t solve;
} lines_env_t;


static void lines_prepare(void * env, FILE * file, char const * file_name, aoc_ex_t * e) {
	lines_env_t * lines_env = (lines_env_t *)env;

	char * input = aoc_read_file(file);
	fclose(file);
	if (!input) {
		aoc_throw(e, AOC_EX_IO_ERROR, AOC_MSG_IO_ERROR);
		return;
	}

	mbstate_t state = { 0 };
	size_t lines_n = aoc_c32_count(input, U'\n', &state, e) + 1;
	if (*e)
		goto undo_input;

	char ** lines = assert_calloc(lines_n, char *);
	size_t * line_lengths = assert_calloc(lines_n, size_t);

	// fprintf(stderr, "expecting %zu lines\n", lines_n);

	lines_env->lines_n = aoc_c32_split(input, U'\n', lines, line_lengths, lines_n, e);
	if (*e)
		goto undo_lines;

	lines_env->lines = (char * const *)lines;
	lines_env->line_lengths = (size_t const *)line_lengths;

	return;
undo_lines:
	free(lines);
	free(line_lengths);
undo_input:
	free(input);
}

static int64_t lines_solve(void * env_, int part, aoc_ex_t * e) {
	lines_env_t * env = (lines_env_t *)env_;
	return env->solve(env->lines, env->lines_n, env->line_lengths, part, e);
}

static void lines_cleanup(void * env_) {
	lines_env_t * env = (lines_env_t *)env_;
	free((void *)env->lines);
	free((void *)env->line_lengths);
}

int aoc_main_lines(int argc, char ** argv, int32_t parts_implemented, aoc_solver_lines_t solve) {
	setlocale(LC_ALL, "");

	lines_env_t * env = assert_calloc(1, lines_env_t);
	env->solve = solve;
	return aoc_main_(argc, argv, parts_implemented, env,
			lines_prepare, lines_solve, lines_cleanup);
}



typedef int64_t (*old_solver_lines_t)(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err);

typedef struct {
	char const * const * lines;
	size_t lines_n;
	size_t longest_line_size;
	old_solver_lines_t solve;
} old_lines_env_t;


static void old_lines_prepare(void * env_, FILE * file, char const * file_name, aoc_ex_t * e) {
	old_lines_env_t * env = (old_lines_env_t *)env_;
	char * input = aoc_read_file(file);
	fclose(file);
	if (!input) {
		aoc_throw(e, AOC_EX_IO_ERROR, AOC_MSG_IO_ERROR);
		return;
	}

	char ** lines_ = NULL;
	size_t lines_buf_size;
	size_t lines_n = aoc_tokenize(input, '\n', &lines_, &lines_buf_size);
	env->lines = (char const * const *)lines_;

	size_t longest_line_size = 0;
	for (size_t i = 0; i < lines_n; ++i) {
		size_t line_size = strlen(env->lines[i]);
		if (line_size > longest_line_size)
			longest_line_size = line_size;
	}
	env->lines_n = lines_n;
	env->longest_line_size = longest_line_size;
}

static int64_t old_lines_solve(void * env_, int part, aoc_ex_t * e) {
	old_lines_env_t * env = (old_lines_env_t *)env_;
	aoc_err_t * err = aoc_ex_to_err(e);
	int64_t result = env->solve(env->lines, env->lines_n, env->longest_line_size, part, err);
	if (err_then_aoc_throw(e, err)) {
		return -1;
	}
	return result;
}

static void old_lines_cleanup(void * env_) {
	old_lines_env_t * env = (old_lines_env_t *)env_;
	free((void *)env->lines);
}

int aoc_main_parse_lines(int argc, char ** argv, int32_t parts_implemented, old_solver_lines_t solve) {
	old_lines_env_t * env = assert_calloc(1, old_lines_env_t);
	env->solve = solve;
	return aoc_main_(argc, argv, parts_implemented, env,
			old_lines_prepare, old_lines_solve, old_lines_cleanup);
}



typedef int64_t (*old_solver_c32_2d_t)(aoc_c32_2d_t matrix, int32_t part, aoc_err_t * err);

typedef struct {
	aoc_c32_2d_t matrix;
	bool old;
	union {
		aoc_solver_c32_2d_t solve;
		old_solver_c32_2d_t solve_old;
	};
} c32_2d_env_t;


static void c32_2d_prepare(void * env_, FILE * file, char const * file_name, aoc_ex_t * e) {
	c32_2d_env_t * env = (c32_2d_env_t *)env_;
	char * input = aoc_read_file(file);
	fclose(file);
	if (!input) {
		aoc_throw(e, AOC_EX_IO_ERROR, AOC_MSG_IO_ERROR);
		return;
	}

	aoc_err_t * err = aoc_ex_to_err(e);
	env->matrix = aoc_c32_2d_parse(input, err);
	if (err_then_aoc_throw(e, err)) {
		goto cleanup_input;
	}
cleanup_input:
	free(input);
}

static int64_t c32_2d_solve(void * env_, int part, aoc_ex_t * e) {
	c32_2d_env_t * env = (c32_2d_env_t *)env_;

	int64_t result;
	if (env->old) {
		aoc_err_t * err = aoc_ex_to_err(e);
		result = env->solve_old(env->matrix, part, err);
		if (err_then_aoc_throw(e, err)) {
			return -1;
		}
	} else {
		result = env->solve(env->matrix, part, e);
	}
	return result;
}

static void c32_2d_cleanup(void * env_) {
	c32_2d_env_t * env = (c32_2d_env_t *)env_;
	free(env->matrix.ws);
}

int aoc_main_c32_2d(int argc, char ** argv, int32_t parts_implemented, aoc_solver_c32_2d_t solve) {
	setlocale(LC_ALL, "");

	c32_2d_env_t * env = assert_calloc(1, c32_2d_env_t);
	env->solve = solve;
	return aoc_main_(argc, argv, parts_implemented, env,
			c32_2d_prepare, c32_2d_solve, c32_2d_cleanup);
}

int aoc_main_parse_c32_2d(int argc, char ** argv, int32_t parts_implemented, old_solver_c32_2d_t solve) {
	setlocale(LC_ALL, "");

	c32_2d_env_t * env = assert_calloc(1, c32_2d_env_t);
	env->old = true;
	env->solve_old = solve;
	return aoc_main_(argc, argv, parts_implemented, env,
			c32_2d_prepare, c32_2d_solve, c32_2d_cleanup);
}
