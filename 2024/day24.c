#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

#define ALPHANUMS 36
#define WIRES_COUNT (ALPHANUMS * ALPHANUMS * ALPHANUMS)
#define WIRE_NAME_LEN 3
#define WIRE_NAME_LEN_S AOC_STR(WIRE_NAME_LEN)


AOC_ASSERT_SANE_ENCODING


typedef enum : char { WIRE_NONE = 0, WIRE_FALSE, WIRE_TRUE } wire_val_t;
typedef enum : char { OP_NONE = 0, OP_AND, OP_OR, OP_XOR } gate_op_t;

typedef struct {
	int left_in;
	int right_in;
	gate_op_t op;
	int out;
} gate_t;



static void wire_num_to_name(int num, char name[4]) {
	name[3] = '\0';
	for (size_t i = 0; i < 3; ++i) {
		int digit = num % ALPHANUMS;
		name[2 - i] = (digit >= 10) ? ('a' + (digit - 10)) : ('0' + digit);
		num /= ALPHANUMS;
	}
}


static void fprint_gate(gate_t gate, FILE * file) {
	char left_in[WIRE_NAME_LEN + 1];
	wire_num_to_name(gate.left_in, left_in);
	char right_in[WIRE_NAME_LEN + 1];
	wire_num_to_name(gate.right_in, right_in);
	char out[WIRE_NAME_LEN + 1];
	wire_num_to_name(gate.out, out);
	fprintf(file, "%s %s %s -> %s", 
			left_in, 
			(gate.op == OP_AND) ? "AND": ((gate.op == OP_OR) ? "OR": "XOR"), 
			right_in,
			out);
}

static bool eval_gate(int out_wire, gate_t * gates, wire_val_t * wires, aoc_ex_t * e) {
	gate_t gate = gates[out_wire];
	if (gate.op == OP_NONE) {
		aoc_throw_fail(e, "no gate!");
		return false;
	}
	bool left_in;
	switch (wires[gate.left_in]) {
		default:
		case WIRE_NONE:
			left_in = eval_gate(gate.left_in, gates, wires, e);
			if (*e)
				return false;
			break;
		case WIRE_FALSE:
			left_in = false;
			break;
		case WIRE_TRUE:
			left_in = true;
			break;
	}

	bool right_in;
	switch (wires[gate.right_in]) {
		default:
		case WIRE_NONE:
			right_in = eval_gate(gate.right_in, gates, wires, e);
			if (*e)
				return false;
			break;
		case WIRE_FALSE:
			right_in = false;
			break;
		case WIRE_TRUE:
			right_in = true;
			break;
	}

	bool out;
	switch(gate.op) {
		case OP_AND:
			out = left_in && right_in;
			break;
		case OP_OR:
			out = left_in || right_in;
			break;
		default:
		case OP_XOR:
			out = left_in ^ right_in;
			break;
	}
	wires[gate.out] = out ? WIRE_TRUE : WIRE_FALSE;
	return out;
}

static int64_t solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	int64_t result = 0;

	wire_val_t * wires = assert_calloc(WIRES_COUNT, wire_val_t);
	gate_t * gates = assert_calloc(WIRES_COUNT, gate_t);

	size_t line_i = 0;
	for (; line_i < lines_n; ++line_i) {
		if (line_lengths[line_i] != 6)
			break;

		char wire_name[WIRE_NAME_LEN + 1];
		char value;
		if (sscanf(lines[line_i], "%"WIRE_NAME_LEN_S"s: %c", wire_name, &value) != 2) {
			aoc_throw_invalid_file(e);
			goto cleanup;
		}
		int wire_num = strtoul(wire_name, NULL, ALPHANUMS);
		// fprintf(stderr, "wire: %s value: %d\n", wire_name, wire_num);
		if (value == '1')
			wires[wire_num] = WIRE_TRUE;
		else
			wires[wire_num] = WIRE_FALSE;
	}

	// skip empty line
	++line_i;

	for (; line_i < lines_n; ++line_i) {
		char left_wire_name[WIRE_NAME_LEN + 1];
		char op_name[3 + 1];
		char right_wire_name[WIRE_NAME_LEN + 1];
		char out_wire_name[WIRE_NAME_LEN + 1];
		if (sscanf(lines[line_i], "%"WIRE_NAME_LEN_S"s %3s %"WIRE_NAME_LEN_S"s -> %"WIRE_NAME_LEN_S"s",
					left_wire_name, op_name, right_wire_name, out_wire_name) != 4) {
			aoc_throw_invalid_file(e);
			goto cleanup;
		}
		gate_t gate = {
			.left_in = strtoul(left_wire_name, NULL, ALPHANUMS),
			.right_in = strtoul(right_wire_name, NULL, ALPHANUMS),
			.op = (op_name[0] == 'A') ? OP_AND : ((op_name[0] == 'O') ? OP_OR : OP_XOR),
			.out = strtoul(out_wire_name, NULL, ALPHANUMS)
		};
		fprint_gate(gate, stderr);
		fputc('\n', stderr);
		gates[gate.out] = gate;
	}

	int z00_wire = (ALPHANUMS - 1) * ALPHANUMS * ALPHANUMS;
	for (int z_bit = 0; z_bit < 63; ++z_bit) {
		int z_wire = z00_wire + (z_bit / 10) * ALPHANUMS + (z_bit % 10);

		char z_wire_name[4];
		wire_num_to_name(z_wire, z_wire_name);
		fprintf(stderr, "%s = ", z_wire_name);

		if (gates[z_wire].op == OP_NONE)
			break;

		bool z_bit_value = eval_gate(z_wire, gates, wires, e);
		fputs(z_bit_value ? "1\n" : "0\n", stderr);
		if (*e)
			goto cleanup;
		if (z_bit_value)
			result |= (1L << z_bit);
	}
	fputc('\n', stderr);
cleanup:
	free(wires);
	free(gates);
	return result;
}


int main(int argc, char * argv[]) {
	aoc_main_const_lines(argc, argv, 1, solve);
}
