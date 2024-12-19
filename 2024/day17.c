#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <inttypes.h>
#include <assert.h>

#include "aoc.h"

/**
 * I spent so long debugging part 1..
 * Not realizing that you ACTUALLY have to submit the answer as a list,
 * with commas.
 * I thought I was clever for joining the output values into a integer value.
 */
// #define DEBUG 1

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

	uint64_t output_decimals;
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
	if (comp->output_decimals * 10 < comp->output_decimals) {
		aoc_throw_fail(e, "Too much output");
		return;
	}
	comp->output_decimals *= 10;
	comp->output_decimals += value;
#ifdef DEBUG
	fprintf(stderr, "%" PRId32 ",", value);
#endif
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

static int64_t exec(computer_t * comp, aoc_ex_t * e) {
	comp->pc = 0;
	comp->output_decimals = 0;
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
			return -1;
	} while (has_next_step);
#ifdef DEBUG
	fputc('\n', stderr);
#endif
	return comp->output_decimals;
}


static bool b_overwritten_by_a(inst_t inst, inst_t operand) {
	if ((inst == BST || inst == BDV) && operand <= 4)
		return true;
	return false;
}

static bool c_remains_const(inst_t inst, inst_t operand) {
	return inst != CDV;
}

static bool c_overwritten_by_a(inst_t inst, inst_t operand) {
	if ((inst == CDV) && operand <= 4)
		return true;
	return false;
}

static bool c_overwritten_by_a_or_b(inst_t inst, inst_t operand) {
	if ((inst == CDV) && operand <= 5)
		return true;
	return false;
}

static bool cannot_make_quine_then_throw(aoc_ex_t * e, computer_t * comp) {
	if (comp->rom_size < 4) {
		aoc_throw_fail(e, "cannot prove that program is not depenant on B and C");
		return true;
	}
	bool b_overwritten = false;
	bool c_overwritten = false;
	size_t found_adv = 0;
	size_t found_out = 0;
	for (size_t i = 0; i < comp->rom_size - 2; i += 2) {
		inst_t inst = comp->rom[i];
		inst_t operand = comp->rom[i + 1];
		if (inst == ADV) {
			if (operand != 3) {
				aoc_throw_fail(e, "cannot analyze program with ADV whose operand != 3");
				return true;
			}
			++found_adv;
		}
		if (b_overwritten_by_a(inst, operand))
			b_overwritten = true;
		if (c_overwritten_by_a(inst, operand) || (b_overwritten && c_overwritten_by_a_or_b(inst, operand)))
			c_overwritten = true;

		if (!c_overwritten && !c_remains_const(inst, operand)) {
			aoc_throw_fail(e, "cannot analyze program: C is not immediately overwritten by A or B");
			return true;
		}

		if (inst == OUT) {
			if (!b_overwritten || !c_overwritten) {
				aoc_throw_fail(e, "cannot analyze program: B and C are not overwritten before printing");
				return true;
			}
			++found_out;
		}
		if (inst == JNZ) {
			aoc_throw_fail(e, "cannot analyze program with JNZ in the middle");
			return true;
		}
	}

	if (found_adv != 1 || found_out != 1) {
		aoc_throw_fail(e, "cannot analyze program that doesn't have exactly one ADV and OUT instruction");
		return true;
	}
	if (comp->rom[comp->rom_size - 2] != JNZ || comp->rom[comp->rom_size - 1] != 0) {
		aoc_throw_fail(e, "cannot analyze program that doesn't end with JNZ 0");
		return true;
	}
	return false;
}

// static int64_t make_quine(computer_t * comp, aoc_ex_t * e) {
// 	int64_t a = 0;
// 	for (size_t i = comp->rom_size; i > 0; --i) {
// 		inst_t word = comp->rom[i - 1];
//
// 		int64_t required_decimals = 0;
// 		for (size_t i2 = i - 1; i2 < comp->rom_size; ++i2)
// 			required_decimals = (required_decimals * 10) + comp->rom[i2];
// 		fprintf(stderr, "REQUIRED DECIMALS: %"PRId64" (word: %"PRId32")\n", required_decimals, word);
//
// 		int64_t try_a;
// 		for (try_a = 0; try_a < 8; ++try_a) {
// 			comp->a = (a << 3) | try_a;
// 			comp->b = 0;
// 			comp->c = 0;
// 			int64_t result_decimals = exec(comp, e);
// 			fprintf(stderr, "(%" PRIo64 " produces %" PRId64 ")\n", (a << 3) | try_a, result_decimals);
// 			if (*e)
// 				return -1;
// 			if (result_decimals == required_decimals) {
// 				fprintf(stderr, "(%" PRIo64 " produces %" PRId64 ")!!!\n", (a << 3) | try_a, required_decimals);
// 				goto continue_next_word;
// 			}
// 		}
// 		aoc_throw_fail(e, "could not find digit");
// 		return -1;
// 	continue_next_word:
// 		a = (a << 3) | try_a;
// 	}
// 	return a;
// }

static int64_t make_quine(computer_t * comp, size_t inst_n, uint64_t a, aoc_ex_t * e) {
	inst_t word = comp->rom[inst_n];

	uint64_t required_decimals = 0;
	for (size_t i = inst_n; i < comp->rom_size; ++i)
		required_decimals = (required_decimals * 10) + comp->rom[i];
	fprintf(stderr, "REQUIRED DECIMALS: %"PRId64" (word: %"PRId32")\n", required_decimals, word);

	uint64_t try_a;
	for (try_a = 0; try_a < 8; ++try_a) {
		uint64_t next_a = (a << 3) | try_a;
		comp->a = next_a;
		int64_t result_decimals = exec(comp, e);
		fprintf(stderr, "(%"PRIo64" produces %"PRId64")\n", next_a, result_decimals);
		if (*e)
			return -1;
		if (result_decimals == required_decimals) {
			fprintf(stderr, "(%"PRIo64" produces %"PRId64")!!!\n", next_a, required_decimals);
			if (inst_n == 0)
				return next_a;
			int64_t min_result = make_quine(comp, inst_n - 1, next_a, e);
			if (min_result > 0)
				return min_result;
		}
	}
	return -1;
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

	int64_t result;
	if (part == 1) {
		result = exec(&comp, e);
	} else {
		if (cannot_make_quine_then_throw(e, &comp))
			return -1;
		result = make_quine(&comp, comp.rom_size - 1, 0, e);
	}
	if (*e)
		return -1;

	return result;
}

int main(int argc, char * argv[]) {
	aoc_main_lines(argc, argv, 2, solve);
}
