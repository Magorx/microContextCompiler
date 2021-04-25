#include <cstdlib>
#include <cstdio>

#include "general/c/debug.h"
#include "general/warnings.h"

#include "compiler.h"
#include "reg_stack.h"
#include "byte_buffer.h"

int main(const int argc, const char **argv) {
	// const char *input_file  = "prog.ctx";
	// const char *output_file = "out.kc";
	// int verbosity = 0;
	
	// if (argc > 1 && strcmp(argv[1], ".")) {
	// 	input_file = argv[1];
	// }
	
	// if (argc > 2 && strcmp(argv[2], ".")) {
	// 	output_file = argv[2];
	// }

	// if (argc > 3 && !strcmp(argv[3], "-v")) {
	// 	verbosity = 1;
	// }

	// File file = {};
	// file.ctor(input_file);
	// if (!file.data) {
	// 	ANNOUNCE("ERR", "kncc", "can't find input file [%s]", input_file);
	// 	return -1;
	// }

	Compiler comp = {};
	comp.ctor();

	comp.C_mov_reg_imm64(REG_RAX, 15);
	comp.C_math_op(REG_RBX, REG_RBX, '^');
	comp.C_mov_reg_imm32(REG_RBX, 1);
	comp.C_math_op(REG_RAX, REG_RBX, '+');

	comp.C_mov_mem_reg(REG_RSP_DISPL(9), REG_RAX);
	comp.C_mov_reg_mem(REG_RAX, REG_RBP_DISPL(15));

	comp.C_push_reg(REG_RAX);
	comp.C_push_reg(REG_R8);

	comp.C_pop_reg(REG_RSP);
	comp.C_pop_reg(REG_R15);

	comp.C_test_reg_reg(REG_RAX, REG_R9);
	comp.C_cmp_reg_reg(REG_R14, REG_RBP);

	comp.C_xchg_rax_reg(REG_RBX);

	comp.C_je_rel32(0);
	comp.C_jne_rel32(0);
	comp.C_jl_rel32(0);
	comp.C_jle_rel32(0);
	comp.C_jg_rel32(0);
	comp.C_jge_rel32(0);
	comp.C_jmp_rel32(0);
	comp.C_call_rel32(0);

	comp.C_ret();
	comp.C_breakpoint();
	comp.C_syscall();

	comp.hexdump_cmd();

	// CodeNode *prog = comp.read_to_nodes(&file);

	// if (!prog) {
	// 	ANNOUNCE("ERR", "kncc", "can't parse input file [%s]", input_file);
	// 	file.dtor();
	// 	comp.dtor();
	// 	return -1;
	// }

	// if (verbosity) prog->gv_dump();

	// if (!comp.compile(prog, output_file)) {
	// 	ANNOUNCE("ERR", "kncc", "can't compile input file [%s]", input_file);
	// 	CodeNode::DELETE(prog, true, true);
	// 	file.dtor();
	// 	comp.dtor();
	// 	return -1;
	// }

	// CodeNode::DELETE(prog, true, true);
	// file.dtor();
	// comp.dtor();

	// printf(".doned.\n");
	return 0;
}
