#ifndef RECURSIVE_PARSER
#define RECURSIVE_PARSER

#include "general/c/announcement.h"
#include "general/cpp/stringview.hpp"
#include "general/cpp/vector.hpp"

#include "general/c/debug.h"

#include <cmath>

#include "code_node.h"
#include "lex_token.h"

typedef CodeNode ParseNode;

enum PARSER_ERROR {
	prs_OK = 0,

	ERROR_SYNTAX = 100,
};

//=============================================================================
// RecursiveParser ============================================================

class RecursiveParser {
private:
// data =======================================================================
	Vector<Token> *expr;
	int            cur_index;
	Token         *cur;

	int            ERROR;
	const Token   *ERRPOS;
//=============================================================================
	bool is_id_char	(const char c);
	bool is_digit	(const char c);
	bool is_sign	(const char c);
	bool is_sign	(const Token *t);

	bool is_multiplicative(const char c);
	bool is_multiplicative(const Token *t);

	ParseNode *parse_ID();
	ParseNode *parse_NUMB();
	ParseNode *parse_UNIT();
	ParseNode *parse_FACT();
	ParseNode *parse_TERM();
	ParseNode *parse_DEF_VAR();
	ParseNode *parse_DEF_ARR();
	ParseNode *parse_NEW_VAR_DEF();
	ParseNode *parse_MATH_EXPR();
	ParseNode *parse_COND();
	ParseNode *parse_AND_EXPR();
	ParseNode *parse_LOGIC_EXPR();
	ParseNode *parse_EXPR();
	ParseNode *parse_IF();
	ParseNode *parse_WHILE();
	ParseNode *parse_FOR();
	ParseNode *parse_DELIMITED_STMT();
	ParseNode *parse_STATEMENT();
	ParseNode *parse_BLOCK_STATEMENT();
	ParseNode *parse_PROG();
	ParseNode *parse_G();
	ParseNode *parse_ELEM_FUNC();
	ParseNode *parse_ARG_DECL();
	ParseNode *parse_ARGLIST_DECL();
	ParseNode *parse_FUNC_DECL();
	ParseNode *parse_ARG_CALL();
	ParseNode *parse_ARGLIST_CALL();
	ParseNode *parse_FUNC_CALL();

public:
	RecursiveParser            (const RecursiveParser&) = delete;
	RecursiveParser &operator= (const RecursiveParser&) = delete;

	RecursiveParser();
	~RecursiveParser();
	void ctor();
	static RecursiveParser *NEW();
	void dtor();
	static void DELETE(RecursiveParser *classname);
	ParseNode *parse(Vector<Token> *expression);

};

#endif // RECURSIVE_PARSER