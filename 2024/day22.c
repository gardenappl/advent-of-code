#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>
#include <threads.h>

#include "aoc.h"


#define CPU_COUNT 4


static inline uint32_t next(uint32_t n) {
	n = ((n << 6) ^ n) & 0x00FFFFFF;
	n = ((n >> 5) ^ n) & 0x00FFFFFF;
	n = ((n << 11) ^ n) & 0x00FFFFFF;
	return n;
}

static inline void set_contains_2_deltas(bool * contains_2_deltas, int d1, int d2) {
	contains_2_deltas[aoc_index_2d(19, d1 + 9, d2 + 9)] = true;
}

static inline bool get_contains_2_deltas(bool const * contains_2_deltas, int d1, int d2) {
	return contains_2_deltas[aoc_index_2d(19, d1 + 9, d2 + 9)];
}

static bool check_contains_2_deltas(uint32_t n, bool * contains_2_deltas, int d1, int d2) {
	int deltas[2];
	for (int step = 0; step < 2; ++step) {
		uint32_t n2 = next(n);
		deltas[step] = (int)(n2 % 10) - (int)(n % 10);
		n = n2;
	}
	if (deltas[0] == d1 && deltas[1] == d2)
		return true;

	for (int step = 0; step < 2000 - 2; ++step) {
		deltas[0] = deltas[1];
		int32_t n2 = next(n);
		deltas[1] = (int)(n2 % 10) - (int)(n % 10);
		n = n2;

		if (deltas[0] == d1 && deltas[1] == d2)
			return true;
	}
	return false;
}

static int get_price_after_deltas(uint32_t n, int deltas_expect[4]) {
	int deltas[4];
	for (int step = 0; step < 4; ++step) {
		uint32_t n2 = next(n);
		deltas[step] = (int)(n2 % 10) - (int)(n % 10);
		n = n2;
	}
	if (deltas[0] == deltas_expect[0]
			&& deltas[1] == deltas_expect[1]
			&& deltas[2] == deltas_expect[2]
			&& deltas[3] == deltas_expect[3])
		return n % 10;

	for (int step = 0; step < 2000 - 4; ++step) {
		for (int i = 1; i < 4; ++i)
			deltas[i - 1] = deltas[i];
		int32_t n2 = next(n);
		deltas[3] = (int)(n2 % 10) - (int)(n % 10);
		n = n2;

		if (deltas[0] == deltas_expect[0]
				&& deltas[1] == deltas_expect[1]
				&& deltas[2] == deltas_expect[2]
				&& deltas[3] == deltas_expect[3])
			return n % 10;
	}
	return 0;
}

static int64_t solve1(char const * const * lines, size_t lines_n, aoc_ex_t * e) {
	int64_t sum = 0;
	for (size_t i = 0; i < lines_n; ++i) {
		uint32_t n;
		if (sscanf(lines[i], "%"PRIu32, &n) != 1) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			return -1;
		}
		for (int i = 0; i < 2000; ++i) {
			n = next(n);
		}
		// fprintf(stderr, "%"PRIu32"\n", n);
		sum += n;
	}
	return sum;
}

typedef struct {
	uint32_t const * seeds;
	size_t seeds_n;
	bool const * const * seed_contains_2_deltas;
	int deltas_expect_max[4];
	int32_t max;
	int mod;
	int id;
} solve2_env_t;

static inline void solve2_deltas(solve2_env_t * env, int deltas_expect[4]) {
	int32_t sum = 0;
	for (size_t i = 0; i < env->seeds_n; ++i) {
		for (size_t d = 0; d < 3; ++d) {
			if (!get_contains_2_deltas(env->seed_contains_2_deltas[i], 
						deltas_expect[d],
						deltas_expect[d + 1]))
				return;
		}
		sum += get_price_after_deltas(env->seeds[i], deltas_expect);
	}
	if (sum > env->max) {
		env->max = sum;
		env->deltas_expect_max[0] = deltas_expect[0];
		env->deltas_expect_max[1] = deltas_expect[1];
		env->deltas_expect_max[2] = deltas_expect[2];
		env->deltas_expect_max[3] = deltas_expect[3];
		fprintf(stderr, "(thread %d) new max: %"PRId32" with seq: %d %d %d %d\n", 
				env->id,
				env->max, 
				env->deltas_expect_max[0],
				env->deltas_expect_max[1],
				env->deltas_expect_max[2],
				env->deltas_expect_max[3]);
	}
}

