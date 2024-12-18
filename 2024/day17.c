#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

typedef enum : char {
	/**
	 * The adv instruction (opcode 0) performs division. The numerator is the value in the A register. The denominator is found by raising 2 to the power of the instruction's combo operand. (So, an operand of 2 would divide A by 4 (2^2); an operand of 5 would divide A by 2^B.) The result of the division operation is truncated to an integer and then written to the A register.
	 */
	ADV = 0,
	/**
	 * The bxl instruction (opcode 1) calculates the bitwise XOR of register B and the instruction's literal operand, then stores the result in register B.
	 */
	BXL = 1,
	/* *
	 * The bst instruction (opcode 2) calculates the value of its combo operand modulo 8 (thereby keeping only its lowest 3 bits), then writes that value to the B register.
	 */
	BST = 2,
	/**
	 * The jnz instruction (opcode 3) does nothing if the A register is 0. However, if the A register is not zero, it jumps by setting the instruction pointer to the value of its literal operand; if this instruction jumps, the instruction pointer is not increased by 2 after this instruction.
	 */
	JNZ = 3,
	/**
	 * The bxc instruction (opcode 4) calculates the bitwise XOR of register B and register C, then stores the result in register B. (For legacy reasons, this instruction reads an operand but ignores it.)
	 */
	BXC = 4,
	/**
	 * The out instruction (opcode 5) calculates the value of its combo operand modulo 8, then outputs that value. (If a program outputs multiple values, they are separated by commas.)
	 */
	OUT = 5,
	/**
	 * The bdv instruction (opcode 6) works exactly like the adv instruction except that the result is stored in the B register. (The numerator is still read from the A register.)
	 */
	BDV = 6,
	/**
	 * The cdv instruction (opcode 7) works exactly like the adv instruction except that the result is stored in the C register. (The numerator is still read from the A register.)
	 */
	CDV = 7
} inst_t;

/**
 * 21 3-bit values can be stored in one 64-bit int.
 */
#define MAX_BIT_OUTPUT 63

typedef struct {
	// Registers
	int64_t a;
	int64_t b;
	int64_t c;

	int32_t * rom;
	size_t pc;
	size_t rom_size;

	int64_t output;
	char output_bits_written;
} computer_t;

static int64_t combo(computer_t comp, inst_t operand) {
	switch (operand) {
		case 4:
			return comp.a;
		case 5:
			return comp.b;
		case 6:
			return comp.c;
		default:
			return operand;
	}
}

static void output(computer_t comp, inst_t value, aoc_ex_t * e) {
	if (comp.output_bits_written == MAX_BIT_OUTPUT) {
		aoc_throw_fail(e, "Too much output");
		return;
	}
	for (int i = 0; i < 3; ++i) {
		comp.output <<= 1;
		comp.output |= (value & 1);
		value >>= 1;
	}
	comp.output_bits_written += 3;
}

static bool step(computer_t comp, aoc_ex_t * e) {
	if (comp.pc >= comp.rom_size - 1)
		return false;

	inst_t inst = comp.rom[comp.pc];
	inst_t operand = comp.rom[comp.pc + 1];
	int64_t combo_operand;
	switch (inst) {
		case ADV:
			combo_operand = combo(comp, operand);
			if (combo_operand >= (sizeof(int64_t) * CHAR_BIT - 1))
				comp.a = 0;
			else
				comp.a /= (1 << combo_operand);
			break;
		case BXL:
			comp.b ^= operand;
			break;
		case BST:
			comp.b = combo(comp, operand) % 8;
			break;
		case JNZ:
			if (comp.a) {
				comp.pc = operand;
				return true;
			}
		case BXC:
			comp.b = comp.b ^ comp.c;
			break;
		case OUT:
			output(comp, combo(comp, operand) % 8, e);
			if (*e)
				return false;
			break;
		case BDV:
			combo_operand = combo(comp, operand);
			if (combo_operand >= (sizeof(int64_t) * CHAR_BIT - 1))
				comp.b = 0;
			else
				comp.b = comp.a / (1 << combo_operand);
			break;
		case CDV:
			combo_operand = combo(comp, operand);
			if (combo_operand >= (sizeof(int64_t) * CHAR_BIT - 1))
				comp.c = 0;
			else
				comp.c = comp.a / (1 << combo_operand);
			break;
	}
	comp.pc += 2;
	return true;
}

static int64_t solve(char * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	if (lines_n != 5) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
		return -1;
	}
	for (size_t i = 0; i < lines_n; ++i) {
		fprintf(stderr, "length: %zu, line: \"%s\"\n", line_lengths[i], lines[i]);
	}
	return 0;
}

int main(int argc, char * argv[]) {
	aoc_main_lines(argc, argv, 1, solve);
}
