#include "recursive_parser.h"

#define NEXT()  ++cur_index;       cur = &(*expr)[cur_index]
#define PREV()  --cur_index;       cur = &(*expr)[cur_index]
#define SETI(ind) cur_index = ind; cur = &(*expr)[cur_index]

#define RESET_POINT int ENTER_INDEX
#define RESET() SETI(ENTER_INDEX)

#define NEW_NODE(type, data, l, r) ParseNode::NEW(type, data, l, r, cur->line, cur->pos)

#define REQUIRE_OP(op)                                  \
	do {                                                \
		if (cur->type != T_OP || cur->data.op != op) {  \
			ERROR  = ERROR_SYNTAX;                      \
			ERRPOS = cur;                               \
			return nullptr;                             \
		} else {                                        \
			NEXT();                                     \
		}                                               \
	} while (0)

#define IF_PARSED(index, ret_name, code)          \
	ParseNode *ret_name = (code);                 \
	if (ERROR) {                                  \
		cur_index = index;                        \
		cur = &(*expr)[cur_index];                \
		SET_ERR(0, &(*expr)[0]);                  \
	} else

#define SET_ERR(errcode, errpos) do {ERROR = errcode; ERRPOS = errpos;} while (0)

bool RecursiveParser::is_id_char(const char c) {
	return ('a' <= c && c <= 'z') || ('A' <= c && c <= 'Z') || c == '_';
}

bool RecursiveParser::is_digit(const char c) {
	return ('0' <= c && c <= '9');
}

bool RecursiveParser::is_sign(const char c) {
	return c == '+' || c == '-';
}

bool RecursiveParser::is_sign(const Token *t) {
	return t->is_op('+') || t->is_op('-');
}

bool RecursiveParser::is_multiplicative(const char c) {
	return c == '*' || c == '/';
}

bool RecursiveParser::is_multiplicative(const Token *t) {
	return t->is_op('*') || t->is_op('/');
}

