#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

#define ALPHANUMS 36
#define WIRES_COUNT (ALPHANUMS * ALPHANUMS * ALPHANUMS)
#define END -1

AOC_ASSERT_SANE_ENCODING


typedef enum : char { AND, OR, XOR } gate_op_t;


typedef struct {
	int left_in;
	int right_in;
	gate_op_t op;
	int out;

	int next_unsolved_left;
	int next_unsolved_right;
} gate_t;


typedef struct {
	bool has_value;
	bool exists;

	int next_unsolved_left;
	int next_unsolved_right;
} wire_sort_t;


static void init(wire_sort_t * wire_sort) {
	wire_sort->exists = true;
	wire_sort->next_unsolved_left = END;
	wire_sort->next_unsolved_right = END;
}


static void wire_num_to_name(int num, char name[4]) {
	name[3] = '\0';
	for (size_t i = 0; i < 3; ++i) {
		int digit = num % ALPHANUMS;
		name[2 - i] = (digit >= 10) ? ('a' + (digit - 10)) : ('0' + digit);
		num /= ALPHANUMS;
	}
}


static void fprint(gate_t gate, FILE * file) {
	char left_in[4];
	wire_num_to_name(gate.left_in, left_in);
	char right_in[4];
	wire_num_to_name(gate.right_in, right_in);
	char out[4];
	wire_num_to_name(gate.out, out);
	fprintf(file, "%s %s %s -> %s", 
			left_in, 
			(gate.op == AND) ? "AND": ((gate.op == OR) ? "OR": "XOR"), 
			right_in,
			out);
}

static void parse_gates(char const * const * lines, size_t const * line_lengths, size_t n, 
		gate_t * gates, wire_sort_t * wires, aoc_ex_t * e);

static void sort_gates(gate_t * in_gates, gate_t * out_gates, size_t n, wire_sort_t * wires);

static void eval_gates(gate_t * gates, size_t gates_n, bool * wires);


static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	wire_sort_t * wires_sort = assert_calloc(WIRES_COUNT, wire_sort_t);
	bool * wire_values = assert_calloc(WIRES_COUNT, bool);

	size_t line_i = 0;
	while (line_i < lines_n && line_lengths[line_i] == 6) {
		char * end;
		int wire_num = strtoul(lines[line_i], &end, ALPHANUMS);
		if (wire_num >= WIRES_COUNT) {
			aoc_throw_invalid_file(e);
			goto cleanup_wires;
		}

		init(&(wires_sort[wire_num]));
		wires_sort[wire_num].has_value = true;

		if (lines[line_i][5] == '1')
			wire_values[wire_num] = true;

		++line_i;
	}

	
	++line_i;
	if (line_i >= lines_n) {
		aoc_throw_invalid_file(e);
		goto cleanup;
	}
	size_t gates_n = lines_n - line_i;
	gate_t * gates = assert_malloc(gates_n, gate_t);
	gate_t * gates_sorted = assert_malloc(gates_n, gate_t);

	parse_gates(lines + line_i, line_lengths + line_i, gates_n, gates, wires_sort, e);
	if (*e) {
		goto cleanup;
	}
	sort_gates(gates, gates_sorted, gates_n, wires_sort);

	eval_gates(gates_sorted, gates_n, wire_values);

	int64_t result = 0;
	int z00_wire = (ALPHANUMS - 1) * ALPHANUMS * ALPHANUMS;
	char z_wire_str[4];

	wire_num_to_name(z00_wire, z_wire_str);
	fprintf(stderr, "values, starting from %s:\n", z_wire_str);

	for (int bit = 0; bit < 64; ++bit) {
		int z_wire = z00_wire + (bit / 10) * ALPHANUMS + (bit % 10);

		if (!wires_sort[z_wire].exists)
			break;

		if (wire_values[z_wire]) {
			wire_num_to_name(z_wire, z_wire_str);
			fprintf(stderr, "%s is TRUE\n", z_wire_str);
			result |= (1L << bit);
		} else {
			wire_num_to_name(z_wire, z_wire_str);
			fprintf(stderr, "%s is FALSE\n", z_wire_str);
		}
	}

cleanup:;
	free(gates);
cleanup_wires:
	free(wires_sort);
	free(wire_values);
	return result;
}

