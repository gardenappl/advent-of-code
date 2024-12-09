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

#define AOC_COMPARE_DECLARE_FOR(type)  int aoc_compare_##type(void const * a, void const * b);

AOC_COMPARE_DECLARE_FOR(int32_t)

typedef struct {
	char const * s;
	size_t width;
	size_t height;
} aoc_s_matrix;

extern size_t aoc_sq8_x_diffs[8];
extern size_t aoc_sq8_y_diffs[8];
extern size_t aoc_cross4_x_diffs[4];
extern size_t aoc_cross4_y_diffs[4];

size_t aoc_count_lines(FILE * file);
void * aoc_read_file(FILE * file);
bool aoc_s_matrix_init(char const * s, aoc_s_matrix * s_matrix);
char aoc_s_matrix_get(aoc_s_matrix s_matrix, size_t x, size_t y);
bool aoc_s_matrix_bounded(aoc_s_matrix s_matrix, size_t x, size_t y);

char * aoc_solve_for_matrix(FILE * input, int64_t (*solve_for_matrix)(aoc_s_matrix));
int aoc_main(int argc, char * argv[], 
		char * (*solve1)(FILE * const), 
		char * (*solve2)(FILE * const));

#endif /* end of include guard: AOC_H */
