#include "code_node.h"

CodeNode::CodeNode():
type(0),
data(),
L(nullptr),
R(nullptr),
line(0),
pos(0)
{}

CodeNode::~CodeNode() {}

void CodeNode::ctor() {
	type     = NONE;
	data.val = 0;
	L        = nullptr;
	R        = nullptr;

	line = 0;
	pos  = 0;
}

CodeNode *CodeNode::NEW() {
	CodeNode *cake = (CodeNode*) calloc(1, sizeof(CodeNode));
	if (!cake) {
		return nullptr;
	}

	cake->ctor();
	return cake;
}

void CodeNode::ctor(const char type_, const int op_or_var, CodeNode *L_, CodeNode *R_, const int line_, const int pos_) {
	type = type_;
	if (type == OPERATION) {
		data.op = op_or_var;
	} else {
		data.var = op_or_var;
	}
	L = L_;
	R = R_;
	line = line_;
	pos  = pos_;
}

CodeNode *CodeNode::NEW(const char type_, const int op_or_var, CodeNode *L_, CodeNode *R_, const int line_, const int pos_) {
	CodeNode *cake = (CodeNode*) calloc(1, sizeof(CodeNode));
	if (!cake) {
		return nullptr;
	}

	cake->ctor(type_, op_or_var, L_, R_, line_, pos_);
	return cake;
}

void CodeNode::ctor(const char type_, const double val_, CodeNode *L_, CodeNode *R_, const int line_, const int pos_) {
	type     = type_;
	data.val = val_;
	L        = L_;
	R        = R_;
	line     = line_;
	pos      = pos_;
}

CodeNode *CodeNode::NEW(const char type_, const double val_, CodeNode *L_, CodeNode *R_, const int line_, const int pos_) {
	CodeNode *cake = (CodeNode*) calloc(1, sizeof(CodeNode));
	if (!cake) {
		return nullptr;
	}

	cake->ctor(type_, val_, L_, R_, line_, pos_);
	return cake;
}

void CodeNode::ctor(const char type_, StringView *id_, CodeNode *L_, CodeNode *R_, const int line_, const int pos_) {
	type    = type_;
	data.id = id_;
	L       = L_;
	R       = R_;
	line    = line_;
	pos     = pos_;
}

CodeNode *CodeNode::NEW(const char type_, StringView *id_, CodeNode *L_, CodeNode *R_, const int line_, const int pos_) {
	CodeNode *cake = (CodeNode*) calloc(1, sizeof(CodeNode));
	if (!cake) {
		return nullptr;
	}

	cake->ctor(type_, id_, L_, R_, line_, pos_);
	return cake;
}

void CodeNode::dtor() {
	type     = NONE;
	data.val = 0;
	L        = nullptr;
	R        = nullptr;
	line     = 0;
	pos      = 0;
}

void CodeNode::DELETE(CodeNode *node, bool recursive, bool to_delete_id) {
	if (!node) {
		return;
	}

	if (recursive) {
		if (node->L) DELETE(node->L, recursive, to_delete_id);
		if (node->R) DELETE(node->R, recursive, to_delete_id);
	}

	if (to_delete_id && node->type == ID) {
		StringView::DELETE(node->data.id);
	}

	node->dtor();
	free(node);
}

//=============================================================================
// Setters & Getters ==========================================================
//=============================================================================

CodeNode *CodeNode::get_L() const {
	return L;
}

CodeNode *CodeNode::get_R() const {
	return R;
}

void CodeNode::set_L(CodeNode *L_) {
	L = L_;
}

void CodeNode::set_R(CodeNode *R_) {
	R = R_;
}

void CodeNode::set_LR(CodeNode *L_, CodeNode *R_) {
	L = L_;
	R = R_;
}

void CodeNode::set_type(const char type_) {
	type = type_;
}

void CodeNode::set_op(const int op_) {
	set_type(OPERATION);
	data.op = op_;
}

void CodeNode::set_var(const int var_) {
	set_type(VARIABLE);
	data.var = var_;
}

void CodeNode::set_val(const double val_) {
	set_type(VALUE);
	data.val = val_;
}

void CodeNode::set_id(StringView *id_) {
	set_type(ID);
	data.id = id_;
}

char CodeNode::get_type() const {
	return type;
}

