#include <cstdlib>
#include <cstdio>

#include "general/c/debug.h"
#include "general/warnings.h"

#include "compiler.h"
#include "reg_stack.h"
#include "byte_buffer.h"

#include "elf_builder.h"

union IntToQWord {
	int i;
	char c[4];

	IntToQWord(char c1, char c2, char c3, char c4) {
		c[0] = c1;
		c[1] = c2;
		c[2] = c3;
		c[3] = c4;
	}

	IntToQWord(const char *str) {
		c[0] = str[0];
		c[1] = str[1];
		c[2] = str[2];
		c[3] = str[3];
	}
};

int main() {//const int argc, const char **argv) {
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

	printf("elf header size: %lu\n", sizeof(ELF_Header));

	Compiler comp = {};
	comp.ctor();

	comp.cpl_nop();
	comp.cpl_nop();
	comp.cpl_nop();

	// comp.cpl_mov_reg_imm64(REG_RAX, 15);
	// comp.cpl_math_op(REG_RBX, REG_RBX, '^');
	// comp.cpl_mov_reg_imm32(REG_RBX, 1);
	// comp.cpl_math_op(REG_RAX, REG_RBX, '+');

	// comp.cpl_mov_mem_reg(REG_RSP_DISPL(9), REG_RAX);
	// comp.cpl_mov_reg_mem(REG_RAX, REG_RBP_DISPL(15));

	// comp.cpl_push_reg(REG_RAX);
	// comp.cpl_push_reg(REG_R8);

	// comp.cpl_pop_reg(REG_RSP);
	// comp.cpl_pop_reg(REG_R15);

	// comp.cpl_test_reg_reg(REG_RAX, REG_R9);
	// comp.cpl_cmp_reg_reg(REG_R14, REG_RBP);

	// comp.cpl_xchg_rax_reg(REG_RBX);

	comp.cpl_mov_reg_imm64(REG_RCX, IntToQWord("DIE\n").i);
	comp.cpl_push_reg(REG_RCX);
	comp.cpl_mov_reg_reg(REG_RSI, REG_RSP);

	comp.cpl_mov_reg_imm64(REG_RAX, 1);
	comp.cpl_mov_reg_imm64(REG_RDI, 1);
	comp.cpl_mov_reg_imm64(REG_RDX, 4);
	comp.cpl_syscall();

	comp.cpl_mov_reg_imm64(REG_RAX, 60);
	comp.cpl_mov_reg_imm64(REG_RDI, 0);
	comp.cpl_syscall();
	
	comp.cpl_nop();
	comp.cpl_nop();
	comp.cpl_nop();

	build_elf((const char*) comp.cmd.get_data(), comp.cmd.get_size(), "elf");

	// for (int i = 0; i < 25; ++i) {
	// 	int x;
	// 	char c;
	// 	scanf("%c", &c);
	// 	if (c == 'q') {
	// 		break;
	// 	}
	// 	if (c == '+') {
	// 		int reg = comp.regman->get_var_reg(i, REGMAN_VAR_LOCAL, "A");
	// 		int id = comp.regman->get_reg_id(reg);
	// 		printf("off[%d] -> reg[%d] -> id[%d]\n", i, reg, id);
	// 	} else {
	// 		scanf("%d", &x);
	// 		int reg = comp.regman->get_var_reg(x, REGMAN_VAR_LOCAL, "A");
	// 		int id = comp.regman->get_reg_id(reg);
	// 		printf("iff [%d] -> id[%d] -> reg[%d]\n", x, id, reg);
	// 	}
	// 	scanf("%c", &c);
	// }

	// for (int i = 0; i < 25; ++i) {
	// 	int reg = comp.regman->get_tmp_reg(i);
	// 	printf("reg: %d\n", reg);
	// }

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
