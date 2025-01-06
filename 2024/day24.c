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
#define SWAPPED_WIRES_N 8

#define MSG_NO_OP "did not find gate with expected op"


AOC_ASSERT_SANE_ENCODING


typedef enum : char { WIRE_NONE = 0, WIRE_FALSE, WIRE_TRUE } wire_val_t;
typedef enum : char { OP_NONE = 0, OP_AND, OP_OR, OP_XOR } gate_op_t;

typedef struct {
	int left_in;
	int right_in;
	gate_op_t op;
	int out;
} gate_t;


typedef struct {
	gate_t gate;

	bool has_next_left;
	int next_left;

	bool has_next_right;
	int next_right;
} gate_list_t;

typedef enum : char { STATUS_NONE, STATUS_VERIFIED, STATUS_SWAPPED } wire_status_t;

typedef struct {
	bool has_next_left;
	int next_left;

	bool has_next_right;
	int next_right;

	wire_status_t status;
} wire_list_t;


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


static int parse_wire(char const * line, wire_val_t * wires, aoc_ex_t * e) {
	char wire_name[WIRE_NAME_LEN + 1];
	char value;
	if (sscanf(line, "%"WIRE_NAME_LEN_S"s: %c", wire_name, &value) != 2) {
		aoc_throw_invalid_file(e);
		return -1;
	}
	int wire_num = strtoul(wire_name, NULL, ALPHANUMS);
	// fprintf(stderr, "wire: %s value: %d\n", wire_name, wire_num);
	if (wires) {
		if (value == '1')
			wires[wire_num] = WIRE_TRUE;
		else
			wires[wire_num] = WIRE_FALSE;
	}
	return wire_num;
}

static gate_t parse_gate(char const * line, aoc_ex_t * e) {
	char left_wire_name[WIRE_NAME_LEN + 1];
	char op_name[3 + 1];
	char right_wire_name[WIRE_NAME_LEN + 1];
	char out_wire_name[WIRE_NAME_LEN + 1];
	if (sscanf(line, "%"WIRE_NAME_LEN_S"s %3s %"WIRE_NAME_LEN_S"s -> %"WIRE_NAME_LEN_S"s",
				left_wire_name, op_name, right_wire_name, out_wire_name) != 4) {
		aoc_throw_invalid_file(e);
		return (gate_t){ 0 };
	}
	return (gate_t){
		.left_in = strtoul(left_wire_name, NULL, ALPHANUMS),
		.right_in = strtoul(right_wire_name, NULL, ALPHANUMS),
		.op = (op_name[0] == 'A') ? OP_AND : ((op_name[0] == 'O') ? OP_OR : OP_XOR),
		.out = strtoul(out_wire_name, NULL, ALPHANUMS)
	};
}


static int get_bit_wire(char c, int bit) {
	char wire_name[4] = { c, (bit / 10) + '0', (bit % 10) + '0', '\0' };
	return strtoul(wire_name, NULL, ALPHANUMS);
}


static int64_t solve1(char const * const * lines, size_t lines_n, size_t const * line_lengths, aoc_ex_t * e) {
	int64_t result = 0;

	wire_val_t * wires = assert_calloc(WIRES_COUNT, wire_val_t);
	gate_t * gates = assert_calloc(WIRES_COUNT, gate_t);

	size_t line_i = 0;
	for (; line_i < lines_n; ++line_i) {
		if (line_lengths[line_i] != 6)
			break;
		parse_wire(lines[line_i], wires, e);
		if (*e)
			goto cleanup;
	}

	// skip empty line
	++line_i;

	for (; line_i < lines_n; ++line_i) {
		gate_t gate = parse_gate(lines[line_i], e);
		if (*e)
			goto cleanup;
		fprint_gate(gate, stderr);
		fputc('\n', stderr);
		gates[gate.out] = gate;
	}

	for (int z_bit = 0; z_bit < 63; ++z_bit) {
		int z_wire = get_bit_wire('z', z_bit);

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
			result |= (1LL << z_bit);
	}
	fputc('\n', stderr);
cleanup:
	free(wires);
	free(gates);
	return result;
}

static int find_gate_with(int in1, gate_op_t op, int in2,
		gate_list_t const * restrict gate_list, 
		wire_list_t const * restrict wire_list) {

	bool has_next = wire_list[in1].has_next_left;
	int gate_i = wire_list[in1].next_left;
	while (has_next) {
		gate_t gate = gate_list[gate_i].gate;
		if (gate.right_in == in2 && gate.op == op)
			return gate_i;

		has_next = gate_list[gate_i].has_next_left;
		gate_i = gate_list[gate_i].next_left;
	}

	has_next = wire_list[in1].has_next_right;
	gate_i = wire_list[in1].next_right;
	while (has_next) {
		gate_t gate = gate_list[gate_i].gate;
		if (gate.left_in == in2 && gate.op == op)
			return gate_i;

		has_next = gate_list[gate_i].has_next_right;
		gate_i = gate_list[gate_i].next_right;
	}
	return -1;
}

