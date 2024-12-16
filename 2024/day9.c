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

static void print_list(block_list_t * list) {
	if (list->prev_blocks)
		fprintf(stderr, "List: (BAD PREV)\n");
	else
		fprintf(stderr, "List:\n");
	bool first = true;
	while (list) {
		fprintf(stderr, "   * id: %" PRId32 ", count: %" PRId32, list->block_id, list->count);
		if (!first && (!list->prev_blocks || list->prev_blocks->next_blocks != list))
			fputs(" (BAD PREV)\n", stderr);
		else
			fputc('\n', stderr);
		list = list->next_blocks;
		first = false;
	}

}

static void compactify_blocks(block_list_t * list, block_list_t ** list_end) {
	block_list_t * free_space = list;

	while (free_space && free_space != *list_end) {
		if ((*list_end)->block_id == EMPTY || (*list_end)->count == 0) {
			remove_last(list_end);
			continue;
		}

		if (free_space->block_id != EMPTY) {
			free_space = free_space->next_blocks;
			continue;
		}

		// Special case in the end
		if (free_space->next_blocks == (*list_end)) {
			free_space->prev_blocks->next_blocks = (*list_end);
			free(free_space);
			break;
		}

		int32_t free_count = free_space->count;
		int32_t end_count = (*list_end)->count;
		if (free_count < end_count) {
			free_space->block_id = (*list_end)->block_id;
			(*list_end)->count -= free_count;
			free_space = free_space->next_blocks;
		} else if (free_count == end_count) {
			free_space->block_id = (*list_end)->block_id;
			remove_last(list_end);
			free_space = free_space->next_blocks;
		} else {
			free_space->count -= end_count;

			block_list_t * file_blocks = *list_end;

			file_blocks->prev_blocks->next_blocks = NULL;
			*list_end = file_blocks->prev_blocks;

			file_blocks->prev_blocks = free_space->prev_blocks;
			file_blocks->next_blocks = free_space;

			free_space->prev_blocks->next_blocks = file_blocks;
			free_space->prev_blocks = file_blocks;
		}
	}
	// print_list(list);
}

static block_list_t * make_free_space_and_compactify(block_list_t * file_blocks, block_list_t ** list_end) {
	block_list_t * reuse_block = NULL;

	if (file_blocks == *list_end) {
		block_list_t * prev_blocks = file_blocks->prev_blocks;
		if (prev_blocks)
			prev_blocks->next_blocks = NULL;
		*list_end = prev_blocks;
		reuse_block = file_blocks;
	} else {
		file_blocks->block_id = EMPTY;

		block_list_t * next_blocks = file_blocks->next_blocks;
		if (next_blocks->block_id == EMPTY) {
			file_blocks->count += next_blocks->count;
			block_list_t * next_blocks2 = next_blocks->next_blocks;
			if (next_blocks2)
				next_blocks2->prev_blocks = file_blocks;
			file_blocks->next_blocks = next_blocks2;

			reuse_block = next_blocks;

			if (next_blocks == *list_end)
				*list_end = file_blocks;
		}

		block_list_t * prev_blocks = file_blocks->prev_blocks;
		if (prev_blocks->block_id == EMPTY) {
			file_blocks->count += prev_blocks->count;
			block_list_t * prev_blocks2 = prev_blocks->prev_blocks;
			if (prev_blocks2)
				prev_blocks2->next_blocks = file_blocks;
			file_blocks->prev_blocks = prev_blocks2;

			if (reuse_block)
				free(reuse_block);
			reuse_block = prev_blocks;
		}
	}
	return reuse_block;
}

static void compactify_files(block_list_t * list, block_list_t ** list_end, aoc_err_t * err) {
	while ((*list_end)->block_id == EMPTY) {
		remove_last(list_end);
	}
	int32_t id = (*list_end)->block_id;
	while (id > 0) {
		block_list_t * file_blocks = *list_end;
		while (file_blocks->block_id != id) {
			file_blocks = file_blocks->prev_blocks;
			if (!file_blocks)
				goto next_file_id;
		}
		int32_t count = file_blocks->count;

		block_list_t * free_space = list;
		while (free_space != file_blocks) {
			while (free_space->block_id != EMPTY) {
				free_space = free_space->next_blocks;
				if (!free_space || free_space == file_blocks)
					goto next_file_id;
			}

			int32_t free_count = free_space->count;
			if (free_count < count) {
				free_space = free_space->next_blocks;
				continue;
			} else if (free_count == count) {
				free_space->block_id = id;

				block_list_t * reuse_blocks = make_free_space_and_compactify(file_blocks, list_end);
				if (reuse_blocks)
					free(reuse_blocks);
				break;
			} else {
				free_space->count -= count;

				block_list_t * reuse_blocks = make_free_space_and_compactify(file_blocks, list_end);
				if (!reuse_blocks) {
					reuse_blocks = malloc(sizeof(block_list_t));
					if (!reuse_blocks) {
						aoc_err(err, "could not allocate new block");
						return;
					}
				}
				reuse_blocks->block_id = id;
				reuse_blocks->count = count;
				reuse_blocks->next_blocks = free_space;
				reuse_blocks->prev_blocks = free_space->prev_blocks;

				free_space->prev_blocks->next_blocks = reuse_blocks;
				free_space->prev_blocks = reuse_blocks;
				break;
			}
		}
next_file_id:
		// print_list(list);
		// fprintf(stderr, "Moved id %" PRId32 "\n", id);
		--id;
	}
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

		int32_t count = *line - '0';
		if (count == 0) {
			++line;
			continue;
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

	if (part == 1)
		compactify_blocks(&list, &list_end);
	else
		compactify_files(&list, &list_end, err);
	if (aoc_is_err(err)) goto cleanup_blocks;

	int64_t checksum = 0;
	int64_t block_pos = 0;

	current = &list;
	while (current) {
		int32_t id = current->block_id;
		if (id == EMPTY) {
			block_pos += current->count;
		} else {
			for (int32_t count = current->count; count > 0; --count) {
				checksum += block_pos * id;
				++block_pos;
			}
		}
		current = current->next_blocks;
	}


cleanup_blocks:
	remove_after(list);
	return checksum;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 2, solve);
}
