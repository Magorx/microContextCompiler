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

#include "machine_codes/defines.h"
#include "reg_manager.h"

class RegManager;

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
	
//=============================================================================
	void fprintf_asgn_additional_operation(FILE *file, const int op);
	void cpl_operation(const CodeNode *node, FILE *file);

	void cpl_expr 		(const CodeNode *node, FILE *file, const bool to_pop = false);
	void cpl_func_call	(const CodeNode *node, FILE *file);
	void cpl_default_arg	(const CodeNode *arg, const CodeNode *prot, FILE *file);
	void cpl_context_arg	(const CodeNode *arg, const CodeNode *prot, FILE *file);
	void cpl_expr_arg		(const CodeNode *arg, const CodeNode *prot, FILE *file);
	void cpl_arr_call		(const CodeNode *node, FILE *file);
	bool cpl_push			(const CodeNode *node, FILE *file);
	bool cpl_value 		(const CodeNode *node, FILE *file);
	void cpl_id			(const CodeNode *node, FILE *file);
	void compile 		(const CodeNode *node, FILE *file);
	bool cpl_lvalue		(const CodeNode *node, FILE *file, 
						 const bool for_asgn_dup = false, 
						 const bool to_push = false, 
						 const bool initialization = false);


public:
	RegManager *regman;
	ByteBuffer cmd;

	void cpl_math_op       (const int reg_dst, const int reg_src, const char op);

	void cpl_cmp_reg_reg	 (const int reg_src, const int reg_dst);
	void cpl_test_reg_reg	 (const int reg_src, const int reg_dst);

	void cpl_mov_reg_imm32 (const int reg_dst, const double val);
	void cpl_mov_reg_imm64 (const int reg_dst, const double val);
	void cpl_mov_reg_reg   (const int reg_dst, const int reg_src);

	void cpl_mov_mem_reg	 (					 const int reg_dst, const int offset, const int reg_src);
	void cpl_mov_reg_mem	 (const int reg_dst, const int reg_src, const int offset);

	void cpl_xchg_rax_reg	 (const int reg_dst);

	void cpl_push_reg		 (const int reg_src);
	void cpl_pop_reg  	 (const int reg_dst);

	void cpl_je_rel32  (const int offset);
	void cpl_jne_rel32 (const int offset);
	void cpl_jl_rel32  (const int offset);
	void cpl_jle_rel32 (const int offset);
	void cpl_jg_rel32  (const int offset);
	void cpl_jge_rel32 (const int offset);
	void cpl_jmp_rel32 (const int offset);

	void cpl_call_rel32(const int offset);
	void cpl_ret();

	void cpl_breakpoint();
	void cpl_syscall();
	void cpl_nop();

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
