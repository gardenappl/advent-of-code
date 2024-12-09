#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

#include "aoc.h"

typedef enum { MUL = 1, DO = 2, DONT = 3, END = 0 } inst_t;
typedef struct {
	bool init;
	char * next_mul;
	char * next_do; 
	char * next_dont; 
	inst_t prev_inst;	
} parser_state;

inst_t next_inst(char ** s, char ** next, parser_state state) {
	if (!state.init) {
		state.next_mul = strstr(*s, "mul(");
		state.next_do = strstr(*s, "do(");
		state.next_dont = strstr(*s, "don't(");
		state.init = true;
	} else {
		switch (state.prev_inst) {
			case MUL:
				state.next_mul = strstr(*s, "mul(");
				break;
			case DO:
				state.next_do = strstr(*s, "do(");
				break;
			case DONT:
				state.next_dont = strstr(*s, "don't(");
				break;
			case END:
				return END;
		}
	}
	if (state.next_mul < state.next_do) {
		if (state.next_dont < state.next_mul) {
			*next = state.next_dont + 6;
			state.prev_inst = DONT;
			return DONT;
		} else {
			*next = state.next_mul + 4;
			state.prev_inst = MUL;
			return MUL;
		}
	} else {
		if (state.next_dont < state.next_do) {
			*next = state.next_dont + 6;
			state.prev_inst = DONT;
			return DONT;
		} else {
			*next = state.next_do + 3;
			state.prev_inst = DO;
			return DO;
		}
	}
}


char * solve1(FILE * input) {
	int64_t result = 0;

	char * s = aoc_read_file(input);
	char * next;
	parser_state state;
	inst_t inst;
	while ((inst = next_inst(&s, &next, state))) {
		s = next;
		if (inst != MUL) {
			continue;
		}

		if (!(isdigit(*s) || *s == '-'))
			continue;
		long x = strtol(s, &next, 10);
		s = next;

		if (*s != ',')
			continue;
		s++;

		if (!(isdigit(*s) || *s == '-'))
			continue;
		long y = strtol(s, &next, 10);
		s = next;

		if (*s != ')')
			continue;

		result += x * y;
		fprintf(stderr, "mul(%ld,%ld)=%" PRId64 "\n", x, y, x * y);
		// fprintf(stdout, "mul(%ld,%ld)\n", x, y);
	}

	char * buf = malloc(AOC_MSG_SIZE);
	snprintf(buf, AOC_MSG_SIZE, "%" PRId64, result);
	return buf;
}

int main(int argc, char * argv[]) {
	aoc_main(argc, argv, solve1, NULL);
}
