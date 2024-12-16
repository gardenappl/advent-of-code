#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"


#define INVALID_FILE "invalid file"
#define MAX_PRESSES 100
#define A_TOKENS 3
#define B_TOKENS 1

typedef struct {
	int32_t a_x, a_y, b_x, b_y, prize_x, prize_y;
} claw_machine_t;

static int64_t calc_min_tokens(claw_machine_t claw) {
	int64_t min_tokens = INT64_MAX;

	int32_t max_a_presses = claw.prize_x / claw.a_x;
	max_a_presses = aoc_min_int32_t(max_a_presses, claw.prize_y / claw.a_y);
	max_a_presses = aoc_min_int32_t(max_a_presses, MAX_PRESSES);

	int32_t max_b_presses = claw.prize_x / claw.b_x;
	max_b_presses = aoc_min_int32_t(max_b_presses, claw.prize_y / claw.b_y);
	max_b_presses = aoc_min_int32_t(max_b_presses, MAX_PRESSES);

	for (int32_t a = 0; a <= max_a_presses; ++a) {
		for (int32_t b = 0; b <= max_b_presses; ++b) {
			int32_t x = a * claw.a_x + b * claw.b_x;
			if (x > claw.prize_x)
				break;

			int32_t y = a * claw.a_y + b * claw.b_y;
			if (y > claw.prize_y)
				break;

			int32_t tokens = a * A_TOKENS + b * B_TOKENS;
			if (tokens >= min_tokens)
				break;

			if (x == claw.prize_x && y == claw.prize_y) {
				min_tokens = tokens;
				break;
			}
		}
	}

	return min_tokens;
}


static int64_t solve(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err) {
	int64_t min_token_sum = 0;

	size_t line_num = 0;
	while (line_num + 2 < lines_n) {
		claw_machine_t claw;
		int res = sscanf(lines[line_num], "Button A: X+%" PRId32 ", Y+%" PRId32, &claw.a_x, &claw.a_y);
		if (res != 2) {
			aoc_err(err, INVALID_FILE " a");
			return -1;
		}

		++line_num;
		res = sscanf(lines[line_num], "Button B: X+%" PRId32 ", Y+%" PRId32, &claw.b_x, &claw.b_y);
		if (res != 2) {
			aoc_err(err, INVALID_FILE " b");
			return -1;
		}

		++line_num;
		res = sscanf(lines[line_num], "Prize: X=%" PRId32 ", Y=%" PRId32, &claw.prize_x, &claw.prize_y);
		if (res != 2) {
			aoc_err(err, INVALID_FILE " p");
			return -1;
		}

		++line_num;

		fprintf(stderr, "A = +(%" PRId32 ", %" PRId32 "), "
				"B = +(%" PRId32 ", %" PRId32 "), "
				"prize = (%" PRId32 ", %" PRId32 ")\n",
				claw.a_x, claw.a_y, claw.b_x, claw.b_y, claw.prize_x, claw.prize_y);
		int64_t min_tokens = calc_min_tokens(claw);
		if (min_tokens == INT64_MAX) {
			fprintf(stderr, "Min. tokens: (unreachable)\n");
		} else {
			fprintf(stderr, "Min. tokens: %" PRId64 "\n", min_tokens);
			min_token_sum += min_tokens;
		}
	}

	return min_token_sum;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 1, solve);
}
