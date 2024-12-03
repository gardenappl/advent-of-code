#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "aoc.h"

bool get_lists(FILE * input, size_t * lines_, int32_t * restrict * arr1_, int32_t * restrict * arr2_) {
	const size_t lines = aoc_count_lines(input);
	*lines_ = lines;
	if (lines < 0) return EXIT_FAILURE;

	int32_t *const arr1 = calloc(lines, sizeof(int32_t));
	int32_t *const arr2 = calloc(lines, sizeof(int32_t));
	// fprintf(stderr, "%zu", lines);
	// fflush(stderr);
	
	int32_t * current1 = arr1;
	int32_t * current2 = arr2;
	while (fscanf(input, "%5" PRId32 "   %5" PRId32, current1, current2) == 2) {
		// fprintf(stderr, "one: %" PRId32 ", two: %" PRId32 "\n", *current1, *current2);
		// fflush(stderr);
		current1++;
		current2++;
	}
	if (ferror(input)) return EXIT_FAILURE;
	*arr1_ = arr1;
	*arr2_ = arr2;
	return EXIT_SUCCESS;
}

char* solve1(FILE * input) {
	size_t lines;
	int32_t * restrict arr1;
	int32_t * restrict arr2;
	bool fail = get_lists(input, &lines, &arr1, &arr2);
	if (fail) return NULL;

	qsort(arr1, lines, sizeof(int32_t), aoc_compare_int32_t);
	qsort(arr2, lines, sizeof(int32_t), aoc_compare_int32_t);

	int32_t diff_sum = 0;
	for (size_t i = 0; i < lines; i++)
		diff_sum += abs(arr1[i] - arr2[i]);

	char * result_msg = malloc(AOC_MSG_SIZE);
	snprintf(result_msg, AOC_MSG_SIZE, "%" PRId32, diff_sum);
	return result_msg;
}

char* solve2(FILE * input) {
	size_t lines;
	int32_t * arr1;
	int32_t * arr2;
	bool fail = get_lists(input, &lines, &arr1, &arr2);
	if (fail) return NULL;

	int32_t max = 0;
	for (size_t i = 0; i < lines; i++) {
		int32_t num = arr2[i];
		if (num > max)
			max = num;
	}
	if (max > SIZE_MAX) {
		fprintf(stderr, "max value higher than max size_t: %" PRId32, max);
		return NULL;
	}

	int32_t * counts = calloc(max, sizeof(int32_t));

	for (size_t i = 0; i < lines; i++)
		counts[arr2[i]]++;
		
	int64_t similarity = 0;
	for (size_t i = 0; i < lines; i++) {
		int64_t num = arr1[i];
		similarity += num * counts[num];
	}
	
	char * result_msg = malloc(AOC_MSG_SIZE);
	snprintf(result_msg, AOC_MSG_SIZE, "%" PRId64, similarity);
	return result_msg;
}

int main(int argc, char * argv[]) {
	return aoc_main(argc, argv, solve1, solve2);
}
