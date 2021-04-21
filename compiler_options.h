#ifndef COMPILER_OPTIONS
#define COMPILER_OPTIONS

#include <cctype>

const int GLOBAL_VARS_OFFSET = 100;
const int GLOBAL_VARS_MAX_COUNT = 100;
const int INIT_RVX_OFFSET = GLOBAL_VARS_OFFSET + GLOBAL_VARS_MAX_COUNT;
const int INIT_RMX_OFFSET = 1000;

enum LOOP_TYPE {
	LOOP_TYPE_WHILE = 1,
	LOOP_TYPE_FOR   = 2
};

struct Loop {
	int type;
	int number;

	Loop() :
	type(0),
	number(0)
	{}

	Loop(int type_, int number_) :
	type(type_),
	number(number_)
	{}
};

#define OPDEF(name, code, ign1, ign2, ign3) name = code,

enum OPERATION_CODE {
	#include "opcodes.h"
};

#undef OPDEF

const char *OPERATION_NAME(const int opcode);

bool is_normal_op			  (const int op);
bool is_bracket				  (const int op);
bool is_printable_op		  (const int op);
bool is_loggable_op 		  (const int op);
bool is_compiling_loggable_op (const int op);
bool is_splitting_op		  (const int op);

#endif // COMPILER_OPTIONS
