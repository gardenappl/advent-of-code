#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>

#include "aoc.h"


static bool try_reach_result(int32_t const * nums, size_t i, size_t nums_n, int64_t required_result, int64_t current_value) {
	if (i == nums_n)
		return current_value == required_result;
	if (current_value > required_result)
		return false;

	int32_t num = nums[i];
	if (try_reach_result(nums, i + 1, nums_n, required_result, current_value + num))
		return true;
	if (try_reach_result(nums, i + 1, nums_n, required_result, current_value * num))
		return true;
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
		fprintf(stderr, "%zu numbers, first is %" PRId32 "\n", nums_n, nums[0]);
		bool found = try_reach_result(nums, 1, nums_n, result, nums[0]);
		if (found) {
			found_results_sum += result;
			fprintf(stderr, "Found for line %s\n", line);
		}
	}
	return found_results_sum;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 1, solve);
}