int CodeNode::get_op() const {
	return data.op;
}

int CodeNode::get_var() const {
	return data.var;
}

double CodeNode::get_val() const {
	return data.val;
}

StringView *CodeNode::get_id() const {
	return data.id;
}

int CodeNode::get_var_from_id() const {
	return (*data.id)[0];
}

bool CodeNode::is_op() const {
	return type == OPERATION;
}

bool CodeNode::is_op(const int op) const {
	return type == OPERATION && data.op == op;
}

bool CodeNode::is_var() const {
	return type == VARIABLE;
}

bool CodeNode::is_val() const {
	return type == VALUE;
}

bool CodeNode::is_id() const {
	return type == ID;
}

// ============================================================================

void CodeNode::space_dump(FILE *file) const {
	if (is_op(';')) {
		//fprintf(file, ";");
		return;
	}

	if (L) {
		fprintf(file, "(");
		L->space_dump(file);
		fprintf(file, ")");
	}

	//fprintf(file, "(");
	if (is_op()) {
		if (is_printable_op(data.op)) {
			fputc(data.op, file);
		} else {
			fprintf(file, "[op_%d]", data.op);
		}
	} else if (is_var()) {
		fprintf(file, "{%d}", data.var);
	} else if (is_val()) {
		fprintf(file, "%lg", data.val);
	} else if (is_id()) {
		data.id->print(file);
	} else {
		fprintf(file, ">ERR<");
	}
	//fprintf(file, ")");

	if (R) {
		fprintf(file, "(");
		R->space_dump(file);
		fprintf(file, ")");
	}
}

void CodeNode::full_dump(FILE *file) const {
	if (L) {
		fprintf(file, "(");
		L->full_dump(file);
		fprintf(file, ")");
	}

	//fprintf(file, "(");
	if (is_op()) {
		if (is_printable_op(data.op)) {
			fputc(data.op, file);
		} else {
			fprintf(file, "[op_%d]", data.op);
		}
	} else if (is_var()) {
		fprintf(file, "{%d}", data.var);
	} else if (is_val()) {
		fprintf(file, "%lg", data.val);
	} else if (is_id()) {
		data.id->print(file);
	} else {
		fprintf(file, "ERR");
	}
	//fprintf(file, ")");

	if (R) {
		fprintf(file, "(");
		R->full_dump(file);
		fprintf(file, ")");
	}
}

void CodeNode::gv_dump(FILE *file, const char *name) const {
	bool call_show = false;
	if (!file) {
		call_show = true;
		file = fopen(name, "w");
		fprintf(file, "digraph list {rankdir=\"UD\";\n");
	}

	fprintf(file, "\"node_%p\" [label=\"", this);

	if (type == VALUE) {
		fprintf(file, "%lg\" shape=circle style=filled fillcolor=\"#FFFFCC\"", get_val());
	} else if (type == OPERATION) {
		const char *opname = OPERATION_NAME(get_op());
		if (opname) {
			fprintf(file, "%s\" shape=invhouse style=filled fillcolor=\"#CCCCFF\"", opname);
		} else {
			if (is_printable_op(get_op()))
				fprintf(file, "%c\" shape=invhouse style=filled fillcolor=\"#FFCCCC\" fontsize=16", get_op());
			else
				fprintf(file, "[OP_%d]\" shape=invhouse style=filled fillcolor=\"#CCCCCC\"", get_op());
		}
	} else if (type == ID) {
		get_id()->print(file);
		fprintf(file, "\" shape=circle style=filled fillcolor=\"#CCFFFF\"");
	}

	fprintf(file, "]\n");

	if (L) {
		fprintf(file, "\"node_%p\" -> \"node_%p\"\n", this, L);
		L->gv_dump(file);
	}

	if (R) {
		fprintf(file, "\"node_%p\" -> \"node_%p\"\n", this, R);
		R->gv_dump(file);
	}

	if (call_show) {
		fprintf(file, "}\n");
		fclose(file);
		char generate_picture_command[100];
		sprintf(generate_picture_command, "dot %s -T%s -o%s.svg", name, "svg", name);

		char view_picture_command[100];
		sprintf(view_picture_command, "eog %s.%s", name, "svg");
		
		system(generate_picture_command);
		system(view_picture_command);
	}
}
