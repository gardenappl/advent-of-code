#ifdef __STDC_ALLOC_LIB__
#define __STDC_WANT_LIB_EXT2__ 1
#else
#define _POSIX_C_SOURCE 200809L
#endif

// #define DEBUG

#include <string.h>
#include <stdint.h>
#include <inttypes.h>
#include <errno.h>
#include <stdlib.h>
#include <stdbool.h>

#include "aoc.h"

bool check_is_safe(char * report, bool try_tolerate, int32_t prev_num, int32_t prev_diff) {
	#ifdef DEBUG
	fprintf(stderr, "Checking with prev_num %" PRId32 " and prev_diff %" PRId32 ": %s", prev_num, prev_diff, report);
	#endif

	char * bytes = report;
	char * next_bytes; 
	int32_t num; 
	int32_t prev_num2 = 0;
	int32_t prev_diff2 = 0;
	while (true) {
		long res_l = strtol(bytes, &next_bytes, 10);
		num = (int32_t)res_l;
		if (errno == ERANGE || num != res_l) {
			fprintf(stderr, "Number out of bounds: %ld", res_l);
			return NULL;
		}
		if (num == 0)
			break;

		if (prev_num != 0) {
			int32_t diff = num - prev_num;
			if (diff == 0 || abs(diff) > 3 || (prev_diff != 0 && (diff > 0) != (prev_diff > 0))) {
				if (!try_tolerate)
					return false;
				// try ignoring current number
				#ifdef DEBUG
				fprintf(stderr, "Skipping current number: %" PRId32 "\n", num);
				#endif
				bool safe = check_is_safe(next_bytes, false, prev_num, prev_diff);
				if (safe)
					return true;
				// try ignoring previous number
				#ifdef DEBUG
				fprintf(stderr, "Skipping previous number: %" PRId32 "\n", prev_num);
				#endif
				safe = check_is_safe(bytes, false, prev_num2, prev_diff2);
				if (safe)
					return true;
				// try ignoring 1st number
				#ifdef DEBUG
				fprintf(stderr, "Skipping first number\n");
				#endif
				safe = check_is_safe(strchr(report, ' '), false, 0, 0);
				#ifdef DEBUG
				if (safe)
					fputs("Safe after skipping first number!\n", stderr);
				#endif
				return safe;
			}
			prev_diff2 = prev_diff;
			prev_diff = diff;
		}
		prev_num2 = prev_num;
		prev_num = num;
		bytes = next_bytes;
	}
	return true;
}

char * solve(FILE * input, bool try_tolerate) {
	int32_t safe_reports = 0;
	int32_t reports = 0;

	char * line = NULL;
	size_t n;
	size_t line_num = 0;
	while (getline(&line, &n, input) >= 0) {
		if (ferror(input)) return NULL;
		if (feof(input))
			break;

		line_num++;
		#ifdef DEBUG
		fputs(line, stderr);
		#endif
		bool safe = check_is_safe(line, try_tolerate, 0, 0);
		#ifdef DEBUG
		fprintf(stderr, "Line is %s\n", safe ? "safe." : "unsafe!");
		#endif

		if (safe) {
			if (try_tolerate)
				printf("%zu\n", line_num);
			safe_reports++;
		} else {
		}
		reports++;
	}
	free(line);
	fprintf(stderr, "Safe reports: %" PRId32 " out of %" PRId32 "\n", safe_reports, reports);
	char * buf = malloc(AOC_MSG_SIZE);
	snprintf(buf, AOC_MSG_SIZE, "%" PRId32, safe_reports);
	return buf;
}

char * solve1(FILE * input) {
	return solve(input, false);
}

char * solve2(FILE * input) {
	return solve(input, true);
}

int main(int argc, char * argv[]) {
	aoc_main(argc, argv, solve1, solve2);
}
