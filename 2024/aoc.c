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

struct aoc_err {
	bool is_error;
	bool check_errno;
	bool error_msg_const;
	char * error_msg;
};

static void free_err_msg(aoc_err_t err) {
	if (err.error_msg && !err.error_msg_const)
		free(err.error_msg);
}

bool aoc_err_if_errno(aoc_err_t * err, char const * err_msg) {
	if (errno) {
		aoc_err(err, err_msg);
		err->check_errno = true;
		return true;
	}
	return false;
}

bool aoc_err_if_errno_msg_buf(aoc_err_t * err, char * err_msg_buf) {
	if (errno) {
		aoc_err_msg_buf(err, err_msg_buf);
		err->check_errno = true;
		return true;
	}
	return false;
}

void aoc_err(aoc_err_t * err, char const * error_msg) {
	free_err_msg(*err);
	aoc_err_t err_ = {
		.is_error = true,
		.error_msg = (char *)error_msg,
		.error_msg_const = true
	};
	*err = err_;
}

void aoc_err_msg_buf(aoc_err_t * err, char * error_msg_buf) {
	free_err_msg(*err);
	aoc_err_t err_ = {
		.is_error = true,
		.error_msg = error_msg_buf
	};
	*err = err_;
}

bool aoc_is_err(aoc_err_t * err) {
	return err && err->is_error;
}


static bool print_err(aoc_err_t err) {
	if (!err.is_error)
		return false;
	if (err.check_errno) {
		perror(err.error_msg);
	} else {
		fputs(err.error_msg, stderr);
	}
	return true;
}



/**
 * Logic helpers
 */

#define AOC_DEFINE_ARITHMETIC_HELPERS_FOR(type) \
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

AOC_DEFINE_ARITHMETIC_HELPERS_FOR(int32_t)


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
			// fprintf(stderr, "string realloc: %p %zu\n", *buf, *buf_size);
			*buf = realloc(*buf, *buf_size);
			// fprintf(stderr, "string after realloc: %p %zu\n", *buf, *buf_size);
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
	// fprintf(stderr, "delim: '%s', fields: %zu\n", delim_string, fields_n);

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


