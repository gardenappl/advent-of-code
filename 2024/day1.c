#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define MSG_SIZE 100

#define AOC_DEFINE_COMPARE(type)  int aoc_compare_##type(const void* a, const void* b) { \
	type arg1 = *(const type*)a; \
	type arg2 = *(const type*)b; \
 \
	if (arg1 < arg2) return -1; \
	if (arg1 > arg2) return 1; \
	return 0; \
}

AOC_DEFINE_COMPARE(int32_t)

size_t aoc_count_lines(FILE* file) {
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

bool get_lists(FILE* input, size_t* _lines, int32_t** restrict _arr1, int32_t** restrict _arr2) {
	size_t lines = aoc_count_lines(input);
	*_lines = lines;
	if (lines < 0) return true;

	int32_t* arr1 = calloc(lines, sizeof(int32_t));
	int32_t* arr2 = calloc(lines, sizeof(int32_t));
	// fprintf(stderr, "%zu", lines);
	// fflush(stderr);
	
	int32_t* current1 = arr1;
	int32_t* current2 = arr2;
	while (fscanf(input, "%5" PRId32 "   %5" PRId32, current1, current2) == 2) {
		// fprintf(stderr, "one: %" PRId32 ", two: %" PRId32 "\n", *current1, *current2);
		// fflush(stderr);
		current1++;
		current2++;
	}
	if (ferror(input)) return true;
	*_arr1 = arr1;
	*_arr2 = arr2;
	return false;
}

char* solve1(FILE* input) {
	size_t lines;
	int32_t* arr1;
	int32_t* arr2;
	bool fail = get_lists(input, &lines, &arr1, &arr2);
	if (fail) return NULL;

	qsort(arr1, lines, sizeof(int32_t), aoc_compare_int32_t);
	qsort(arr2, lines, sizeof(int32_t), aoc_compare_int32_t);

	int32_t diff_sum = 0;
	for (size_t i = 0; i < lines; i++)
		diff_sum += abs(arr1[i] - arr2[i]);

	char* result_msg = malloc(MSG_SIZE);
	snprintf(result_msg, MSG_SIZE, "%" PRId32, diff_sum);
	return result_msg;
}

char* solve2(FILE* input) {
	size_t lines;
	int32_t* arr1;
	int32_t* arr2;
	bool fail = get_lists(input, &lines, &arr1, &arr2);
	if (fail) return NULL;

	int32_t max = 0;
	for (size_t i = 0; i < lines; i++) {
		int32_t num = arr2[i];
		if (num > max)
			max = num;
	}

	int32_t* counts = calloc(max, sizeof(int32_t));

	for (size_t i = 0; i < lines; i++)
		counts[arr2[i]]++;
		
	int64_t similarity = 0;
	for (size_t i = 0; i < lines; i++) {
		int64_t num = arr1[i];
		similarity += num * counts[num];
	}
	
	char* result_msg = malloc(MSG_SIZE);
	snprintf(result_msg, MSG_SIZE, "%" PRId64, similarity);
	return result_msg;
}

int main(int argc, char* argv[]) {
	if (argc < 2) {
		char* prog_name = "PROGRAM_NAME";
		if (argc > 0)
			prog_name = argv[0];
		fprintf(stderr, "Usage: %s INPUT_FILE\n", prog_name);
		return EXIT_FAILURE;
	}
	FILE* file = fopen(argv[1], "r");
	if (!file) {
		perror("Could not open file");
		return EXIT_FAILURE;
	}

	char* result = solve1(file);
	if (result) {
		fprintf(stderr, "Part 1 success!\n");
		puts(result);
	} else if (ferror(file)) {
		perror("File error");
		return EXIT_FAILURE;
	} else {
		fprintf(stderr, "Part 1 fail!\n");
	}

	fseek(file, 0, SEEK_SET);
	if (ferror(file)) {
		perror("Could not seek?");
		return EXIT_FAILURE;
	}

	result = solve2(file);
	if (result) {
		fprintf(stderr, "Part 2 success!\n");
		puts(result);
	} else if (ferror(file)) {
		perror("File error");
		return EXIT_FAILURE;
	} else {
		fprintf(stderr, "Part 2 fail!\n");
	}

	fclose(file);
	return EXIT_SUCCESS;
}
