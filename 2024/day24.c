#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

#define ALPHANUMS 36
#define WIRES_COUNT (ALPHANUMS * ALPHANUMS * ALPHANUMS)

AOC_ASSERT_SANE_ENCODING


typedef enum : char { AND, OR, XOR } gate_op_t;

typedef struct gate {
	int left_in;
	int right_in;
	gate_op_t op;
	int out;

	// pointers to next unsolved gates
	ptrdiff_t next_left_in;
	ptrdiff_t next_right_in;
} gate_t;

typedef enum : char { VAL_NOT_INIT = 0, VAL_EMPTY, VAL_FALSE, VAL_TRUE } wire_val_t;

typedef struct {
	wire_val_t value;
	// pointers to next unsolved gates
	ptrdiff_t next_left_in;
	ptrdiff_t next_right_in;
} wire_t;


static void wire_num_to_name(int num, char name[4]) {
	name[3] = '\0';
	for (size_t i = 0; i < 3; ++i) {
		int digit = num % ALPHANUMS;
		name[2 - i] = (digit >= 10) ? ('a' + (digit - 10)) : ('0' + digit);
		num /= ALPHANUMS;
	}
}


static void fprint(gate_t gate) {
	char left_in[4];
	wire_num_to_name(gate.left_in, left_in);
	char right_in[4];
	wire_num_to_name(gate.right_in, right_in);
	char out[4];
	wire_num_to_name(gate.out, out);
	fprintf(stderr, "%s %s %s -> %s", 
			left_in, 
			(gate.op == AND) ? "AND": ((gate.op == OR) ? "OR": "XOR"), 
			right_in,
			out);
}

static bool try_eval(wire_t * wires, gate_t * gates, size_t i) {
	gate_t gate = gates[i];
	bool left_in;
	switch (wires[gate.left_in].value) {
		case VAL_FALSE:
			left_in = false;
			break;
		case VAL_TRUE:
			left_in = true;
			break;
		default:
			return false;
	}
	bool right_in;
	switch (wires[gate.right_in].value) {
		case VAL_FALSE:
			right_in = false;
			break;
		case VAL_TRUE:
			right_in = true;
			break;
		default:
			return false;
	}
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

	bool out_wire_init = wires[gate.out].value != VAL_NOT_INIT;
	wires[gate.out].value = out ? VAL_TRUE : VAL_FALSE;
	
	fprint(gate);
	if (out)
		fputs(" (= TRUE)\n", stderr);
	else
		fputs(" (= FALSE)\n", stderr);

	if (!out_wire_init)
		return true;

	ptrdiff_t * in_gate_i = &(wires[gate.out].next_left_in);
	while (*in_gate_i != -1) {
		fprint(gates[*in_gate_i]);
		fprintf(stderr, " - gate (i = %zd)\n", *in_gate_i);
		if (try_eval(wires, gates, *in_gate_i)) {
			fprint(gates[*in_gate_i]);
			fprintf(stderr, " - removing\n");
			// remove gate from next_left chain
			*in_gate_i = gates[*in_gate_i].next_left_in;
		} else {
			// skip to next gate
			in_gate_i = &(gates[*in_gate_i].next_left_in);
		}
	}

	in_gate_i = &(wires[gate.out].next_right_in);
	while (*in_gate_i != -1) {
		if (try_eval(wires, gates, *in_gate_i)) {
			fprint(gates[*in_gate_i]);
			fprintf(stderr, " - removing\n");
			// remove gate from next_right chain
			*in_gate_i = gates[*in_gate_i].next_right_in;
		} else {
			// skip to next gate
			in_gate_i = &(gates[*in_gate_i].next_right_in);
		}
	}

	return true;
}

static void parse_gates(char const * const * lines, size_t const * line_lengths, size_t n, 
		gate_t * gates, wire_t * wires, aoc_ex_t * e);
static void sort_gates(gate_t * gates, size_t n, wire_t * wires);
static void eval_gates(gate_t * gates, size_t n, wire_t * wires);


static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	wire_t * wires = assert_calloc(WIRES_COUNT, wire_t);
	fprintf(stderr, "%zu\n", sizeof(wire_t));

	size_t line_i = 0;
	while (line_i < lines_n && line_lengths[line_i] == 6) {
		char * end;
		int wire_num = strtoul(lines[line_i], &end, ALPHANUMS);
		if (wire_num >= WIRES_COUNT) {
			aoc_throw_invalid_file(e);
			goto cleanup_wires;
		}

		if (lines[line_i][5] == '1')
			wires[wire_num].value = VAL_TRUE;
		else
			wires[wire_num].value = VAL_FALSE;
		wires[wire_num].next_left_in = -1;
		wires[wire_num].next_right_in = -1;

		++line_i;
	}

	
	++line_i;
	if (line_i >= lines_n) {
		aoc_throw_invalid_file(e);
		goto cleanup;
	}
	size_t gates_n = lines_n - line_i;
	gate_t * gates = assert_malloc(gates_n, gate_t);

	parse_gates(lines + line_i, line_lengths + line_i, gates_n, gates, wires, e);
	if (*e) {
		goto cleanup;
	}

	// sort_gates(gates, gates_n, wires);

	int64_t result = 0;
	int z00_wire = (ALPHANUMS - 1) * ALPHANUMS * ALPHANUMS;
	char z_wire_str[4];

	wire_num_to_name(z00_wire, z_wire_str);
	fprintf(stderr, "values, starting from %s:\n", z_wire_str);

	for (int bit = 0; bit < 64; ++bit) {
		int z_wire = z00_wire + (bit / 10) * ALPHANUMS + (bit % 10);
		switch (wires[z_wire].value) {
			case VAL_TRUE:
				wire_num_to_name(z_wire, z_wire_str);
				fprintf(stderr, "%s is TRUE\n", z_wire_str);
				result |= (1L << bit);
				break;
			case VAL_FALSE:
				wire_num_to_name(z_wire, z_wire_str);
				fprintf(stderr, "%s is FALSE\n", z_wire_str);
				break;
			default:
				goto end_bits;
		}
	}
end_bits:;

cleanup:;
	free(gates);
cleanup_wires:
	free(wires);
	return result;
}

static void parse_gates(char const * const * lines, size_t const * line_lengths, size_t n, 
		gate_t * gates, wire_t * wires, aoc_ex_t * e) {
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
			.out = strtoul(lines[i] + 14, NULL, ALPHANUMS)
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

		if (!try_eval(wires, gates, i)) {
			fprint(gate);
			fprintf(stderr, " - storing for later...\n");
			if (wires[gate.left_in].value == VAL_NOT_INIT) {
				wires[gate.left_in].value = VAL_EMPTY;
				wires[gate.left_in].next_left_in = i;
				wires[gate.left_in].next_right_in = -1;
				gates[i].next_left_in = -1;
			} else {
				gates[i].next_left_in = wires[gate.left_in].next_left_in;
				wires[gate.left_in].next_left_in = i;
			}

			if (wires[gate.right_in].value == VAL_NOT_INIT) {
				wires[gate.right_in].value = VAL_EMPTY;
				wires[gate.right_in].next_left_in = -1;
				wires[gate.right_in].next_right_in = i;
				gates[i].next_right_in = -1;
			} else {
				gates[i].next_right_in = wires[gate.right_in].next_right_in;
				wires[gate.right_in].next_right_in = i;
			}
		}

		++i;
	} while (i < n);
}


int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
