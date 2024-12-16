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
	int32_t a_x, a_y;
	int32_t b_x, b_y;
	int64_t prize_x, prize_y;
} claw_machine_t;

/**
 *  The stupid way. I forgot high school algebra when I wrote this.
 */
static int64_t calc_min_tokens_1(claw_machine_t claw) {
	int64_t min_tokens = INT64_MAX;

	int64_t max_a_presses = claw.prize_x / claw.a_x;
	max_a_presses = aoc_min_int64_t(max_a_presses, claw.prize_y / claw.a_y);
	max_a_presses = aoc_min_int64_t(max_a_presses, MAX_PRESSES);

	int64_t max_b_presses = claw.prize_x / claw.b_x;
	max_b_presses = aoc_min_int64_t(max_b_presses, claw.prize_y / claw.b_y);
	max_b_presses = aoc_min_int64_t(max_b_presses, MAX_PRESSES);

	for (int64_t a = 0; a <= max_a_presses; ++a) {
		for (int64_t b = 0; b <= max_b_presses; ++b) {
			int64_t x = a * claw.a_x + b * claw.b_x;
			if (x > claw.prize_x)
				break;

			int64_t y = a * claw.a_y + b * claw.b_y;
			if (y > claw.prize_y)
				break;

			int64_t tokens = a * A_TOKENS + b * B_TOKENS;
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


/**
 * https://en.wikipedia.org/wiki/Extended_Euclidean_algorithm
 */
static int32_t extended_gcd(int32_t x, int32_t y, int32_t * a, int32_t * b) {
	if (y > x) {
		aoc_swap_int32_t(&x, &y);
		int32_t * swp = b;
		b = a;
		a = swp;
	}
	int32_t r = y, prev_r = x;
	int32_t s = 0, prev_s = 1;
	int32_t t = 1, prev_t = 0;


	do {
		int32_t q = prev_r / r;

		int32_t next_r = prev_r % r;
		int32_t next_s = prev_s - q * s;
		int32_t next_t = prev_t - q * t;

		prev_r = r;
		r = next_r;
		prev_s = s;
		s = next_s;
		prev_t = t;
		t = next_t;
	} while (r > 0);

	if (a)
		*a = prev_s;
	if (b)
		*b = prev_t;
	return prev_r;
}

/**
 *  Gaussian elimination on:
 *  a_x * a + b_x * b = prize_x  (* a_y/gcd)   (- (2))
 *  a_y * a + b_y * b = prize_y  (* a_x/gcd)
 */
static int64_t calc_min_tokens_2(claw_machine_t claw) {
	int32_t gcd = extended_gcd(claw.a_x, claw.a_y, NULL, NULL);
	
	int64_t b_x_new = claw.b_x * (claw.a_y / gcd) - claw.b_y * (claw.a_x / gcd);
	int64_t prize_x_new = claw.prize_x * (claw.a_y / gcd) - claw.prize_y * (claw.a_x / gcd);
	// fprintf(stderr, "%" PRId64 "b = %" PRId64 "\n", b_x_new, prize_x_new);
	if (b_x_new == 0)
		return INT64_MAX;

	int64_t b = prize_x_new / b_x_new;
	int64_t b_rem = prize_x_new % b_x_new;
	// fprintf(stderr, "b = %" PRId64 " (rem. %" PRId64 ")\n", b, b_rem);
	if (b_rem != 0 || b < 0)
		return INT64_MAX;

	int64_t a = (claw.prize_x - claw.b_x * b) / claw.a_x;
	int64_t a_rem = (claw.prize_x - claw.b_x * b) % claw.a_x;
	if (a_rem != 0 || a < 0)
		return INT64_MAX;
	return a * A_TOKENS + b * B_TOKENS;
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
		res = sscanf(lines[line_num], "Prize: X=%" PRId64 ", Y=%" PRId64, &claw.prize_x, &claw.prize_y);
		if (res != 2) {
			aoc_err(err, INVALID_FILE " p");
			return -1;
		}

		++line_num;

		if (part == 2) {
			claw.prize_x += 10000000000000;
			claw.prize_y += 10000000000000;
		}

		// fprintf(stderr, "A = +(%" PRId32 ", %" PRId32 "), "
		// 		"B = +(%" PRId32 ", %" PRId32 "), "
		// 		"prize = (%" PRId64 ", %" PRId64 ")\n",
		// 		claw.a_x, claw.a_y, claw.b_x, claw.b_y, claw.prize_x, claw.prize_y);

		// int32_t a, b;
		// int32_t gcd = extended_gcd(claw.a_x, claw.b_x, &a, &b);
		// fprintf(stderr, "%" PRId32 " * %" PRId32 " + "
		// 		"%" PRId32 " * %" PRId32 " = "
		// 		"%" PRId32 "\n",
		// 		a, claw.a_x, b, claw.b_x, gcd);


		int64_t min_tokens;
		if (part == 1)
			min_tokens = calc_min_tokens_1(claw);
		else
			min_tokens = calc_min_tokens_2(claw);

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
	aoc_main_parse_lines(argc, argv, 2, solve);
}
