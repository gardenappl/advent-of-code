#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

// I spent so long debugging this..
// Not realizing that you ACTUALLY have to submit the answer as a list,
// with commas.
// I thought I was clever for joining the output values into a integer value.
#define DEBUG 1

typedef enum : int32_t {
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


typedef struct {
	// Registers
	uint64_t a;
	uint64_t b;
	uint64_t c;

	inst_t * rom;
	size_t pc;
	size_t rom_size;

	uint64_t output;
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

static void output(computer_t * comp, inst_t value, aoc_ex_t * e) {
	if (comp->output * 10 < comp->output) {
		aoc_throw_fail(e, "Too much output");
		return;
	}
	comp->output *= 10;
	comp->output += value;
	fprintf(stderr, "%" PRId32 ",", value);
}

static bool step(computer_t * comp, aoc_ex_t * e) {
	if (comp->pc >= comp->rom_size - 1)
		return false;

	inst_t inst = comp->rom[comp->pc];
	inst_t operand = comp->rom[comp->pc + 1];
	switch (inst) {
		case ADV:
#ifdef DEBUG
			fprintf(stderr, "ADV A=%" PRIo64 " combo(%" PRIo32 ")=%" PRIo64 "\n", 
					comp->a, operand, combo(*comp, operand));
#endif
			comp->a >>= combo(*comp, operand);
			break;
		case BXL:
#ifdef DEBUG
			fprintf(stderr, "BXL B=%" PRIo64 " op=%" PRIo32 "\n", comp->b, operand);
#endif
			comp->b ^= operand;
			break;
		case BST:
#ifdef DEBUG
			fprintf(stderr, "BST B=%" PRIo64 " combo(%" PRIo32 ")=%" PRIo64 "\n", 
					comp->b, operand, combo(*comp, operand));
#endif
			comp->b = combo(*comp, operand) % 8;
			break;
		case JNZ:
#ifdef DEBUG
			fprintf(stderr, "JNZ A=%" PRIo64 " op=%" PRIo32 "\n", comp->a, operand);
#endif
			if (comp->a) {
				comp->pc = operand;
				return true;
			}
			break;
		case BXC:
#ifdef DEBUG
			fprintf(stderr, "BXC B=%" PRIo64 " C=%" PRIo64 "\n", comp->b, comp->c);
#endif
			comp->b ^= comp->c;
			break;
		case OUT:
#ifdef DEBUG
			fprintf(stderr, "OUT combo(%" PRIo32 ")=%" PRIo64 "\n", 
					operand, combo(*comp, operand));
#endif
			output(comp, combo(*comp, operand) % 8, e);
			if (*e)
				return false;
			break;
		case BDV:
#ifdef DEBUG
			fprintf(stderr, "BDV A=%" PRIo64 " combo(%" PRIo32 ")=%" PRIo64 "\n", 
					comp->a, operand, combo(*comp, operand));
#endif
			comp->b = comp->a >> combo(*comp, operand);
			break;
		case CDV:
#ifdef DEBUG
			fprintf(stderr, "CDV A=%" PRIo64 " combo(%" PRIo32 ")=%" PRIo64 "\n", 
					comp->a, operand, combo(*comp, operand));
#endif
			comp->c = comp->a >> combo(*comp, operand);
			break;
	}
	comp->pc += 2;
	return true;
}

static void exec(computer_t * comp, aoc_ex_t * e) {
	bool has_next_step;
	do {
#ifdef DEBUG
		for (size_t i = 0; i < comp->rom_size; i += 2) {
			if (comp->pc == i)
				fprintf(stderr, "(%" PRIo32 " %" PRIo32 ")", comp->rom[i], comp->rom[i + 1]);
			else
				fprintf(stderr, " %" PRIo32 " %" PRIo32 " ", comp->rom[i], comp->rom[i + 1]);
		}
		fprintf(stderr, "\nA=%.10" PRIo64 " B=%.10" PRIo64 " C=%.10" PRIo64 " pc=%zu\n", 
				comp->a, comp->b, comp->c, comp->pc);
#endif
		has_next_step = step(comp, e);
#ifdef DEBUG
		if (has_next_step) {
			fprintf(stderr, "A=%.10" PRIo64 " B=%.10" PRIo64 " C=%.10" PRIo64 " pc=%zu\n\n", 
					comp->a, comp->b, comp->c, comp->pc);
			fgetc(stdin);
		}
#endif
		if (*e)
			return;
	} while (has_next_step);
	fputc('\n', stderr);
}

static int64_t solve(char * const * lines, size_t lines_n, size_t const * line_lengths, int32_t part, aoc_ex_t * e) {
	if (lines_n != 5) {
		aoc_throw(e, AOC_EX_DATA_ERR, "wrong number of lines");
		return -1;
	}
	computer_t comp = { 0 };
	if (sscanf(lines[0], "Register A: %" PRIu64, &comp.a) != 1) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
		return -1;
	}
	if (sscanf(lines[1], "Register B: %" PRIu64, &comp.b) != 1) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
		return -1;
	}
	if (sscanf(lines[2], "Register C: %" PRIu64, &comp.c) != 1) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
		return -1;
	}
	int parse = 0;
	sscanf(lines[4], "Program: %n", &parse);
	if (parse == 0) {
		aoc_throw(e, AOC_EX_DATA_ERR, AOC_MSG_INVALID_FILE);
		return -1;
	}
	size_t rom_buf_size = aoc_numbers_line_estimate_size(line_lengths[4]);
	comp.rom = assert_calloc(rom_buf_size, inst_t);
	comp.rom_size = aoc_numbers_line_parse(lines[4], ',', comp.rom, rom_buf_size);

	for (size_t i = 0; i < comp.rom_size; ++i)
		fprintf(stderr, "%" PRId32 "\n", comp.rom[i]);

	exec(&comp, e);
	if (*e)
		return -1;

	return comp.output;
}

int main(int argc, char * argv[]) {
	aoc_main_lines(argc, argv, 1, solve);
}
