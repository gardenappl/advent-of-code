#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

static int hostname_to_ip(char hostname[3]) {
	static_assert('b' - 'a' == 1, "insane encoding");
	static_assert('c' - 'a' == 2, "insane encoding");
	static_assert('d' - 'a' == 3, "insane encoding");
	static_assert('e' - 'a' == 4, "insane encoding");
	static_assert('f' - 'a' == 5, "insane encoding");
	static_assert('g' - 'a' == 6, "insane encoding");
	static_assert('h' - 'a' == 7, "insane encoding");
	static_assert('i' - 'a' == 8, "insane encoding");
	static_assert('j' - 'a' == 9, "insane encoding");
	static_assert('k' - 'a' == 10, "insane encoding");
	static_assert('l' - 'a' == 11, "insane encoding");
	static_assert('m' - 'a' == 12, "insane encoding");
	static_assert('n' - 'a' == 13, "insane encoding");
	static_assert('o' - 'a' == 14, "insane encoding");
	static_assert('p' - 'a' == 15, "insane encoding");
	static_assert('q' - 'a' == 16, "insane encoding");
	static_assert('r' - 'a' == 17, "insane encoding");
	static_assert('s' - 'a' == 18, "insane encoding");
	static_assert('t' - 'a' == 19, "insane encoding");
	static_assert('u' - 'a' == 20, "insane encoding");
	static_assert('v' - 'a' == 21, "insane encoding");
	static_assert('w' - 'a' == 22, "insane encoding");
	static_assert('x' - 'a' == 23, "insane encoding");
	static_assert('y' - 'a' == 24, "insane encoding");
	static_assert('z' - 'a' == 25, "insane encoding");

	return (hostname[0] - 'a') * 26 + (hostname[1] - 'a');
}

static void ip_to_hostname(int ip, char hostname[3]) {
	hostname[0] = 'a' + (ip / 26);
	hostname[1] = 'a' + (ip % 26);
	hostname[2] = '\0';
}

static void parse_connection(char const * restrict line, char hostname1[3], char hostname2[3], aoc_ex_t * e) {
	if (sscanf(line, "%c%c-%c%c", &hostname1[0], &hostname1[1], &hostname2[0], &hostname2[1]) != 4) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
		return;
	}
	hostname1[2] = 0;
	hostname2[2] = 0;
}

static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	bool ip_exists[26 * 26] = { 0 };
	for (size_t i = 0; i < lines_n; ++i) {
		char hostname1[3];
		char hostname2[3];
		parse_connection(lines[i], hostname1, hostname2, e);
		// fprintf(stderr, "Parsed %s-%s (%d-%d)\n", hostname1, hostname2, hostname_to_ip(hostname1), hostname_to_ip(hostname2));
		if (*e)
			return -1;

		ip_exists[hostname_to_ip(hostname1)] = true;
		ip_exists[hostname_to_ip(hostname2)] = true;
	}
	size_t names_n = 0;
	for (size_t i = 0; i < 26 * 26; ++i) {
		if (ip_exists[i]) {
			char hostname[3];
			ip_to_hostname(i, hostname);
			// fprintf(stderr, "IP %zu (hostname %s) exists\n", i, hostname);
			++names_n;
		}
	}
	int * ips = assert_malloc(names_n, int);
	size_t * ip_to_id = assert_malloc(26 * 26, size_t);
	size_t id_n = 0;
	for (int ip = 0; ip < 26 * 26; ++ip) {
		if (ip_exists[ip]) {
			// char hostname[3];
			// ip_to_hostname(ip, hostname);
			// fprintf(stderr, "IP %d (hostname %s) gets ID %zu\n", ip, hostname, id_n);
			ip_to_id[ip] = id_n;
			ips[id_n] = ip;
			++id_n;
		}
	}

	bool * connect_matrix = assert_calloc(id_n * id_n, bool);

	for (size_t i = 0; i < lines_n; ++i) {
		char hostname1[3];
		char hostname2[3];
		parse_connection(lines[i], hostname1, hostname2, NULL); // error should not happen here

		int id1 = ip_to_id[hostname_to_ip(hostname1)];
		int id2 = ip_to_id[hostname_to_ip(hostname2)];
		connect_matrix[aoc_index_2d(id_n, id1, id2)] = true;
		connect_matrix[aoc_index_2d(id_n, id2, id1)] = true;
	}

	size_t found_t_triples = 0;

	for (size_t y = 0; y < id_n; ++y) {
		int ip1 = ips[y];
		if (ip1 / 26 != 't' - 'a')
			continue;
		for (size_t x = 0; x < id_n; ++x) {
			int ip2 = ips[x];
			if (ip2 / 26 == 't' - 'a' && ip2 < ip1)
				continue;
			if (connect_matrix[aoc_index_2d(id_n, x, y)]) {
				char hostname1[3];
				ip_to_hostname(ip1, hostname1);
				char hostname2[3];
				ip_to_hostname(ip2, hostname2);
				fprintf(stderr, "Connected %s-%s\n", hostname1, hostname2);

				for (size_t id3 = x; id3 < id_n; ++id3) {
					int ip3 = ips[id3];
					if (ip3 / 26 == 't' - 'a' && ip3 < ip1)
						continue;
					if (connect_matrix[aoc_index_2d(id_n, id3, x)]) {
						if (connect_matrix[aoc_index_2d(id_n, id3, y)]) {
							char hostname3[3];
							ip_to_hostname(ip3, hostname3);
							fprintf(stderr, "Connected %s-%s-%s\n", hostname1, hostname2, hostname3);
							++found_t_triples;
						}
					}
				}
			}
		}
	}
	

	free(ips);
	free(ip_to_id);
	return found_t_triples;
}

int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
