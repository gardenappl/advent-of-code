#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "aoc.h"

char * solve1(FILE * input) {
	int64_t result = 0;

	char * s = aoc_read_file(input);
	char * current = strstr(s, "mul(");
	if (!current)
		return NULL;
	do {
		char * next;
		current += 4;

		if (!(isdigit(*current) || *current == '-'))
			continue;
		long x = strtol(current, &next, 10);
		current = next;

		if (*current != ',')
			continue;
		current++;

		if (!(isdigit(*current) || *current == '-'))
			continue;
		long y = strtol(current, &next, 10);
		current = next;

		if (*current != ')')
			continue;

		result += x * y;
		fprintf(stderr, "mul(%ld,%ld)=%" PRId64 "\n", x, y, x * y);
		// fprintf(stdout, "mul(%ld,%ld)\n", x, y);
	} while ((current = strstr(current, "mul(")));

	char * buf = malloc(AOC_MSG_SIZE);
	snprintf(buf, AOC_MSG_SIZE, "%" PRId64, result);
	return buf;
}

int main(int argc, char * argv[]) {
	aoc_main(argc, argv, solve1, NULL);
}
