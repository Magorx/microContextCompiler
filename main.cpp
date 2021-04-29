#include <cstdlib>
#include <cstdio>

#include "general/c/debug.h"
#include "general/warnings.h"

#include "compiler.h"
#include "reg_stack.h"
#include "byte_buffer.h"

#include "linker.h"

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

void test();

int main(const int argc, const char **argv) {
	const char *input_file  = "prog.ctx";
	const char *output_file = "out.kc";
	int verbosity = 0;
	
	if (argc > 1 && strcmp(argv[1], ".")) {
		input_file = argv[1];
	}
	
	if (argc > 2 && strcmp(argv[2], ".")) {
		output_file = argv[2];
	}

	if (argc > 3 && !strcmp(argv[3], "-v")) {
		verbosity = 1;
	}

	File file = {};
	file.ctor(input_file);
	if (!file.data) {
		ANNOUNCE("ERR", "kncc", "can't find input file [%s]", input_file);
		return -1;
	}

	Compiler comp = {};
	comp.ctor();

	CodeNode *prog = comp.read_to_nodes(&file);

	if (!prog) {
		ANNOUNCE("ERR", "kncc", "can't parse input file [%s]", input_file);
		file.dtor();
		comp.dtor();
		return -1;
	}

	if (verbosity) prog->gv_dump();

	if (!comp.compile(prog, output_file)) {
		ANNOUNCE("ERR", "kncc", "can't compile input file [%s]", input_file);
		CodeNode::DELETE(prog, true, true);
		file.dtor();
		comp.dtor();
		return -1;
	}

	CodeNode::DELETE(prog, true, true);
	file.dtor();

	MicroLinker linker;
	linker.ctor();

	MicroObj obj1;
	obj1.ctor();

	comp.cpl_jmp_rel32(0);

	obj1.request_fixup({"write_result", (int) comp.cmd.get_size() - 4, fxp_RELATIVE});
	obj1.add_fixup    ({"prog_end",     (int) comp.cmd.get_size() - 4, fxp_RELATIVE});
	obj1.add_fixup    ({"GLOB_VAR", 0, fxp_ABSOLUTE, 8});

	comp.cpl_mov_reg_imm64(REG_RAX, 60);
	comp.cpl_math_op(REG_RDI, REG_RDI, '^');
	comp.cpl_syscall();

	Compiler comp2;
	comp2.ctor();
	MicroObj obj2;
	obj2.ctor();

	obj2.add_fixup({"write_result", (int) comp2.cmd.get_size() - 4, fxp_RELATIVE});

	comp2.cpl_mov_reg_imm64(REG_RCX, IntToQWord(0, 0, 0, '\n').i);
	comp2.cpl_math_op(REG_RCX, REG_RAX, '+');
	comp2.cpl_push_reg(REG_RCX);
	comp2.cpl_mov_reg_reg(REG_RSI, REG_RSP);

	comp2.cpl_mov_reg_imm64(REG_RAX, 1);
	comp2.cpl_mov_reg_imm64(REG_RDI, 1);
	comp2.cpl_mov_reg_imm64(REG_RDX, 4);
	comp2.cpl_syscall();

	comp2.cpl_jmp_rel32(0);

	obj2.request_fixup({"prog_end", (int) comp2.cmd.get_size() - 4, fxp_RELATIVE});

	Vector<MicroObj*> objs = {};
	objs.ctor();

	obj1.set_prog((byte*) comp.cmd.get_data(), comp.cmd.get_size());
	obj2.set_prog((byte*) comp2.cmd.get_data(), comp2.cmd.get_size());

	objs.push_back(&obj2);
	objs.push_back(&obj1);

	ByteBuffer result_cmd;

	linker.link_objectives(objs, 1, "elf", &result_cmd);
	result_cmd.hexdump();

	comp.dtor();

	// printf(".doned.\n");
	return 0;
}

void test() {
	Compiler comp = {};
	comp.ctor();

	comp.cpl_nop();
	comp.cpl_nop();
	comp.cpl_nop();

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

	build_elf((const char*) comp.cmd.get_data(), 0, comp.cmd.get_size(), "elf", 0);

	comp.hexdump_cmd();
}

/*
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
*/