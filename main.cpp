#include <cstdlib>
#include <cstdio>

#include "general/c/debug.h"
#include "general/warnings.h"

#include "compiler.h"
#include "reg_stack.h"
#include "byte_buffer.h"

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
	comp.dtor();

	// printf(".doned.\n");
	return 0;
}
