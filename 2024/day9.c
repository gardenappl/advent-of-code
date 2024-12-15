#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"


#define EMPTY -1

typedef struct block_list {
	int32_t block_id;
	int32_t count;
	struct block_list * prev_blocks;
	struct block_list * next_blocks;
} block_list_t;

static void remove_after(block_list_t list) {
	block_list_t * next = list.next_blocks;
	while (next) {
		block_list_t * next2 = next->next_blocks;
		free(next);
		next = next2;
	}
}

static void remove_last(block_list_t ** list_last) {
	block_list_t * prev = (*list_last)->prev_blocks;
	if (prev) {
		prev->next_blocks = NULL;
	}
	free(*list_last);
	*list_last = prev;
}

static int64_t solve(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err) {
	if (lines_n != 1) {
		aoc_err(err, "Wrong number of lines!");
		return -1;
	}

	char const * line = *lines;
	if (*line == '\0')
		return 0;

	block_list_t list = { 
		.block_id = 0,
		.count = *line - '0',
		.prev_blocks = NULL,
		.next_blocks = NULL
	};
	block_list_t * list_end = &list;

	++line;

	bool free_space = true;
	int32_t block_id = 1;

	while (*line) {
		int32_t assign_block_id;
		if (free_space) {
			assign_block_id = EMPTY;
			free_space = false;
		} else {
			assign_block_id = block_id;
			++block_id;
			free_space = true;
		}
		block_list_t * next = malloc(sizeof(block_list_t));
		if (!next) {
			aoc_err(err, "no space for list");
			goto cleanup_blocks;
		}
		next->block_id = assign_block_id;
		next->count = *line - '0';
		next->next_blocks = NULL;
		next->prev_blocks = list_end;

		list_end->next_blocks = next;
		list_end = next;

		++line;
	}

	block_list_t * current = &list;
	while (current) {
		fprintf(stderr, "id: %" PRId32 ", count: %" PRId32 "\n", current->block_id, current->count);
		current = current->next_blocks;
	}
	fputs("yeaa\n", stderr);

	block_list_t * defrag_cur = &list;
	while (defrag_cur && defrag_cur != list_end) {
		if (list_end->block_id == EMPTY || list_end->count == 0) {
			remove_last(&list_end);
			continue;
		}

		if (defrag_cur->block_id != EMPTY) {
			defrag_cur = defrag_cur->next_blocks;
			continue;
		}

		// Special case in the end
		if (defrag_cur->next_blocks == list_end) {
			defrag_cur->prev_blocks->next_blocks = list_end;
			free(defrag_cur);
			break;
		}

		int32_t free_count = defrag_cur->count;
		int32_t end_count = list_end->count;
		if (free_count < end_count) {
			defrag_cur->block_id = list_end->block_id;
			list_end->count -= free_count;
			defrag_cur = defrag_cur->next_blocks;
		} else if (free_count == end_count) {
			defrag_cur->block_id = list_end->block_id;
			remove_last(&list_end);
			defrag_cur = defrag_cur->next_blocks;
		} else {
			block_list_t * new_blocks = malloc(sizeof(block_list_t));
			if (!new_blocks) {
				aoc_err(err, "could not alloc new blocks");
				goto cleanup_blocks;
			}
			defrag_cur->count -= end_count;

			new_blocks->block_id = list_end->block_id;
			new_blocks->count = end_count;
			new_blocks->prev_blocks = defrag_cur->prev_blocks;
			new_blocks->next_blocks = defrag_cur;

			defrag_cur->prev_blocks->next_blocks = new_blocks;
			defrag_cur->prev_blocks = new_blocks;
			remove_last(&list_end);
		}
	}

	int64_t checksum = 0;
	int64_t block_pos = 0;

	current = &list;
	while (current) {
		fprintf(stderr, "id: %" PRId32 ", count: %" PRId32 "\n", current->block_id, current->count);

		int32_t id = current->block_id;
		for (int32_t count = current->count; count > 0; --count) {
			checksum += block_pos * id;
			++block_pos;
		}
		current = current->next_blocks;
	}


cleanup_blocks:
	remove_after(list);
	return checksum;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 1, solve);
}