static void parse_gates(char const * const * lines, size_t const * line_lengths, size_t n, 
		gate_t * gates, wire_sort_t * wires, aoc_ex_t * e) {
	size_t i = 0;
	do {
		if (line_lengths[i] < 17) {
			aoc_throw_invalid_file(e);
			return;
		}
		// strtoul may skip one whitespace, if the op was 2 chars instead of 3
		gate_t gate = {
			.left_in = strtoul(lines[i], NULL, ALPHANUMS),
			.right_in = strtoul(lines[i] + 7, NULL, ALPHANUMS),
			.out = strtoul(lines[i] + 14, NULL, ALPHANUMS),

			.next_unsolved_left = END,
			.next_unsolved_right = END
		};
		char const * op_str = lines[i] + 4;
		if (strncmp(op_str, "AND", 3) == 0) {
			gate.op = AND;
		} else if (strncmp(op_str, "OR", 2) == 0) {
			gate.op = OR;
		} else if (strncmp(op_str, "XOR", 3) == 0) {
			gate.op = XOR;
		} else {
			aoc_throw_invalid_file(e);
			return;
		}
		if (gate.left_in >= WIRES_COUNT || gate.right_in >= WIRES_COUNT || gate.out >= WIRES_COUNT) {
			aoc_throw_invalid_file(e);
			return;
		}

		gates[i] = gate;

		if (!wires[gate.left_in].exists)
			init(&(wires[gate.left_in]));

		if (!wires[gate.right_in].exists)
			init(&(wires[gate.right_in]));

		if (!wires[gate.out].exists)
			init(&(wires[gate.out]));

		++i;
	} while (i < n);
}

static bool sort_(gate_t * restrict in_gates, int in_i,
		gate_t * restrict out_gates, int * restrict out_i, wire_sort_t * restrict wires) {
	gate_t gate = in_gates[in_i];

	fprint(gate, stderr);
	if (wires[gate.left_in].has_value && wires[gate.right_in].has_value) {
		fputs("...has value now\n", stderr);

		out_gates[*out_i] = gate;
		++*out_i;
		
		wires[gate.out].has_value = true;
		
		// Try to resolve all gates that we can

		int * next = &(wires[gate.out].next_unsolved_left);
		while (*next != END) {
			if (sort_(in_gates, *next, out_gates, out_i, wires)) {
				// remove gate from next* chain
				*next = in_gates[*next].next_unsolved_left;
			} else {
				// continue to next gate
				next = &(in_gates[*next].next_unsolved_left);
			}
		}

		next = &(wires[gate.out].next_unsolved_right);
		while (*next != END) {
			if (sort_(in_gates, *next, out_gates, out_i, wires)) {
				// remove gate from next* chain
				*next = in_gates[*next].next_unsolved_right;
			} else {
				// continue to next gate
				next = &(in_gates[*next].next_unsolved_right);
			}
		}
		return true;
	} else {
		fputs("...will be resolved later\n", stderr);
		return false;
	}
}

static void sort_gates(gate_t * restrict in_gates, gate_t * restrict out_gates, size_t n, wire_sort_t * wires) {
	int out_i = 0;

	for (size_t i = 0; i < n; ++i) {
		bool resolved_now = sort_(in_gates, i, out_gates, &out_i, wires);

		if (!resolved_now) {
			gate_t * gate = &in_gates[i];

			// Add gate to be resolved later
			if (wires[gate->left_in].next_unsolved_left != END) {
				gate->next_unsolved_left = wires[gate->left_in].next_unsolved_left;
			}
			wires[gate->left_in].next_unsolved_left = i;

			if (wires[gate->right_in].next_unsolved_right != END) {
				gate->next_unsolved_right = wires[gate->right_in].next_unsolved_right;
			}
			wires[gate->right_in].next_unsolved_right = i;
		}
	}
	fprintf(stderr, "out gates: %d, gates total: %zu\n", out_i, n);
	// assert(out_i == n);
}

static void eval_gates(gate_t * gates, size_t gates_n, bool * wires) {
	for (size_t i = 0; i < gates_n; ++i) {
		gate_t gate = gates[i];

		bool left_in = wires[gate.left_in];
		bool right_in = wires[gate.right_in];
		bool out;
		switch (gate.op) {
			case AND:
				out = left_in && right_in;
				break;
			case OR:
				out = left_in || right_in;
				break;
			case XOR:
				out = left_in ^ right_in;
				break;
		}
		wires[gate.out] = out;
	}
}


int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
