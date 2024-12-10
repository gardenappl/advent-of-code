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


static bool parse_rules(FILE * input, rule_t ** rules_, rule_t *** rules_for_page_after_, int32_t * max_rule_page) {
	size_t lines_n = aoc_count_lines(input);
	if (lines_n == 0) {
		return EXIT_FAILURE;
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

	*max_rule_page = 0;
	for (size_t i = 0; i < rules_n; i++) {
		rule = rules + i;
		if (rule->page_before > *max_rule_page)
			*max_rule_page = rule->page_before;
		if (rule->page_after > *max_rule_page)
			*max_rule_page = rule->page_after;
	}

	rule_t ** rules_for_page_after = calloc(*max_rule_page + 1, sizeof(rule_t *));
	for (size_t i = 0; i < lines_n; i++) {
		rule = rules + i;
		if (rules_for_page_after[rule->page_after] != NULL) {
			rule->next_rule_for_page_after = rules_for_page_after[rule->page_after];
		}
		rules_for_page_after[rule->page_after] = rule;
	}
	
	fseek(input, successful_rule_scan, SEEK_SET);

	*rules_ = rules;
	*rules_for_page_after_ = rules_for_page_after;
	return EXIT_SUCCESS;
}

static char * solve1(FILE * input) {
	rule_t * rules;
	rule_t ** rules_for_page_after;
	int32_t max_banned_page;
	if (parse_rules(input, &rules, &rules_for_page_after, &max_banned_page))
		return NULL;

	aoc_numbers_line num_line = { 0 };
	int64_t middle_page_sum = 0;
	bool * page_banned = calloc(max_banned_page + 1, sizeof(bool));

	char * line = NULL;
	size_t line_buf_size;
	size_t line_length;
	while ((line_length = aoc_get_until_newline(&line, &line_buf_size, input))) {
		aoc_numbers_line_parse_new(line, &num_line, ',');

		memset(page_banned, 0, sizeof(bool) * (max_banned_page + 1));
		bool none_banned = true;

		for (size_t i = 0; i < num_line.numbers_count; i++) {
			int32_t page = num_line.numbers[i];
			fprintf(stderr, "%" PRId32 ",", page);
			if (page > max_banned_page)
				continue;

			if (page_banned[page]) {
				fprintf(stderr, "...Banned!!\n");
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

static char * solve2(FILE * input) {
	rule_t * rules;
	rule_t ** rules_for_page_after;
	int32_t max_banned_page;
	if (parse_rules(input, &rules, &rules_for_page_after, &max_banned_page))
		return NULL;
	fprintf(stderr, "Max: %" PRId32 "\n", max_banned_page);

	aoc_numbers_line num_line = { 0 };
	int64_t middle_page_sum = 0;
	bool * page_banned = calloc(max_banned_page + 1, sizeof(bool));
	size_t * page_banned_at = calloc(max_banned_page + 1, sizeof(size_t));

	char * line = NULL;
	size_t line_buf_size;
	size_t line_length;
	while ((line_length = aoc_get_until_newline(&line, &line_buf_size, input))) {
		aoc_numbers_line_parse_new(line, &num_line, ',');
		fprintf(stderr, "Yea: ");
		for (size_t i = 0; i < num_line.numbers_count; i++) {
			int32_t page = num_line.numbers[i];
			fprintf(stderr, "%" PRId32 ",", page);
		}
		fprintf(stderr, "\n");

		memset(page_banned, 0, sizeof(bool) * (max_banned_page + 1));
		memset(page_banned_at, 0, sizeof(size_t) * (max_banned_page + 1));
		bool found_banned = false;

		for (size_t i = 0; i < num_line.numbers_count; i++) {
			int32_t page = num_line.numbers[i];
			fprintf(stderr, "%" PRId32 ",", page);
			if (page > max_banned_page)
				continue;

			if (page_banned[page]) {
				size_t page_ban_i = page_banned_at[page];
				fprintf(stderr, "...Banned because of %" PRId32 " at pos %zu!!\n", num_line.numbers[page_ban_i], page_ban_i);
				found_banned = true;

				aoc_move_value_int32_t(num_line.numbers, i, page_ban_i);
				fprintf(stderr, "Now: ");
				for (size_t i = 0; i < num_line.numbers_count; i++) {
					int32_t page = num_line.numbers[i];
					fprintf(stderr, "%" PRId32 ",", page);
				}
				fprintf(stderr, "\n");
				memset(page_banned, 0, sizeof(bool) * (max_banned_page + 1));
				memset(page_banned_at, 0, sizeof(size_t) * (max_banned_page + 1));
				i = -1;
				continue;
			}

			rule_t const * rule = rules_for_page_after[page];
			while (rule) {
				int32_t page_before = rule->page_before;
				if (!page_banned[page_before]) {
					// fprintf(stderr, "(banning %" PRId32 ")", page_before);
					page_banned[page_before] = true;
					page_banned_at[page_before] = i;
				}
				rule = rule->next_rule_for_page_after;
			}
		}
		if (found_banned) {
			int32_t middle_page = num_line.numbers[num_line.numbers_count / 2];
			middle_page_sum += middle_page;
		}
		fprintf(stderr, "\n");
	}

	free(rules);
	free(rules_for_page_after);
	free(line);
	aoc_numbers_line_free(num_line);
	free(page_banned);
	free(page_banned_at);
	
	
	char * buf = malloc(AOC_MSG_SIZE);
	snprintf(buf, AOC_MSG_SIZE, "%" PRId64, middle_page_sum);
	return buf;
}

int main(int argc, char * argv[]) {
	aoc_main(argc, argv, solve1, solve2);
}
