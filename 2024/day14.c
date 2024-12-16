#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"


#define INVALID_FILE "invalid file"
#define WIDTH 101
#define HEIGHT 103
#define STEPS 100

static int64_t solve(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err) {
	int64_t robots_in_quad[4] = { 0 };

	for (size_t i = 0; i < lines_n; ++i) {
		int x, y, vel_x, vel_y;
		int res = sscanf(lines[i], "p=%d,%d v=%d,%d", &x, &y, &vel_x, &vel_y);
		if (res != 4) {
			aoc_err(err, INVALID_FILE);
			return -1;
		}
		x = aoc_mod_int32_t(x + vel_x * STEPS, WIDTH);
		y = aoc_mod_int32_t(y + vel_y * STEPS, HEIGHT);
		fprintf(stderr, "p=%d,%d v=%d,%d\n", x, y, vel_x, vel_y);

		if (x < WIDTH / 2 && y < HEIGHT / 2)
			++robots_in_quad[0];
		else if (x > WIDTH / 2 && y < HEIGHT / 2)
			++robots_in_quad[1];
		else if (x < WIDTH / 2 && y > HEIGHT / 2)
			++robots_in_quad[2];
		else if (x > WIDTH / 2 && y > HEIGHT / 2)
			++robots_in_quad[3];
	}

	int64_t safety_score = robots_in_quad[0];
	for (size_t i = 1; i < 4; ++i)
		safety_score *= robots_in_quad[i];
	return safety_score;
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 2, solve);
}