static int solve2_(void * arg) {
	solve2_env_t * env = (solve2_env_t *)arg;

	for (int d1 = -9; d1 <= 9; ++d1) {
		if (aoc_mod_int(d1, env->mod) != env->id)
			continue;
		for (int d2 = -9; d2 <= 9; ++d2) {
			for (int d3 = -9; d3 <= 9; ++d3) {
				for (int d4 = -9; d4 <= 9; ++d4) {
					int deltas_expect[4] = { d1, d2, d3, d4 };
					solve2_deltas(env, deltas_expect);
				}
			}
		}
	}
	return EXIT_SUCCESS;
}

static int64_t solve2(char const * const * lines, size_t lines_n, aoc_ex_t * e) {
	int32_t max = 0;

	size_t seeds_n = 0;
	uint32_t * seeds = assert_malloc(lines_n, uint32_t);

	bool ** seed_contains_2_deltas = assert_malloc(lines_n, bool *);
	for (size_t i = 0; i < lines_n; ++i) {
		uint32_t n;
		if (sscanf(lines[i], "%"PRIu32, &n) != 1) {
			aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
			goto cleanup;
		}
		// fprintf(stderr, "%"PRIu32"\n", n);
		seeds[i] = n;
		seed_contains_2_deltas[i] = assert_calloc(19 * 19, bool);
		for (int d1 = -9; d1 <= 9; ++d1) {
			for (int d2 = -9; d2 <= 9; ++d2) {
				if (check_contains_2_deltas(n, seed_contains_2_deltas[i], d1, d2))
					set_contains_2_deltas(seed_contains_2_deltas[i], d1, d2);
			}
		}
		++seeds_n;
	}

	solve2_env_t envs[CPU_COUNT];
	thrd_t threads[CPU_COUNT];

	for (int i = 0; i < CPU_COUNT; ++i) {
		envs[i] = (solve2_env_t){
			.seeds = seeds,
			.seeds_n = seeds_n,
			.seed_contains_2_deltas = (bool const * const *)seed_contains_2_deltas,
			.deltas_expect_max = { 0 },
			.max = 0,
			.id = i,
			.mod = CPU_COUNT
		};
		if (thrd_create(&threads[i], solve2_, &envs[i]) != thrd_success) {
			aoc_throw_fail(e, "thread error");
			// no idea what to do here
			goto cleanup;
		}
	}

	solve2_env_t * max_env = &envs[0];
	for (int i = 0; i < CPU_COUNT; ++i) {
		if (thrd_join(threads[i], NULL) != thrd_success) {
			aoc_throw_fail(e, "thread error");
		}
		if (envs[i].max > max) {
			max_env = &envs[i];
			max = envs[i].max;
		}
	}
	fprintf(stderr, "max: %"PRId32" with seq: %d %d %d %d\n", 
			max, 
			max_env->deltas_expect_max[0],
			max_env->deltas_expect_max[1],
			max_env->deltas_expect_max[2],
			max_env->deltas_expect_max[3]);

cleanup:
	for (size_t i = 0; i < seeds_n; ++i)
		free(seed_contains_2_deltas[i]);
	free(seed_contains_2_deltas);
	free(seeds);
	return max;
}

static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	if (part == 1)
		return solve1(lines, lines_n, e);
	else
		return solve2(lines, lines_n, e);
}

int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 2, solve);
}
