#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#include "aoc.h"


static bool mark_edge_walk(aoc_c32_2d_t matrix, char32_t plant_type, int32_t x, int32_t y, 
		size_t dir_walk, size_t dir_look, aoc_bit_array_t checked_edge) {
	int32_t x_look = x + aoc_dir4_x_diffs[dir_look];
	int32_t y_look = y + aoc_dir4_y_diffs[dir_look];
	int32_t x_walk = x;
	int32_t y_walk = y;

	bool marked_edge = false;
	while (
		aoc_check_bounds(matrix, x_walk, y_walk) && 
		aoc_c32_2d_get(matrix, x_walk, y_walk) == plant_type &&
		(!aoc_check_bounds(matrix, x_look, y_look) || aoc_c32_2d_get(matrix, x_look, y_look) != plant_type)
	) {
		// fprintf(stderr, "Marking edge from x: %" PRId32 " y: %" PRId32 " looking at x: %" PRId32 " y: %" PRId32 "\n",
		// 		x_walk, y_walk, x_look, y_look);
		marked_edge = true;
		aoc_bit_array_set(checked_edge, aoc_index_2d(matrix.width, x_walk, y_walk), true);

		x_look += aoc_dir4_x_diffs[dir_walk];
		y_look += aoc_dir4_y_diffs[dir_walk];

		x_walk += aoc_dir4_x_diffs[dir_walk];
		y_walk += aoc_dir4_y_diffs[dir_walk];
	}
	return marked_edge;
}

static bool mark_new_edge(aoc_c32_2d_t matrix, char32_t plant_type, int32_t x, int32_t y, size_t dir, 
		aoc_bit_array_t checked_edge) {
	size_t clockwise_dir = (dir + 1) % 4;
	bool marked_edge = mark_edge_walk(matrix, plant_type, x, y, clockwise_dir, dir, checked_edge);
	if (!marked_edge)
		return false;

	size_t counter_dir = (dir + 3) % 4;
	x += aoc_dir4_x_diffs[counter_dir];
	y += aoc_dir4_y_diffs[counter_dir];
	mark_edge_walk(matrix, plant_type, x, y, counter_dir, dir, checked_edge);
	return true;
}


static void scan_region(aoc_c32_2d_t plots, int32_t x, int32_t y, char32_t plant_type, 
		int32_t * area, int32_t * perimiter, int32_t * edges,
		aoc_bit_array_t checked_plot, aoc_bit_array_t checked_edges[4]) {
	size_t x_y_i = aoc_index_2d(plots.width, x, y);

	++(*area);
	aoc_bit_array_set(checked_plot, x_y_i, true);
	// fprintf(stderr, "Checking x %" PRId32 " y: %" PRId32 "\n", x, y);

	for (size_t dir = 0; dir < 4; ++dir) {
		if (edges) {
			if (!aoc_bit_array_get(checked_edges[dir], x_y_i)) {
				if (mark_new_edge(plots, plant_type, x, y, dir, checked_edges[dir])) {
					// fprintf(stderr, "New edge!\n");
					// aoc_bit_array_2d_print(checked_edges[dir], plots.width,
					// 		'.', aoc_dir4_chars[dir], stderr);
					++(*edges);
				}
			}
		}

		int32_t x2 = x + aoc_dir4_x_diffs[dir];
		int32_t y2 = y + aoc_dir4_y_diffs[dir];
		size_t x2_y2_i = aoc_index_2d(plots.width, x2, y2);

		if (!aoc_check_bounds(plots, x2, y2)) {
			if (perimiter) {
				++(*perimiter);
			}
		} else {
			char32_t plant_type2 = aoc_c32_2d_get(plots, x2, y2);
			if (perimiter && plant_type != plant_type2) {
				++(*perimiter);
			}
			if (plant_type == plant_type2 && !aoc_bit_array_get(checked_plot, x2_y2_i)) {
				scan_region(plots, x2, y2, plant_type, 
						area, perimiter, edges, 
						checked_plot, checked_edges);
			}
		}
	}
}


static int64_t solve(aoc_c32_2d_t matrix, int32_t part, aoc_err_t * err) {
	aoc_bit_array_t checked_plot = aoc_bit_array_make(matrix.width * matrix.height, err);
	if (aoc_is_err(err))
		return -1;

	aoc_bit_array_t checked_edges[4];
	size_t init_edges = 0;
	for (; init_edges < 4; ++init_edges) {
		checked_edges[init_edges] = aoc_bit_array_make(matrix.width * matrix.height, err);
		if (aoc_is_err(err)) {
			goto cleanup;
		}
	}

	int64_t cost_total = 0;

	for (int32_t x = 0; x < matrix.width; ++x) {
		for (int32_t y = 0; y < matrix.height; ++y) {
			if (aoc_bit_array_get(checked_plot, aoc_index_2d(matrix.width, x, y)))
				continue;

			char32_t plant_type = aoc_c32_2d_get(matrix, x, y);

			int32_t area = 0, perimiter = 0, edges = 0;
			scan_region(matrix, x, y, plant_type, &area, 
					part == 1 ? &perimiter : NULL, 
					part == 2 ? &edges : NULL,
					checked_plot, checked_edges);

			char plant_type_string[MB_CUR_MAX];
			aoc_c32_to_str(plant_type, plant_type_string, err);
			fprintf(stderr, "Plant type: %s, area: %" PRId32 
					", perimiter: %" PRId32
					", edges: %" PRId32 "\n",
					plant_type_string, area, perimiter, edges);
			// if (part == 2) {
			// 	for (size_t dir = 0; dir < 4; ++dir) {
			// 		char c = aoc_dir4_chars[dir];
			// 		aoc_bit_array_2d_print(checked_edges[dir], matrix.width, '.', c, stderr);
			// 	}
			// }

			if (part == 1)
				cost_total += area * perimiter;
			else
				cost_total += area * edges;
		}
	}

cleanup:
	free(checked_plot.data);
	for (; init_edges > 0; --init_edges) {
		free(checked_edges[init_edges - 1].data);
	}

	return cost_total;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_c32_2d(argc, argv, 2, solve);
}