typedef struct {
	int sus_n;
	union {
		struct {
			int sus_wires[3];
		};
		int next_carry_bit;
	};
} sus_wires_t;

static sus_wires_t verify_adder(int bit, int carry_bit,
		gate_list_t const * restrict gate_list, 
		wire_list_t * restrict wire_list, aoc_ex_t * e) {
	int x_wire = get_bit_wire('x', bit);
	int y_wire = get_bit_wire('y', bit);


	int xor_gate = find_gate_with(x_wire, OP_XOR, y_wire, gate_list, wire_list);
	if (xor_gate == -1) {
		aoc_throw_fail(e, MSG_NO_OP);
		return (sus_wires_t){ 0 };
	}
	int xor_wire = gate_list[xor_gate].gate.out;


	int and_gate = find_gate_with(x_wire, OP_AND, y_wire, gate_list, wire_list);
	if (and_gate == -1) {
		aoc_throw_fail(e, MSG_NO_OP);
		return (sus_wires_t){ 0 };
	}
	int and_wire = gate_list[and_gate].gate.out;


	int xor_gate2, and_gate2, or_gate;
	int xor_wire2, and_wire2, or_wire;
	int out_bit;
	int next_carry_bit;

	if (carry_bit >= 0) {
		xor_gate2 = find_gate_with(xor_wire, OP_XOR, carry_bit, 
				gate_list, wire_list);
		if (xor_gate2 == -1)
			return (sus_wires_t){ .sus_n = 2, .sus_wires = { xor_gate, carry_bit }};
		xor_wire2 = gate_list[xor_gate2].gate.out;


		and_gate2 = find_gate_with(xor_wire, OP_AND, carry_bit, 
				gate_list, wire_list);
		if (and_gate2 == -1)
			return (sus_wires_t){ .sus_n = 2, .sus_wires = { xor_gate, carry_bit }};
		and_wire2 = gate_list[and_gate2].gate.out;


		or_gate = find_gate_with(gate_list[and_gate].gate.out, OP_OR, gate_list[and_gate2].gate.out, 
				gate_list, wire_list);
		if (or_gate == -1)
			return (sus_wires_t){ .sus_n = 2, .sus_wires = { and_gate, and_gate2 }};
		or_wire = gate_list[or_gate].gate.out;


		out_bit = xor_wire2;
		next_carry_bit = or_wire;
	} else {
		out_bit = xor_wire;
		next_carry_bit = and_wire;
	}

	if (out_bit != get_bit_wire('z', bit)) {
		return (sus_wires_t){ .sus_n = 1, .sus_wires = { out_bit } };
	}

	wire_list[xor_wire].status = STATUS_VERIFIED;
	wire_list[and_wire].status = STATUS_VERIFIED;
	if (carry_bit >= 0) {
		wire_list[xor_wire2].status = STATUS_VERIFIED;
		wire_list[and_wire2].status = STATUS_VERIFIED;
		wire_list[or_wire].status = STATUS_VERIFIED;
	}

	return (sus_wires_t){ .sus_n = 0, .next_carry_bit = next_carry_bit };
}


