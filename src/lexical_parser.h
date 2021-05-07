#ifndef LEXICAL_PARSER
#define LEXICAL_PARSER

#include "general/cpp/stringview.hpp"
#include "general/cpp/vector.hpp"

#include <cctype>
#include <cmath>

#include "compiler_options.h"
#include "lex_token.h"

//=============================================================================
// LexicalParser ==============================================================

class LexicalParser {
private:
// data =======================================================================
	const char *cur_expr;
	const char *cur_line;
	const char *cur;
	Vector<Token> *tokens;
	char skip_mode;
	int line;
//=============================================================================

	bool is_id_char(const char c);
	bool is_sign   (const char c);

	bool one_available  ();
	bool two_available  ();
	bool three_available();

	void collect_number	();
	void collect_id		();

	bool try_collect_long_op();

	void parse();

public:
	LexicalParser            (const LexicalParser&) = delete;
	LexicalParser &operator= (const LexicalParser&) = delete;

	LexicalParser ();
	~LexicalParser();

	void ctor();
	static LexicalParser *NEW();

	void dtor();
	static void DELETE(LexicalParser *parser);

//=============================================================================

	Vector<Token> *parse(const char *expression);
};

#endif // LEXICAL_PARSER