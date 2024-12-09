#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "aoc.h"

static bool try_mul_args(char * left_paren_s, int64_t * result) {
	char * next;
	left_paren_s++;

	if (!(isdigit(*left_paren_s) || *left_paren_s == '-'))
		return EXIT_FAILURE;
	long x = strtol(left_paren_s, &next, 10);
	left_paren_s = next;

	if (*left_paren_s != ',')
		return EXIT_FAILURE;
	left_paren_s++;

	if (!(isdigit(*left_paren_s) || *left_paren_s == '-'))
		return EXIT_FAILURE;
	long y = strtol(left_paren_s, &next, 10);
	left_paren_s = next;

	if (*left_paren_s != ')')
		return EXIT_FAILURE;

	(*result) += x * y;
	fprintf(stderr, "mul(%ld,%ld)=%" PRId64 "\n", x, y, x * y);
	return EXIT_SUCCESS;
}

char * solve1(FILE * input) {
	int64_t result = 0;

	char * s = aoc_read_file(input);
	char * current = strstr(s, "mul(");
	if (!current)
		return NULL;
	do {
		current += 3;
		bool err_ = try_mul_args(current, &result);
		if (err_)
			continue;
		// fprintf(stdout, "mul(%ld,%ld)\n", x, y);
	} while ((current = strstr(current, "mul(")));

	char * buf = malloc(AOC_MSG_SIZE);
	snprintf(buf, AOC_MSG_SIZE, "%" PRId64, result);
	return buf;
}

char * solve2(FILE * input) {
	int64_t result = 0;

	char * s = aoc_read_file(input);
	char * current = strchr(s, '(');
	if (!current)
		return NULL;
	bool doing = true;
	do {
		if (doing) {
			// fprintf(stderr, "String:      %.5s\n", current);
			if ((current - s) >= 5) {
				// fprintf(stderr, "String: %.10s\n", current - 5);
				if (!strncmp((current - 5), "don't(", 5)) {
					doing = false;
					// fprintf(stderr, "don'ting\n");
					continue;
				}
			}
			if ((current - s) >= 3) {
				if (!strncmp((current - 3), "mul(", 3))
					try_mul_args(current, &result);
			}
		} else {
			if ((current - s) >= 2) {
				if (!strncmp((current - 2), "do(", 2)) {
					doing = true;
					// fprintf(stderr, "doing\n");
				}
			}
		}
	} while ((current = strchr(current + 1, '(')));

	char * buf = malloc(AOC_MSG_SIZE);
	snprintf(buf, AOC_MSG_SIZE, "%" PRId64, result);
	return buf;
}

int main(int argc, char * argv[]) {
	aoc_main(argc, argv, solve1, solve2);
}
