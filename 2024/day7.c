#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include "aoc.h"

typedef enum { ADD, MUL } op_t;

static bool try_reach_result(int32_t const * nums, size_t nums_n, size_t i, 
		int64_t prev_term, op_t op, int64_t term,
		int64_t required_result, bool try_concat) {
	fprintf(stderr, "i: %zu, (%" PRId64 " %c) %" PRId64 "\n", i, prev_term, op == ADD ? '+' : '*', term);
	int64_t current_result;
	switch (op) {
		case ADD:
			current_result = prev_term + term;
			break;
		case MUL:
			current_result = prev_term * term;
			break;
	}

	if (i + 1 == nums_n)
		return current_result == required_result;
	if (current_result > required_result)
		return false;

	int32_t next_num = nums[i + 1];
	if (try_reach_result(nums, nums_n, i + 1, current_result, ADD, next_num, required_result, try_concat))
		return true;
	if (try_reach_result(nums, nums_n, i + 1, current_result, MUL, next_num, required_result, try_concat))
		return true;

	if (try_concat) {
		fprintf(stderr, "%" PRId64 " || %" PRId32, term, next_num);
		int32_t term_digit_calc = next_num;
		while (term_digit_calc > 0) {
			term_digit_calc /= 10;
			term *= 10;
			if (term > required_result) {
				fprintf(stderr, " = (too much)\n");
				return false;
			}
		}
		term += next_num;
		fprintf(stderr, " = %" PRId64 "\n", term);
		if (try_reach_result(nums, nums_n, i + 1, prev_term, op, term, required_result, true))
			return true;
	}
	return false;
}


static int64_t solve(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err) {
	size_t nums_buf_size = aoc_numbers_line_estimate_size(longest_line_size);
	int32_t * nums = calloc(nums_buf_size, sizeof(int32_t));
	if (!nums) {
		aoc_err(err, "Could not allocate nums");
		return -1;
	}

	size_t found_results_sum = 0;
	for (size_t line_n = 0; line_n < lines_n; ++line_n) {
		char const * line = lines[line_n];

		char * str;
		int64_t result = strtoll(line, &str, 10);
		if (*str != ':' || *(str + 1) != ' ') {
			aoc_err(err, "Invalid line");
			return -1;
		}
		str += 2;
		fprintf(stderr, "Result: %" PRId64 ", rest of string: %s\n", result, str);

		size_t nums_n = aoc_numbers_line_parse(str, ' ', nums, nums_buf_size);
		if (nums_n == 0) {
			aoc_err(err, "No numbers?");
			return -1;
		}
		// fprintf(stderr, "%zu numbers, first is %" PRId32 "\n", nums_n, nums[0]);
		bool found = try_reach_result(nums, nums_n, 0, 0, ADD, nums[0], result, part == 2);
		if (found) {
			found_results_sum += result;
			fprintf(stderr, "Found for line %s\n", line);
		}
	}
	return found_results_sum;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 2, solve);
}
