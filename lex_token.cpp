#include "lex_token.h"

 
Token::Token():
type(0),
data(),
line(0),
pos(0)
{}

Token::~Token() {}

void Token::ctor() {
	type = 0;
	data.num = 0;
	line = 0;
	pos = 0;
}

void Token::ctor(int type_, int line_, int pos_) {
	type = type_;
	data.num = 0;
	line = line_;
	pos = pos_;
}

void Token::ctor(int type_, int op_, int line_, int pos_) {
	type = type_;
	data.op = op_;
	line = line_;
	pos = pos_;
}

void Token::ctor(int type_, double num_, int line_, int pos_) {
	type = type_;
	data.num = num_;
	line = line_;
	pos = pos_;
}

void Token::ctor(int type_, StringView *id_, int line_, int pos_) {
	type = type_;
	data.id = id_;
	line = line_;
	pos = pos_;
}

Token *Token::NEW() {
	Token *cake = (Token*) calloc(1, sizeof(Token));
	if (!cake) {
		return nullptr;
	}

	cake->ctor();
	return cake;
}

Token *Token::NEW(int type_, int line_, int pos_) {
	Token *cake = (Token*) calloc(1, sizeof(Token));
	if (!cake) {
		return nullptr;
	}

	cake->ctor(type_, line_, pos_);
	return cake;
}

Token *Token::NEW(int type_, int op_, int line_, int pos_) {
	Token *cake = (Token*) calloc(1, sizeof(Token));
	if (!cake) {
		return nullptr;
	}

	cake->ctor(type_, op_, line_, pos_);
	return cake;
}

Token *Token::NEW(int type_, double num_, int line_, int pos_) {
	Token *cake = (Token*) calloc(1, sizeof(Token));
	if (!cake) {
		return nullptr;
	}

	cake->ctor(type_, num_, line_, pos_);
	return cake;
}

Token *Token::NEW(int type_, StringView *id_, int line_, int pos_) {
	Token *cake = (Token*) calloc(1, sizeof(Token));
	if (!cake) {
		return nullptr;
	}

	cake->ctor(type_, id_, line_, pos_);
	return cake;
}

void Token::dtor(bool to_delete_id) {
	if (type == T_ID) {
		if (to_delete_id) {
			StringView::DELETE(data.id);
		}
	}
}

void Token::DELETE(Token *classname, bool to_delete_id) {
	if (!classname) {
		return;
	}

	classname->dtor(to_delete_id);
	free(classname);
}

int Token::get_op() const {
	return data.op;
}

StringView *Token::get_id() const {
	return data.id;
}

bool Token::is_op() const {
	return type == T_OP;
}

bool Token::is_op(const int op) const {
	return type == T_OP && data.op == op;
}

bool Token::is_number() const {
	return type == T_NUMBER;
}

bool Token::is_id() const {
	return type == T_ID;
}

bool Token::is_end() const {
	return type == T_END;
}

void Token::dump(FILE *file, bool bracked) const {
	if (bracked) {
		fprintf(file, "[");
	}
	if (type == T_END) {
		fprintf(file, "0[END]0");
	} else if (type == T_NUMBER) {
		fprintf(file, "%lg", data.num);
	} else if (type == T_OP) {
		if (is_printable_op(data.op)) {
			fputc(data.op, file);
		} else {
			fprintf(file, "[op_%d]", data.op);
		}
	} else {
		data.id->print();
	}
	if (bracked) {
		fprintf(file, "]");
	}
}
