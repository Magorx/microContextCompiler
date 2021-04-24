#ifndef COMPILER
#define COMPILER

#include "general/c/announcement.h"
#include "general/cpp/file.hpp"
#include "general/cpp/vector.hpp"

#include <cassert>

#include "compiler_options.h"

#include "lexical_parser.h"
#include "recursive_parser.h"
#include "id_table.h"
#include "byte_buffer.h"
#include "reg_stack.h"

//=============================================================================
// Compiler ===================================================================

class Compiler {
private:
// data =======================================================================
	char *prog_text;
	RecursiveParser rec_parser;
	LexicalParser   lex_parser;
	
	IdTable 		id_table;
	Vector<Loop> 	cycles_end_stack;

	int if_cnt;
	int while_cnt;
	int for_cnt;
//=============================================================================
	RegStack regstack;
	ByteBuffer bbuf;
//=============================================================================
	void fprintf_asgn_additional_operation(FILE *file, const int op);
	void compile_operation(const CodeNode *node, FILE *file);

	void compile_expr 		(const CodeNode *node, FILE *file, const bool to_pop = false);
	void compile_func_call	(const CodeNode *node, FILE *file);
	void compile_default_arg(const CodeNode *arg, const CodeNode *prot, FILE *file);
	void compile_context_arg(const CodeNode *arg, const CodeNode *prot, FILE *file);
	void compile_expr_arg	(const CodeNode *arg, const CodeNode *prot, FILE *file);
	void compile_arr_call	(const CodeNode *node, FILE *file);
	bool compile_push		(const CodeNode *node, FILE *file);
	bool compile_value 		(const CodeNode *node, FILE *file);
	void compile_id			(const CodeNode *node, FILE *file);
	void compile 			(const CodeNode *node, FILE *file);
	bool compile_lvalue		(const CodeNode *node, FILE *file, 
							 const bool for_asgn_dup = false, 
							 const bool to_push = false, 
							 const bool initialization = false);


public:
	Compiler            (const Compiler&) = delete;
	Compiler &operator= (const Compiler&) = delete;

	Compiler ();
	~Compiler();

	void ctor();
	static Compiler *NEW();

	void dtor();
	static void DELETE(Compiler *compiler);

//=============================================================================

	CodeNode *read_to_nodes(const File *file);

	bool compile(const CodeNode *prog, const char *filename);

};

#endif // COMPILER