size_t aoc_c32_get_line(char32_t * ws_out, size_t * n, char const ** s_in, mbstate_t * state, aoc_err_t * err) {
	size_t len = 0;

	char32_t c32;
	char32_t * c32_out = ws_out ? ws_out : &c32;
	// char print_c32[MB_LEN_MAX + 1];
	while (n == NULL || *n > 1) {
		size_t rc = mbrtoc32(c32_out, *s_in, 4, state);
		switch (rc) {
			case (size_t)-1:
				aoc_err_if_errno(err, "Encoding error!");
				return rc;
			case (size_t)-2:
				// more than 4 bytes needed? Weird but okay
				continue;
			case (size_t)-3:
				aoc_err(err, "32-bit surrogate pair? Shouldn't happen, unless it's not UTF-32...");
				return rc;
			case 0:
				goto end_line;
			default:
				// aoc_c32_to_str(*c32_out, print_c32, err);
				// fprintf(stderr, "Got char: '%s'\n", print_c32);
				*s_in += rc;
				if (*c32_out == U'\n')
					goto end_line;
				if (ws_out) {
					++c32_out;
					// fprintf(stderr, "Next will write to %p\n", c32_out);
				}
				++len;
				if (n)
					--(*n);
		}
	}
end_line:
	// fprintf(stderr, "End line\n");
	if (ws_out)
		*(c32_out + 1) = U'\0';
	return len;
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


/*
 * Matrix helpers
 */

size_t aoc_sq8_x_diffs[] = { 0,  1,  1,  1,  0, -1, -1, -1 };
size_t aoc_sq8_y_diffs[] = { 1,  1,  0, -1, -1, -1,  0,  1 };
size_t aoc_cross4_x_diffs[] = {  1,  1, -1, -1 };
size_t aoc_cross4_y_diffs[] = { -1,  1,  1, -1 };
size_t aoc_dir4_x_diffs[] = {  0,  1,  0, -1 };
size_t aoc_dir4_y_diffs[] = { -1,  0,  1,  0 };


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

aoc_c32_2d_t aoc_c32_2d_parse(char const * s, aoc_err_t * err) {
	aoc_c32_2d_t matrix = { 0 };
	size_t height = aoc_count_chars(s, '\n') + 1;
	if (height > INT32_MAX) {
		aoc_err(err, "Too tall");
		return matrix;
	}
	matrix.height = height;

	mbstate_t state;
	memset(&state, 0, sizeof(mbstate_t));

	char const * first_line = s;
	size_t width = aoc_c32_get_line(NULL, NULL, &first_line, &state, err);
	if (err->is_error)
		return matrix;
	if (width > INT32_MAX) {
		aoc_err(err, "Too wide");
		return matrix;
	}
	matrix.width = width;

	size_t matrix_chars = matrix.width * matrix.height + 1;

	matrix.ws = calloc(matrix_chars, sizeof(char32_t));
	if (!matrix.ws) {
		aoc_err(err, "couldn't allocate 32-bit matrix");
		return matrix;
	}

	memset(&state, 0, sizeof(mbstate_t));

	int32_t line_n = 0;
	while (line_n < matrix.height) {
		size_t line_len = aoc_c32_get_line(matrix.ws + matrix.width * line_n, &matrix_chars, &s, &state, err);
		if (err->is_error) goto cleanup_matrix;

		if (line_len != matrix.width) {
			fprintf(stderr, "Expected %" PRId32 ", got %zu\n", matrix.width, line_len);
			aoc_err(err, "Wrong width of input line");
			goto cleanup_matrix;
		}
		++line_n;
	}
	if (*s != '\0') {
		aoc_err(err, "Incorrectly parsed height of input");
		goto cleanup_matrix;
	}
	return matrix;

cleanup_matrix:
	free(matrix.ws);
	return matrix;
}
extern char32_t aoc_c32_2d_get(aoc_c32_2d_t matrix, size_t x, size_t y);
extern void aoc_c32_2d_set(aoc_c32_2d_t matrix, size_t x, size_t y, char32_t c);



/**
 * Bit array
 */


aoc_bit_array_t aoc_bit_array_make(size_t bits_count, aoc_err_t * err) {
	aoc_bit_array_t bit_array = {
		.data = (char *)calloc(aoc_div_ceil(bits_count, CHAR_BIT), sizeof(char)),
		.bits_count = bits_count
	};
	if (!bit_array.data) {
		aoc_err(err, "Could not allocate bit array");
	}
	return bit_array;
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



/*
 * Main function boilerplate
 */

static int parse_args_and_open(int argc, char ** argv, FILE ** input_file) {
	if (argc < 2) {
		char * prog_name = "PROGRAM_NAME";
		if (argc > 0)
			prog_name = argv[0];
		fprintf(stderr, "Usage: %s INPUT_FILE\n", prog_name);
		*input_file = NULL;
		return AOC_EXIT_USAGE;
	}
	*input_file = fopen(argv[1], "r");
	if (!(*input_file)) {
		perror("Could not open file");
		return AOC_EXIT_NO_INPUT;
	}
	return EXIT_SUCCESS;
}

int aoc_main(int argc, char ** argv, char * (*solve1)(FILE *), char * (*solve2)(FILE *)) {
	FILE * file;
	int exit_code = parse_args_and_open(argc, argv, &file);
	if (exit_code != EXIT_SUCCESS)
		return exit_code;

	bool fail = false;
	char* result = solve1(file);
	if (result) {
		fprintf(stderr, "Part 1 success!\n");
		puts(result);
	} else if (ferror(file)) {
		perror("File error");
		return AOC_EXIT_IO_ERROR;
	} else {
		fprintf(stderr, "Part 1 fail!\n");
		fail = true;
	}

	fseek(file, 0, SEEK_SET);
	if (ferror(file)) {
		perror("Could not seek?");
		return AOC_EXIT_IO_ERROR;
	}

	if (solve2) {
		result = solve2(file);
		if (result) {
			fprintf(stderr, "Part 2 success!\n");
			puts(result);
		} else if (ferror(file)) {
			perror("File error");
			return AOC_EXIT_IO_ERROR;
		} else {
			fprintf(stderr, "Part 2 fail!\n");
			fail = true;
		}
	}

	fclose(file);
	return fail ? AOC_EXIT_SOFTWARE_FAIL : EXIT_SUCCESS;
}


int aoc_main_parse_lines(int argc, char ** argv, int32_t parts_implemented, aoc_solver_lines_t solve) {
	FILE * file;
	int exit_code = parse_args_and_open(argc, argv, &file);
	if (exit_code != EXIT_SUCCESS)
		return exit_code;

	char * input = aoc_read_file(file);
	fclose(file);
	if (!input) {
		perror("Error while parsing input");
		return AOC_EXIT_IO_ERROR;
	}

	char ** lines_ = NULL;
	size_t lines_buf_size;
	size_t lines_n = aoc_tokenize(input, '\n', &lines_, &lines_buf_size);
	char const * const * lines = (char const * const *)lines_;

	size_t longest_line_size = 0;
	for (size_t i = 0; i < lines_n; ++i) {
		size_t line_size = strlen(lines[i]);
		if (line_size > longest_line_size)
			longest_line_size = line_size;
	}

	bool had_err = false;
	for (int32_t part = 1; part <= parts_implemented; ++part) {
		aoc_err_t err = { 0 };
		int64_t result = solve(lines, lines_n, longest_line_size, part, &err);

		if (print_err(err)) {
			free_err_msg(err);
			fprintf(stderr, " - Part %" PRId32 " failure!\n", part);
			had_err = true;
		} else {
			fprintf(stderr, "Part %" PRId32 " success!\n%" PRId64 "\n", part, result);
		}
	}
	free(input);
	free(lines_);

	return had_err ? AOC_EXIT_SOFTWARE_FAIL : EXIT_SUCCESS;
}


int aoc_main_parse_c32_2d(int argc, char ** argv, int32_t parts_implemented, aoc_solver_c32_2d_t solve) {
	setlocale(LC_ALL, "");
	// char * locale = setlocale(LC_ALL, "C.utf8");
	// if (strcmp(locale, "C.utf8") != 0) {
	// 	return AOC_EXIT_SOFTWARE_FAIL;
	// }

	FILE * file;
	int exit_code = parse_args_and_open(argc, argv, &file);
	if (exit_code != EXIT_SUCCESS)
		return exit_code;

	char * input = aoc_read_file(file);
	fclose(file);
	if (!input) {
		perror("Error while parsing input");
		return AOC_EXIT_IO_ERROR;
	}

	aoc_err_t err = { 0 };

	aoc_c32_2d_t matrix = aoc_c32_2d_parse(input, &err);

	if (print_err(err)) {
		free_err_msg(err);
		free(input);
		return AOC_EXIT_DATA_ERR;
	}

	bool had_err = false;
	for (int32_t part = 1; part <= parts_implemented; ++part) {
		int64_t result = solve(matrix, part, &err);

		if (print_err(err)) {
			free_err_msg(err);
			fprintf(stderr, " - Part %" PRId32 " failure!\n", part);
			had_err = true;
		} else {
			fprintf(stderr, "Part %" PRId32 " success!\n%" PRId64 "\n", part, result);
		}
	}
	free(input);
	free(matrix.ws);

	return had_err ? AOC_EXIT_SOFTWARE_FAIL : EXIT_SUCCESS;
}


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

