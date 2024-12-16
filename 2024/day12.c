#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>

#include "aoc.h"


static void calculate_cost(aoc_c32_2d_t matrix, int32_t x, int32_t y, char32_t plant_type, 
		int32_t * area, int32_t * perimiter, aoc_bit_array_t checked_plot) {
	++(*area);
	aoc_bit_array_set(checked_plot, aoc_index_2d(matrix.width, x, y), true);

	for (size_t dir = 0; dir < 4; ++dir) {
		int32_t x2 = x + aoc_dir4_x_diffs[dir];
		int32_t y2 = y + aoc_dir4_y_diffs[dir];

		if (!aoc_check_bounds(matrix, x2, y2)) {
			++(*perimiter);
			continue;
		}
		if (aoc_c32_2d_get(matrix, x2, y2) != plant_type) {
			++(*perimiter);
		} else if (!aoc_bit_array_get(checked_plot, aoc_index_2d(matrix.width, x2, y2))) {
			calculate_cost(matrix, x2, y2, plant_type, area, perimiter, checked_plot);
		}
	}
}


static int64_t solve(aoc_c32_2d_t matrix, int32_t part, aoc_err_t * err) {
	aoc_bit_array_t checked_plot = aoc_bit_array_make(matrix.width * matrix.height, err);
	if (aoc_is_err(err))
		return -1;

	int64_t cost_total = 0;

	for (int32_t x = 0; x < matrix.width; ++x) {
		for (int32_t y = 0; y < matrix.height; ++y) {
			if (aoc_bit_array_get(checked_plot, aoc_index_2d(matrix.width, x, y)))
				continue;
			char32_t plant_type = aoc_c32_2d_get(matrix, x, y);
			int32_t area = 0, perimiter = 0;

			calculate_cost(matrix, x, y, plant_type, &area, &perimiter, checked_plot);

			char plant_type_string[MB_CUR_MAX];
			aoc_c32_to_str(plant_type, plant_type_string, err);
			fprintf(stderr, "Plant type: %s, area: %" PRId32 ", perimiter: %" PRId32 "\n",
					plant_type_string, area, perimiter);

			cost_total += area * perimiter;
		}
	}

	return cost_total;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_c32_2d(argc, argv, 1, solve);
}
