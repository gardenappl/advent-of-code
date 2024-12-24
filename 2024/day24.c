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

	struct gate * next;
	// pointers to next unsolved gates
	struct gate * next_left_in;
	struct gate * next_right_in;
} gate_t;

typedef enum : char { VAL_NOT_INIT = 0, VAL_EMPTY, VAL_FALSE, VAL_TRUE } wire_val_t;

typedef struct {
	wire_val_t value;
	// pointers to next unsolved gates
	gate_t * next_left_in;
	gate_t * next_right_in;
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

static bool try_eval(wire_t * wires, gate_t gate) {
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

	wires[gate.out].value = out ? VAL_TRUE : VAL_FALSE;
	
	fprint(gate);
	if (out)
		fputs(" (= TRUE)\n", stderr);
	else
		fputs(" (= FALSE)\n", stderr);

	gate_t ** in_gate = &(wires[gate.out].next_left_in);
	while (*in_gate) {
		if (try_eval(wires, **in_gate)) {
			fprint(**in_gate);
			fprintf(stderr, " - removing\n");
			// remove gate from next_left chain
			*in_gate = (*in_gate)->next_left_in;
		} else {
			// skip to next gate
			in_gate = &((*in_gate)->next_left_in);
		}
	}

	in_gate = &(wires[gate.out].next_right_in);
	while (*in_gate) {
		if (try_eval(wires, **in_gate)) {
			fprint(**in_gate);
			fprintf(stderr, " - removing\n");
			// remove gate from next_right chain
			*in_gate = (*in_gate)->next_right_in;
		} else {
			// skip to next gate
			in_gate = &((*in_gate)->next_right_in);
		}
	}

	return true;
}


static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	wire_t * wires = assert_calloc(WIRES_COUNT, wire_t);
	gate_t * gates = NULL;
	fprintf(stderr, "%zu\n", sizeof(wire_t));

	size_t line_i = 0;
	while (line_i < lines_n && line_lengths[line_i] == 6) {
		char * end;
		int wire_num = strtoul(lines[line_i], &end, ALPHANUMS);
		if (wire_num >= WIRES_COUNT) {
			aoc_throw_invalid_file(e);
			goto cleanup;
		}
		// aoc_bit_array_set(has_value, wire_num, true);

		if (lines[line_i][5] == '1')
			wires[wire_num].value = VAL_TRUE;
		else
			wires[wire_num].value = VAL_FALSE;
		wires[wire_num].next_left_in = NULL;
		wires[wire_num].next_right_in = NULL;

		++line_i;
	}


	// for (size_t i = 0; i < WIRES_MAX; ++i) {
	// 	if (aoc_bit_array_get(has_value, i)) {
	// 		char wire_name[4];
	// 		wire_num_to_name(i, wire_name);
	// 		if (aoc_bit_array_get(value, i)) {
	// 			fprintf(stderr, "%s -> 1\n", wire_name);
	// 		} else {
	// 			fprintf(stderr, "%s -> 0\n", wire_name);
	// 		}
	// 	}
	// }
	
	++line_i;
	if (line_i >= lines_n) {
		aoc_throw_invalid_file(e);
		goto cleanup;
	}

	do {
		if (line_lengths[line_i] < 17) {
			aoc_throw_invalid_file(e);
			goto cleanup;
		}
		// strtoul may skip one whitespace, if the op was 2 chars instead of 3
		gate_t gate = {
			.left_in = strtoul(lines[line_i], NULL, ALPHANUMS),
			.right_in = strtoul(lines[line_i] + 7, NULL, ALPHANUMS),
			.out = strtoul(lines[line_i] + 14, NULL, ALPHANUMS)
		};
		char const * op_str = lines[line_i] + 4;
		if (strncmp(op_str, "AND", 3) == 0) {
			gate.op = AND;
		} else if (strncmp(op_str, "OR", 2) == 0) {
			gate.op = OR;
		} else if (strncmp(op_str, "XOR", 3) == 0) {
			gate.op = XOR;
		} else {
			aoc_throw_invalid_file(e);
			goto cleanup;
		}

		if (try_eval(wires, gate)) {
			// no need to store it for later
			goto continue_next_line;
		} else {
			fprint(gate);
			fprintf(stderr, " - storing for later...\n");
			gate_t * next_gate = gates;
			gates = assert_malloc(1, gate_t);
			*gates = gate;
			gates->next = next_gate;

			if (wires[gate.left_in].value == VAL_NOT_INIT) {
				wires[gate.left_in].value = VAL_EMPTY;
				wires[gate.left_in].next_left_in = gates;
				gates->next_left_in = NULL;
			} else {
				gates->next_left_in = wires[gate.left_in].next_left_in;
				wires[gate.left_in].next_left_in = gates;
			}

			if (wires[gate.right_in].value == VAL_NOT_INIT) {
				wires[gate.right_in].value = VAL_EMPTY;
				wires[gate.right_in].next_right_in = gates;
				gates->next_right_in = NULL;
			} else {
				gates->next_right_in = wires[gate.right_in].next_right_in;
				wires[gate.right_in].next_right_in = gates;
			}
		}

continue_next_line:
		++line_i;
	} while (line_i < lines_n);

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
			case VAL_NOT_INIT:
				goto end_bits;
			default:
				aoc_throw_fail(e, "empty value for z wire!");
				goto cleanup;
		}
	}
end_bits:;

cleanup:;
	gate_t * gate = gates;
	while (gate) {
		gate_t * next_gate = gate->next;
		free(gate);
		gate = next_gate;
	}
	free(wires);
	return result;
}

int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
