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
#define COMMAND_NEXT 'n'
#define COMMAND_PREV 'p'
#define COMMAND_SKIP 's'


static int64_t solve1(char const * const * lines, size_t lines_n, aoc_err_t * err) {
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




typedef struct {
	int x, y;
	int vel_x, vel_y;
} robot_t;

static bool prompt_ulong(char const * prompt, unsigned long * num) {
	char in[AOC_MSG_SIZE];
	char * res;
	char * parse_end;
	do {
		puts(prompt);
		res = fgets(in, AOC_MSG_SIZE, stdin);
		if (res) {
			*num = strtoul(res, &parse_end, 10);
		}
	} while ((!res || res == parse_end) && !feof(stdin));

	return !feof(stdin);
}

static int64_t solve2(char const * const * lines, size_t robots_n, aoc_err_t * err) {
	unsigned long first_h_line_step, first_v_line_step;
	if (!prompt_ulong("First step with horizontal lines: ", &first_h_line_step))
		return 0;
	if (!prompt_ulong("First step with vertical lines: ", &first_v_line_step))
		return 0;

	// setvbuf(stdin, NULL, _IONBF, 0);

	robot_t * robots = malloc(robots_n * sizeof(robot_t));
	if (!robots) {
		aoc_err(err, "out of memory");
		return -1;
	}

	for (size_t i = 0; i < robots_n; ++i) {
		int res = sscanf(lines[i], "p=%d,%d v=%d,%d", 
				&(robots[i].x), &(robots[i].y), &(robots[i].vel_x), &(robots[i].vel_y));
		if (res != 4) {
			aoc_err(err, INVALID_FILE);
			goto go_robots_go;
			return -1;
		}
	}
	int64_t steps = 0;
	char command = COMMAND_SKIP;
	while (!feof(stdin)) {
		for (size_t i = 0; i < robots_n; ++i) {
			robot_t * robot = &robots[i];
			if (command == COMMAND_NEXT || command == COMMAND_SKIP) {
				robot->x = aoc_mod_int32_t(robot->x + robot->vel_x, WIDTH);
				robot->y = aoc_mod_int32_t(robot->y + robot->vel_y, HEIGHT);
			} else {
				robot->x = aoc_mod_int32_t(robot->x - robot->vel_x, WIDTH);
				robot->y = aoc_mod_int32_t(robot->y - robot->vel_y, HEIGHT);
			}
		}
		if (command == COMMAND_NEXT || command == COMMAND_SKIP)
			++steps;
		else
			--steps;

		if (command == COMMAND_SKIP) {
			if (((steps - first_h_line_step) % HEIGHT != 0) 
					&& ((steps - first_v_line_step) % WIDTH != 0))
				continue;
		}

		for (size_t y = 0; y < HEIGHT; ++y) {
			for (size_t x = 0; x < WIDTH; ++x) {
				size_t robot_count = 0;
				for (size_t i = 0; i < robots_n; ++i) {
					if (robots[i].x == x && robots[i].y == y)
						++robot_count;
				}
				char c;
				if (robot_count == 0)
					c = '.';
				else if (robot_count < 10)
					c = '0' + robot_count;
				else
					c = '+';
				fputc(c, stderr);
			}
			fputc('\n', stderr);
		}
		fprintf(stderr, "Step #%" PRId64 "\n", steps);
		do {
			// Ignore newline after previous command
			if (command == COMMAND_PREV || command == COMMAND_NEXT)
				getc(stdin);
			command = getc(stdin);
		} while (!feof(stdin) && command != '\n' && command != COMMAND_PREV && command != COMMAND_NEXT);
		if (command == '\n')
			command = COMMAND_SKIP;
	}

go_robots_go:
	free(robots);
	return steps;
}

static int64_t solve(char const * const * lines, size_t lines_n, size_t longest_line_size, int32_t part, aoc_err_t * err) {
	if (part == 1)
		return solve1(lines, lines_n, err);
	else
		return solve2(lines, lines_n, err);
}

int main(int argc, char * argv[]) {
	aoc_main_parse_lines(argc, argv, 2, solve);
}
