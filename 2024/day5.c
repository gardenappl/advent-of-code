#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "aoc.h"

typedef struct rule {
	size_t num;
	int32_t page_before;
	int32_t page_after;
	struct rule const * next_rule_for_page_after;
} rule_t;

static char * solve1(FILE * input) {
	size_t lines_n = aoc_count_lines(input);
	if (lines_n == 0) {
		return NULL;
	}
	rule_t * rules = calloc(lines_n, sizeof(rule_t));

	int scan;
	rule_t * rule = rules;
	size_t rules_n = 0;
	long successful_rule_scan;
	while ((scan = fscanf(input, "%" PRId32 "|%" PRId32 "\n", &(rule->page_before), &(rule->page_after))) == 2) {
		fprintf(stderr, "Rule: %" PRId32 " -> %" PRId32 "\n", rule->page_before, rule->page_after);
		rule->num = rules_n;
		rules_n++;

		rule++;

		successful_rule_scan = ftell(input);
	}

	int32_t max_banned_page = 0;
	for (size_t i = 0; i < rules_n; i++) {
		rule = rules + i;
		if (rule->page_before > max_banned_page)
			max_banned_page = rule->page_before;
		if (rule->page_after > max_banned_page)
			max_banned_page = rule->page_after;
	}

	rule_t ** rules_for_page_after = calloc(max_banned_page + 1, sizeof(rule_t *));
	for (size_t i = 0; i < lines_n; i++) {
		rule = rules + i;
		if (rules_for_page_after[rule->page_after] != NULL) {
			rule->next_rule_for_page_after = rules_for_page_after[rule->page_after];
		}
		rules_for_page_after[rule->page_after] = rule;
	}
	
	fseek(input, successful_rule_scan, SEEK_SET);

	aoc_numbers_line num_line = { 0 };
	int64_t middle_page_sum = 0;
	bool * page_banned = calloc(max_banned_page + 1, sizeof(bool));

	char * line = NULL;
	size_t line_buf_size;
	size_t line_length;
	while ((line_length = aoc_get_until_newline(&line, &line_buf_size, input))) {
		aoc_numbers_line_parse_new(line, &num_line, ',');

		// fprintf(stderr, "Numbers: %zu, token buffer size: %zu\n", num_line.numbers_count, num_line.tokens_buf_size);

		memset(page_banned, 0, sizeof(bool) * (max_banned_page + 1));
		bool none_banned = true;

		for (size_t i = 0; i < num_line.numbers_count; i++) {
			int32_t page = num_line.numbers[i];
			fprintf(stderr, "%" PRId32 ",", page);
			if (page > max_banned_page)
				continue;

			if (page_banned[page]) {
				// fprintf(stderr, "Banned!!\n");
				none_banned = false;
				break;
			}

			rule_t const * rule = rules_for_page_after[page];
			while (rule) {
				int32_t page_before = rule->page_before;
				page_banned[page_before] = true;
				rule = rule->next_rule_for_page_after;
			}
		}
		fprintf(stderr, "\n");
		if (none_banned) {
			int32_t middle_page = num_line.numbers[num_line.numbers_count / 2];
			middle_page_sum += middle_page;
		}
	}

	free(rules);
	free(rules_for_page_after);
	free(line);
	aoc_numbers_line_free(num_line);
	free(page_banned);
	
	
	char * buf = malloc(AOC_MSG_SIZE);
	snprintf(buf, AOC_MSG_SIZE, "%" PRId64, middle_page_sum);
	return buf;
}

int main(int argc, char * argv[]) {
	aoc_main(argc, argv, solve1, NULL);
}
