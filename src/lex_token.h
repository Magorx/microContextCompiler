#ifndef LEX_TOKEN
#define LEX_TOKEN

#include "general/cpp/stringview.hpp"
#include "compiler_options.h"

//=============================================================================
// Token ======================================================================

enum TOKEN_TYPE {
	T_END    = 0,
	T_NUMBER = 1,
	T_ID     = 2,
	T_OP     = 3,
};

union TokenData {
	int         op;
	double      num;
	StringView *id;
};

struct Token {
// data =======================================================================
	int type;
	TokenData data;
	int line;
	int pos;
//=============================================================================

	// Token            (const Token&) = delete;
	// Token &operator= (const Token&) = delete;

	Token ();
	~Token();

	void ctor();
	void ctor(int type_, int line_, int pos_);
	void ctor(int type_, int op_, int line_, int pos_);
	void ctor(int type_, double num_, int line_, int pos_);
	void ctor(int type_, StringView *id_, int line_, int pos_);

	static Token *NEW();
	static Token *NEW(int type_, int line_, int pos_);
	static Token *NEW(int type_, int op_, int line_, int pos_);
	static Token *NEW(int type_, double num_, int line_, int pos_);
	static Token *NEW(int type_, StringView *id_, int line_, int pos_);

	void dtor(bool to_delete_id = false);

	static void DELETE(Token *classname, bool to_delete_id = false);

	int 		get_op() const;
	StringView *get_id() const;

	bool is_op		() const;
	bool is_op		(const int op) const;
	bool is_number	() const;
	bool is_id		() const;
	bool is_end		() const;
	void dump(FILE *file = stdout, bool bracked = true) const;
};

#endif // LEX_TOKEN