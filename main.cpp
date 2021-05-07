#include <cstdlib>
#include <cstdio>

#include "general/c/debug.h"
#include "general/warnings.h"

#include "compiler.h"
#include "reg_stack.h"
#include "byte_buffer.h"

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

int main(const int argc, const char **argv) {
	srand((unsigned) time(NULL));

	const char *input_file  = "prog.ctx";
	const char *output_file = "elf";
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

	comp.dtor();

	// printf(".doned.\n");
	return 0;
}