ParseNode *RecursiveParser::parse_ID() {
	if (!cur->is_id()) {
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	CodeNode *ret = NEW_NODE(ID, cur->data.id, nullptr, nullptr);
	NEXT();
	return ret;
}

ParseNode *RecursiveParser::parse_NUMB() {
	if (!cur->is_number()) {
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}
	CodeNode *ret = NEW_NODE(VALUE, cur->data.num, nullptr, nullptr);
	NEXT();
	return ret;
}

ParseNode *RecursiveParser::parse_UNIT() {
	IF_PARSED (cur_index, unit_id, parse_ID()) {
		if (cur->is_op('(')) {
			NEXT();
			IF_PARSED (cur_index, bracket_expr, parse_EXPR()) {
				if (cur->is_op(')')) {
					NEXT();
					unit_id->set_R(bracket_expr);
					return unit_id;
				}
			}

			NEXT();
			SET_ERR(ERROR_SYNTAX, cur);
			ParseNode::DELETE(unit_id);
			return nullptr;
		} else if (cur->is_op('[')) {
			ParseNode *func_call = NEW_NODE(OPERATION, OPCODE_FUNC_CALL, nullptr, unit_id);

			IF_PARSED (cur_index, args, parse_ARGLIST_CALL()) {
				func_call->L = args;
				return func_call;
			}

			ParseNode::DELETE(func_call, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		} else {
			return unit_id;
		}
	}

	if (cur->is_op('(')) {
		NEXT();
		IF_PARSED (cur_index, expr_in_brackets, parse_EXPR()) {
			if (cur->is_op(')')) {
				NEXT();
				return expr_in_brackets;
			} else {
				ParseNode::DELETE(expr_in_brackets);
			}
		}
		NEXT();
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	IF_PARSED (cur_index, elem_func, parse_ELEM_FUNC()) {
		return elem_func;
	}

	IF_PARSED (cur_index, number, parse_NUMB()) {
		return number;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_FACT() {
	if (is_sign(cur)) {
		int sign = cur->get_op();
		NEXT();
		IF_PARSED (cur_index, fact, parse_FACT()) {
			return NEW_NODE(OPERATION, sign, nullptr, fact);
		}
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	IF_PARSED (cur_index, unit, parse_UNIT()) {
		if (cur->is_op('^')) {
			NEXT();
			IF_PARSED (cur_index, fact, parse_FACT()) {
				return NEW_NODE(OPERATION, '^', unit, fact);
			}
			ParseNode::DELETE(unit);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		} else {
			return unit;
		}
	}
	
	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_TERM() {
	IF_PARSED (cur_index, fact, parse_FACT()) {
		while (is_multiplicative(cur)) {
			int op = cur->get_op();
			NEXT();

			IF_PARSED (cur_index, next_fact, parse_FACT()) {
				fact = NEW_NODE(OPERATION, op, fact, next_fact);
				continue;
			}

			ParseNode::DELETE(fact, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}

		return fact;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_DEF_VAR() {
	if (!cur->is_op(OPCODE_VAR)) {
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	NEXT();
	IF_PARSED (cur_index, id, parse_ID()) {
		ParseNode *var_definition = NEW_NODE(OPERATION, OPCODE_VAR_DEF, id, nullptr);
		if (cur->is_op('=')) {
			NEXT();

			IF_PARSED (cur_index, expr_node, parse_EXPR()) {
				var_definition->R = expr_node;
				return var_definition;
			}

			PREV();
			PREV();
			ParseNode::DELETE(var_definition, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		} else {
			return var_definition;
		}
	}

	PREV();
	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_DEF_ARR() {
	RESET_POINT = cur_index;

	if (!cur->is_op(OPCODE_VAR)) {
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	NEXT();
	IF_PARSED (cur_index, id, parse_ID()) {
		ParseNode *arr_info = NEW_NODE(OPERATION, OPCODE_ARR_INFO, nullptr, id);
		ParseNode *arr_def  = NEW_NODE(OPERATION, OPCODE_ARR_DEF, arr_info, nullptr);

		if (cur->is_op('[')) {
			NEXT();

			IF_PARSED (cur_index, numb, parse_NUMB()) {
				arr_info->L = numb;
				if (cur->is_op(']')) {
					NEXT();
					return arr_def;
				}
			}
		}

		RESET();
		ParseNode::DELETE(arr_def, true);
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	RESET();
	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_NEW_VAR_DEF() {
	IF_PARSED (cur_index, arr, parse_DEF_ARR()) {
		return arr;
	}

	IF_PARSED (cur_index, var, parse_DEF_VAR()) {
		return var;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_MATH_EXPR() {
	IF_PARSED (cur_index, term, parse_TERM()) {
		while (is_sign(cur)) {
			int op = cur->get_op();
			NEXT();

			IF_PARSED (cur_index, next_term, parse_TERM()) {
				term = NEW_NODE(OPERATION, op, term, next_term);
				continue;
			}

			ParseNode::DELETE(term, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}

		return term;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
} 

ParseNode *RecursiveParser::parse_COND() {
	IF_PARSED (cur_index, cur_expr, parse_MATH_EXPR()) {
		while (cur->is_op('>') || cur->is_op('<') ||
			   cur->is_op(OPCODE_LE) || cur->is_op(OPCODE_GE ) ||
			   cur->is_op(OPCODE_EQ) || cur->is_op(OPCODE_NEQ)) {
			int op = cur->get_op();
			NEXT();

			IF_PARSED (cur_index, next_expr, parse_MATH_EXPR()) {
				cur_expr = NEW_NODE(OPERATION, op, cur_expr, next_expr);
				continue;
			}

			ParseNode::DELETE(cur_expr, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}

		return cur_expr;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_AND_EXPR() {
	IF_PARSED (cur_index, cur_cond, parse_COND()) {
		while (cur->is_op(OPCODE_AND)) {
			int op = cur->get_op();
			NEXT();

			IF_PARSED (cur_index, next_cond, parse_COND()) {
				cur_cond = NEW_NODE(OPERATION, op, cur_cond, next_cond);
				continue;
			}

			ParseNode::DELETE(cur_cond, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}

		return cur_cond;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_LOGIC_EXPR() {
	IF_PARSED (cur_index, cur_and_node, parse_AND_EXPR()) {
		while (cur->is_op(OPCODE_OR)) {
			int op = cur->get_op();
			NEXT();

			IF_PARSED (cur_index, next_and_node, parse_AND_EXPR()) {
				cur_and_node = NEW_NODE(OPERATION, op, cur_and_node, next_and_node);
				continue;
			}

			ParseNode::DELETE(cur_and_node, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}

		return cur_and_node;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_EXPR() {
	if (cur->is_id()) {
		ParseNode *id = NEW_NODE(ID, cur->data.id, nullptr, nullptr);
		NEXT();

		if (cur->is_op('=') || cur->is_op(OPCODE_ASGN_ADD)
							|| cur->is_op(OPCODE_ASGN_SUB)
							|| cur->is_op(OPCODE_ASGN_MUL)
							|| cur->is_op(OPCODE_ASGN_DIV)
							|| cur->is_op(OPCODE_ASGN_POW)) {
			int op = cur->get_op();
			NEXT();

			IF_PARSED (cur_index, expr_node, parse_EXPR()) {
				return NEW_NODE(OPERATION, op, id, expr_node);
			}

			PREV();
		}

		PREV();
		ParseNode::DELETE(id);
	}

	IF_PARSED (cur_index, logic_expr_node, parse_LOGIC_EXPR()) {
		if (cur->is_op('=') || cur->is_op(OPCODE_ASGN_ADD)
							|| cur->is_op(OPCODE_ASGN_SUB)
							|| cur->is_op(OPCODE_ASGN_MUL)
							|| cur->is_op(OPCODE_ASGN_DIV)
							|| cur->is_op(OPCODE_ASGN_POW)) {
			int op = cur->get_op();
			NEXT();

			IF_PARSED (cur_index, expr_node, parse_EXPR()) {
				return NEW_NODE(OPERATION, op, logic_expr_node, expr_node);
			}

			PREV();
		}

		return logic_expr_node;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_IF() {
	RESET_POINT = cur_index;

	if (!cur->is_op(OPCODE_IF)) {
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}
	NEXT();

	if (!cur->is_op('(')) {
		RESET();
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}
	NEXT();

	IF_PARSED (cur_index, cond_block, parse_EXPR()) {
		if (!cur->is_op(')')) {
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}
		NEXT();

		IF_PARSED (cur_index, true_block, parse_BLOCK_STATEMENT()) {
			ParseNode *dep  = NEW_NODE(OPERATION, OPCODE_COND_DEPENDENT, nullptr,    true_block);
			ParseNode *cond = NEW_NODE(OPERATION, OPCODE_IF,             cond_block, dep       );
			if (!cur->is_op(':')) {
				return cond;
			}

			NEXT();
			IF_PARSED (cur_index, false_block, parse_BLOCK_STATEMENT()) {
				dep->L = false_block;
				return cond;
			}

			ParseNode::DELETE(cond, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}

		ParseNode::DELETE(cond_block, true);
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	RESET();
	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_WHILE() {
	RESET_POINT = cur_index;

	if (!cur->is_op(OPCODE_WHILE)) {
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}
	NEXT();

	if (!cur->is_op('(')) {
		RESET();
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}
	NEXT();

	IF_PARSED (cur_index, cond_block, parse_EXPR()) {
		if (!cur->is_op(')')) {
			ParseNode::DELETE(cond_block, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}
		NEXT();

		IF_PARSED (cur_index, body_block, parse_BLOCK_STATEMENT()) {
			ParseNode *whil  = NEW_NODE(OPERATION, OPCODE_WHILE, cond_block, body_block);
			return whil;
		}

		ParseNode::DELETE(cond_block, true);
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	RESET();
	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_FOR() {
	RESET_POINT = cur_index;

	if (!cur->is_op(OPCODE_FOR)) {
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}
	NEXT();

	if (!cur->is_op('(')) {
		RESET();
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}
	NEXT();

	IF_PARSED (cur_index, init_block, parse_DELIMITED_STMT()) {

		if (!cur->is_op('|')) {
			ParseNode::DELETE(init_block, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}
		NEXT();

		IF_PARSED (cur_index, cond_block, parse_DELIMITED_STMT()) {
			ParseNode *for_info  = NEW_NODE(OPERATION, OPCODE_FOR_INFO, init_block, cond_block);

			if (!cur->is_op('|')) {
				ParseNode::DELETE(for_info, true);
				SET_ERR(ERROR_SYNTAX, cur);
				return nullptr;
			}
			NEXT();

			IF_PARSED (cur_index, act_block, parse_DELIMITED_STMT()) {
				ParseNode *for_upper_info = NEW_NODE(OPERATION, OPCODE_FOR_INFO, for_info, act_block);

				if (!cur->is_op(')')) {
					ParseNode::DELETE(for_upper_info, true);
					SET_ERR(ERROR_SYNTAX, cur);
					return nullptr;
				}
				NEXT();

				IF_PARSED (cur_index, body_block, parse_BLOCK_STATEMENT()) {
					ParseNode *for_node = NEW_NODE(OPERATION, OPCODE_FOR, for_upper_info, body_block);
					return for_node;
				}

				ParseNode::DELETE(for_upper_info, true);
				SET_ERR(ERROR_SYNTAX, cur);
				return nullptr;
			}

			ParseNode::DELETE(for_info, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}

		ParseNode::DELETE(init_block, true);
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	RESET();
	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_DELIMITED_STMT() {
	if (cur->is_op(OPCODE_BREAK)) {
		NEXT();
		return NEW_NODE(OPERATION, OPCODE_BREAK, nullptr, nullptr);
	}

	if (cur->is_op(OPCODE_CONTINUE)) {
		NEXT();
		return NEW_NODE(OPERATION, OPCODE_CONTINUE, nullptr, nullptr);
	}

	IF_PARSED (cur_index, var_def, parse_NEW_VAR_DEF()) {
		return var_def;
	}

	IF_PARSED (cur_index, expr_node, parse_EXPR()) {
		return expr_node;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_STATEMENT() {
	IF_PARSED (cur_index, func_decl, parse_FUNC_DECL()) {
		return NEW_NODE(OPERATION, ';', func_decl, nullptr);
	}

	IF_PARSED (cur_index, if_node, parse_IF()) {
		return NEW_NODE(OPERATION, ';', if_node, nullptr);
	}

	IF_PARSED (cur_index, while_node, parse_WHILE()) {
		return NEW_NODE(OPERATION, ';', while_node, nullptr);
	}

	IF_PARSED (cur_index, for_node, parse_FOR()) {
		return NEW_NODE(OPERATION, ';', for_node, nullptr);
	}

	IF_PARSED (cur_index, delim_stmt, parse_DELIMITED_STMT()) {
		if (!cur->is_op(';')) {
			ParseNode::DELETE(delim_stmt, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		} else {
			NEXT();

			if (delim_stmt->is_op(OPCODE_VAR_DEF) || delim_stmt->is_op(OPCODE_ARR_DEF) || delim_stmt->is_op(OPCODE_BREAK)) {
				return NEW_NODE(OPERATION, ';', delim_stmt, nullptr);
			} else {
				return NEW_NODE(OPERATION, ';', NEW_NODE(OPERATION, OPCODE_EXPR, delim_stmt, nullptr), nullptr);
			}
		}
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_BLOCK_STATEMENT() {
	if (cur->is_op('{')) {
		NEXT();
		ParseNode *block_node = NEW_NODE(OPERATION, '{', nullptr, nullptr);
		IF_PARSED (cur_index, block_stmt, parse_BLOCK_STATEMENT()) {
			block_node->L = block_stmt;
			while (!cur->is_op('}')) {
				IF_PARSED(cur_index, next_block, parse_BLOCK_STATEMENT()) {
					block_stmt->R = next_block;
					block_stmt = next_block;
					continue;
				}

				ParseNode::DELETE(block_node, true);
				SET_ERR(ERROR_SYNTAX, cur);
				return nullptr;
			}

			NEXT();
			return block_node;
		}

		ParseNode::DELETE(block_node, true);
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	IF_PARSED (cur_index, statement, parse_STATEMENT()) {
		return statement;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_PROG() {
	return parse_BLOCK_STATEMENT();
}      

ParseNode *RecursiveParser::parse_G() {
	IF_PARSED (cur_index, prog, parse_PROG()) {
		if (cur->type == T_END) {
			return prog;
		} else {
			ParseNode::DELETE(prog, true);
		}
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_ELEM_FUNC() {

	// 0 args
	if (cur->is_op(OPCODE_ELEM_INPUT) || cur->is_op(OPCODE_ELEM_EXIT) || cur->is_op(OPCODE_ELEM_G_DRAW_TICK)) {
		int op = cur->get_op();
		NEXT();
		return NEW_NODE(OPERATION, op, nullptr, nullptr);
	}

	// 0-1 arg;
	if (cur->is_op(OPCODE_ELEM_PUTN)   || cur->is_op(OPCODE_ELEM_PUTC) || 
		cur->is_op(OPCODE_ELEM_MALLOC) || cur->is_op(OPCODE_RET) || cur->is_op(OPCODE_ELEM_G_FILL)) {

		int op = cur->get_op();
		NEXT();

		IF_PARSED (cur_index, arg, parse_EXPR()) {
			return NEW_NODE(OPERATION, op, nullptr, arg);
		}

		return NEW_NODE(OPERATION, op, nullptr, nullptr);
	}

	// 2 args;
	if (cur->is_op(OPCODE_ELEM_G_INIT) || cur->is_op(OPCODE_ELEM_G_PUT_PIXEL) || cur->is_op(OPCODE_ELEM_RANDOM)) {
		int op = cur->get_op();
		NEXT();
		IF_PARSED (cur_index, arg1, parse_EXPR()) {
			IF_PARSED (cur_index, arg2, parse_EXPR()) {
				return NEW_NODE(OPERATION, op, arg1, arg2);
			}
		}
	}

	// if (cur->is_op(OPCODE_RET)) {
	// 	NEXT();
	// 	ParseNode *ret = NEW_NODE(OPERATION, OPCODE_RET, nullptr, nullptr);

	// 	IF_PARSED (cur_index, expr_node, parse_EXPR()) {
	// 		ret->R = expr_node;
	// 	}

	// 	return ret;
	// }

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_ARG_DECL() {
	IF_PARSED (cur_index, var_def, parse_NEW_VAR_DEF()) {
		return var_def;
	}

	IF_PARSED (cur_index, id, parse_ID()) {
		return id;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_ARGLIST_DECL() {
	ParseNode *arglist = NEW_NODE(OPERATION, OPCODE_FUNC_ARG_DECL, nullptr, nullptr);
	ParseNode *cur_arg = arglist;

	while (cur->is_op('[')) {
		NEXT();

		IF_PARSED (cur_index, arg, parse_ARG_DECL()) {
			cur_arg->L = arg;
			cur_arg->R = NEW_NODE(OPERATION, OPCODE_FUNC_ARG_DECL, nullptr, nullptr);
			cur_arg = cur_arg->R;

			if (!cur->is_op(']')) {
				ParseNode::DELETE(arglist, true);
				SET_ERR(ERROR_SYNTAX, cur);
				return nullptr;
			} else {
				NEXT();
				continue;
			}
		}

		PREV();
		ParseNode::DELETE(arglist, true);
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	return arglist;
}

ParseNode *RecursiveParser::parse_FUNC_DECL() {
	RESET_POINT = cur_index;

	if (!cur->is_op(OPCODE_FUNC)) {
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}
	NEXT();

	IF_PARSED (cur_index, func_name, parse_ID()) {
		IF_PARSED (cur_index, arglist, parse_ARGLIST_DECL()) {
			ParseNode *func_info = NEW_NODE(OPERATION, OPCODE_FUNC_INFO, arglist, func_name);

			IF_PARSED (cur_index, func_body, parse_BLOCK_STATEMENT()) {
				ParseNode *func_decl = NEW_NODE(OPERATION, OPCODE_FUNC_DECL, func_info, func_body);
				return func_decl;
			}

			RESET();
			ParseNode::DELETE(func_info, true);
			SET_ERR(ERROR_SYNTAX, cur);
			return nullptr;
		}

		RESET();
		ParseNode::DELETE(func_name, true);
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	RESET();
	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_ARG_CALL() {
	if (cur->is_op('.')) {
		NEXT();
		return NEW_NODE(OPERATION, OPCODE_CONTEXT_ARG, nullptr, nullptr);
	}

	if (cur->is_op(']')) {
		return NEW_NODE(OPERATION, OPCODE_DEFAULT_ARG, nullptr, nullptr);
	}

	IF_PARSED (cur_index, expr_node, parse_DELIMITED_STMT()) {
		return NEW_NODE(OPERATION, OPCODE_EXPR, expr_node, nullptr);
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

ParseNode *RecursiveParser::parse_ARGLIST_CALL() {
	ParseNode *arglist = NEW_NODE(OPERATION, OPCODE_FUNC_ARG_CALL, nullptr, nullptr);
	ParseNode *cur_arg = arglist;

	while (cur->is_op('[')) {
		NEXT();

		IF_PARSED (cur_index, arg, parse_ARG_CALL()) {
			cur_arg->L = arg;
			cur_arg->R = NEW_NODE(OPERATION, OPCODE_FUNC_ARG_CALL, nullptr, nullptr);
			cur_arg = cur_arg->R;

			if (!cur->is_op(']')) {
				ParseNode::DELETE(arglist, true);
				SET_ERR(ERROR_SYNTAX, cur);
				return nullptr;
			} else {
				NEXT();
				continue;
			}
		}

		PREV();
		ParseNode::DELETE(arglist, true);
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	return arglist;
}

ParseNode *RecursiveParser::parse_FUNC_CALL() {
	cur->dump();
	printf("| ");
	printf("%d %d\n", cur->line, cur->pos);
	IF_PARSED (cur_index, id, parse_ID()) {
		ParseNode *func_call = NEW_NODE(OPERATION, OPCODE_FUNC_CALL, nullptr, id);

		IF_PARSED (cur_index, args, parse_ARGLIST_CALL()) {
			func_call->L = args;
			return func_call;
		}

		PREV();
		ParseNode::DELETE(func_call, true);
		SET_ERR(ERROR_SYNTAX, cur);
		return nullptr;
	}

	SET_ERR(ERROR_SYNTAX, cur);
	return nullptr;
}

RecursiveParser::RecursiveParser():
expr(nullptr),
cur_index(0),
cur(nullptr),
ERROR(0),
ERRPOS(nullptr)
{}

RecursiveParser::~RecursiveParser() {}

void RecursiveParser::ctor() {
	expr = nullptr;
	cur_index = 0;
	cur = nullptr;
	ERROR = 0;
	ERRPOS = nullptr;
}

RecursiveParser *RecursiveParser::NEW() {
	RecursiveParser *cake = (RecursiveParser*) calloc(1, sizeof(RecursiveParser));
	if (!cake) {
		return nullptr;
	}

	cake->ctor();
	return cake;
}

void RecursiveParser::dtor() {
	expr = nullptr;
	cur_index = 0;
	cur = nullptr;

	ERROR  = 0;
	ERRPOS = 0;
}

void RecursiveParser::DELETE(RecursiveParser *classname) {
	if (!classname) {
		return;
	}

	classname->dtor();
	free(classname);
}

//=============================================================================

ParseNode *RecursiveParser::parse(Vector<Token> *expression) {
	expr      = expression;
	cur_index = 0;
	cur       = &(*expr)[cur_index];

	ParseNode *res = parse_G();
	if (!ERROR) {
		return res;
	} else {
		ANNOUNCE("ERR", "parser", "an error occured during grammar parsing");
		ANNOUNCE("ERR", "parser", "line [%d] | pos [%d]", ERRPOS->line, ERRPOS->pos);
		//ERRPOS->dump();
		return nullptr;
	}
}

#undef NEXT
#undef PREV
#undef SETI
#undef RESET_POINT
#undef RESET
#undef NEW_NODE
#undef REQUIRE_OP
#undef IF_PARSED
#undef SET_ERR