static char * solve2(char const * const * lines, size_t lines_n, size_t const * line_lengths, aoc_ex_t * e) {
	char * msg = NULL;

	int64_t result = 0;

	int x_bits = 0;
	int y_bits = 0;

	size_t line_i = 0;
	for (; line_i < lines_n; ++line_i) {
		if (line_lengths[line_i] != 6)
			break;
		int wire = parse_wire(lines[line_i], NULL, e);
		if (*e)
			goto cleanup;
		if (wire == get_bit_wire('x', x_bits)) {
			++x_bits;
		}
		else if (wire == get_bit_wire('y', y_bits)) {
			++y_bits;
		} else {
			aoc_throw_fail(e, "can't validate adder with extra inputs");
			return NULL;
		}
	}
	if (x_bits != y_bits) {
		aoc_throw_invalid_file(e);
		return NULL;
	}

	wire_list_t * wire_list = assert_calloc(WIRES_COUNT, wire_list_t);
	gate_list_t * gate_list = assert_calloc(WIRES_COUNT, gate_list_t);

	// skip empty line
	++line_i;

	for (; line_i < lines_n; ++line_i) {
		gate_t gate = parse_gate(lines[line_i], e);
		if (*e)
			goto cleanup;
		fprint_gate(gate, stderr);
		fputc('\n', stderr);
		gate_list[gate.out].gate = gate;

		if (wire_list[gate.left_in].has_next_left) {
			int next = wire_list[gate.left_in].next_left;
			wire_list[gate.left_in].next_left = gate.out;

			gate_list[gate.out].next_left = next;
			gate_list[gate.out].has_next_left = true;
		} else {
			wire_list[gate.left_in].next_left = gate.out;
			wire_list[gate.left_in].has_next_left = true;
		}

		if (wire_list[gate.right_in].has_next_right) {
			int next = wire_list[gate.right_in].next_right;
			wire_list[gate.right_in].next_right = gate.out;

			gate_list[gate.out].next_right = next;
			gate_list[gate.out].has_next_right = true;
		} else {
			wire_list[gate.right_in].next_right = gate.out;
			wire_list[gate.right_in].has_next_right = true;
		}
	}

	int swapped_wires[SWAPPED_WIRES_N];
	int swapped_wires_n = 0;

	int carry_bit = -1;
	for (int bit = 0; bit < x_bits; ++bit) {
		sus_wires_t sus = verify_adder(bit, carry_bit, gate_list, wire_list, e);
		if (*e)
			goto cleanup;

		for (int i = 0; i < sus.sus_n; ++i) {
			if (swapped_wires_n == SWAPPED_WIRES_N) {
				aoc_throw_fail(e, "Too many swapped wires!");
				goto cleanup;
			}
			int sus_wire = sus.sus_wires[i];
			char sus_wire_name[WIRE_NAME_LEN + 1];
			wire_num_to_name(sus_wire, sus_wire_name);
			fprintf(stderr, "Suspicious gate: %s\n", sus_wire_name);

			for (int swap_wire = 0; swap_wire < WIRES_COUNT; ++swap_wire) {
				if (gate_list[swap_wire].gate.op == OP_NONE)
					continue;
				switch (wire_list[swap_wire].status) {
					case STATUS_VERIFIED:
						continue;
					case STATUS_SWAPPED:
						aoc_throw_fail(e, "swapped in previous adder?");
						goto cleanup;
					case STATUS_NONE:
						break;
				}

				char swap_wire_name[WIRE_NAME_LEN + 1];
				wire_num_to_name(swap_wire, swap_wire_name);
				// fprintf(stderr, "Try swapping with: %s\n", swap_wire_name);

				// Swap
				gate_list[swap_wire].gate.out = sus_wire;
				wire_list[swap_wire].status = STATUS_SWAPPED;
				gate_list[sus_wire].gate.out = swap_wire;
				wire_list[sus_wire].status = STATUS_SWAPPED;

				sus_wires_t sus2 = verify_adder(bit, carry_bit, gate_list, wire_list, e);
				if (*e)
					goto cleanup;

				if (sus2.sus_n > 0) {
					// Undo swap and try another
					gate_list[swap_wire].gate.out = swap_wire;
					wire_list[swap_wire].status = STATUS_NONE;
					gate_list[sus_wire].gate.out = sus_wire;
					wire_list[sus_wire].status = STATUS_NONE;
				} else {
					// verify_adder should set verified status
					swapped_wires[swapped_wires_n] = sus_wire;
					swapped_wires[swapped_wires_n + 1] = swap_wire;
					swapped_wires_n += 2;

					fprintf(stderr, "Swapped %s and %s.\n", sus_wire_name, swap_wire_name);
					carry_bit = sus2.next_carry_bit;
					goto continue_next_adder;
				}
			}
		}

		if (sus.sus_n == 0) {
			carry_bit = sus.next_carry_bit;
		} else {
			aoc_throw_fail(e, "Could not fix adder.");
			goto cleanup;
		}
continue_next_adder:;
	}

	qsort(swapped_wires, swapped_wires_n, sizeof(int), aoc_compare_int);

	size_t size = strlen("XXX,") * SWAPPED_WIRES_N;
	msg = assert_calloc(size, char);
	
	char * s = msg;
	for (int i = 0; i < swapped_wires_n; ++i) {
		char wire_name[4];
		wire_num_to_name(swapped_wires[i], wire_name);
		if (i != swapped_wires_n - 1)
			wire_name[3] = ',';
		memcpy(s, wire_name, 4 * sizeof(char));
		s += 4;
	}

cleanup:
	free(wire_list);
	free(gate_list);
	return msg;
}


static char * solve(char const * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	if (part == 1) {
		return make_result_str(solve1(lines, lines_n, line_lengths, e));
	} else {
		return solve2(lines, lines_n, line_lengths, e);
	}
}


int main(int argc, char * argv[]) {
	aoc_main_lines_to_str(argc, argv, 2, solve);
}
