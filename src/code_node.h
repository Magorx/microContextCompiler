#ifndef CODENODE_H
#define CODENODE_H

#include "general/cpp/stringview.hpp"
#include "general/constants.h"
#include "compiler_options.h"

#include <cstdio>

//=============================================================================
// CodeNode ===================================================================

enum CODE_NODE_TYPE {
	NONE        = 0,
	OPERATION   = 1,
	VARIABLE    = 2,
	VALUE       = 3,
	ID          = 4,
};

union CodeNodeData {
	int op;
	int var;
	double val;
	StringView *id;
};

struct CodeNode {
// data =======================================================================
	char type;
	CodeNodeData data;
	CodeNode *L;
	CodeNode *R;
//=============================================================================

public:
	int line;
	int pos;
	int ershov_number;
	CodeNode            (const CodeNode&) = delete;
	CodeNode &operator= (const CodeNode&) = delete;

	CodeNode ();
	~CodeNode();

	void ctor();
	void ctor(const char type_, const int op_or_var, CodeNode *L_, CodeNode *R_, const int line_, const int pos_);
	void ctor(const char type_, const double val_,   CodeNode *L_, CodeNode *R_, const int line_, const int pos_);
	void ctor(const char type_, StringView *id_,     CodeNode *L_, CodeNode *R_, const int line_, const int pos_);

	static CodeNode *NEW();
	static CodeNode *NEW(const char type_, const int op_or_var, CodeNode *L_, CodeNode *R_, const int line_, const int pos_);
	static CodeNode *NEW(const char type_, const double val_,   CodeNode *L_, CodeNode *R_, const int line_, const int pos_);
	static CodeNode *NEW(const char type_, StringView *id_,     CodeNode *L_, CodeNode *R_, const int line_, const int pos_);

	void dtor();
	static void DELETE(CodeNode *node, bool recursive = false, bool to_delete_id = false);

//=============================================================================
// Setters & Getters ==========================================================
//=============================================================================

	CodeNode *get_L() const;
	CodeNode *get_R() const;

	void set_L(CodeNode *L_);
	void set_R(CodeNode *R_);
	void set_LR(CodeNode *L_, CodeNode *R_);

	void set_type(const char type_);
	void set_op(const int op_);
	void set_var(const int var_);
	void set_val(const double val_);
	void set_id(StringView *id_);

	char get_type  () const;
	int  get_op    () const;
	int  get_var   () const;
	double get_val () const;

	StringView *get_id  () const;
	int get_var_from_id () const;

	bool is_op  () const;
	bool is_var () const;
	bool is_val () const;
	bool is_id  () const;

	bool is_op(const int op) const;

	void space_dump(FILE *file = stdout) const;
	void full_dump (FILE *file = stdout) const;

	void gv_dump(FILE *file = nullptr, const char *name = (const char*) "code_tree") const;

	static int get_ershov_number(const CodeNode *node);
};

#endif // CODENODE_H