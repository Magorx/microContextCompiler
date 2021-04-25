#ifndef REG_MASTER
#define REG_MASTER

#include "compiler.h"

enum REGMAN_VAR_TYPE {
	REGMAN_VAR_LOCAL  = 1,
	REGMAN_VAR_GLOBAL = 2
};

extern const int REGMAN_STACK_REGS_CNT;
extern const int REGMAN_REGS[REGMAN_REGS_CNT];

class RegManager {
private:
// data =======================================================================
	Compiler *compiler;

//=============================================================================


public:
	 RegManager();
	~RegManager();

	void ctor();
	static RegManager *NEW();

	void dtor();
	static void DELETE(RegManager *classname);
//=============================================================================

	int get_var_reg(int offset, REGMAN_VAR_TYPE var_type, char* var_name=nullptr);
	int get_tmp_reg(int id);

};

#endif // REG_MASTER
