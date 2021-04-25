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
	// RegStack regstack;
	ByteBuffer cmd;
//=============================================================================
	void fprintf_asgn_additional_operation(FILE *file, const int op);
	void C_operation(const CodeNode *node, FILE *file);

	void C_expr 		(const CodeNode *node, FILE *file, const bool to_pop = false);
	void C_func_call	(const CodeNode *node, FILE *file);
	void C_default_arg	(const CodeNode *arg, const CodeNode *prot, FILE *file);
	void C_context_arg	(const CodeNode *arg, const CodeNode *prot, FILE *file);
	void C_expr_arg		(const CodeNode *arg, const CodeNode *prot, FILE *file);
	void C_arr_call		(const CodeNode *node, FILE *file);
	bool C_push			(const CodeNode *node, FILE *file);
	bool C_value 		(const CodeNode *node, FILE *file);
	void C_id			(const CodeNode *node, FILE *file);
	void compile 		(const CodeNode *node, FILE *file);
	bool C_lvalue		(const CodeNode *node, FILE *file, 
						 const bool for_asgn_dup = false, 
						 const bool to_push = false, 
						 const bool initialization = false);


public:
	void C_math_op       (const int reg_dst, const int reg_src, const char op);

	void C_cmp_reg_reg	 (const int reg_src, const int reg_dst);
	void C_test_reg_reg	 (const int reg_src, const int reg_dst);

	void C_mov_reg_imm32 (const int reg_dst, const double val);
	void C_mov_reg_imm64 (const int reg_dst, const double val);
	void C_mov_reg_reg   (const int reg_dst, const int reg_src);

	void C_mov_mem_reg	 (					 const int reg_dst, const int offset, const int reg_src);
	void C_mov_reg_mem	 (const int reg_dst, const int reg_src, const int offset);

	void C_xchg_rax_reg	 (const int reg_dst);

	void C_push_reg		 (const int reg_src);
	void C_pop_reg  	 (const int reg_dst);

	void C_je_rel32  (const int offset);
	void C_jne_rel32 (const int offset);
	void C_jl_rel32  (const int offset);
	void C_jle_rel32 (const int offset);
	void C_jg_rel32  (const int offset);
	void C_jge_rel32 (const int offset);
	void C_jmp_rel32 (const int offset);

	void C_call_rel32(const int offset);
	void C_ret();

	void C_breakpoint();
	void C_syscall();

	void hexdump_cmd() const;

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
