#ifndef REG_MASTER
#define REG_MASTER

#include "compiler.h"
#include "general/cpp/vector.hpp"

enum REGMAN_VAR_TYPE {
	REGMAN_VAR_LOCAL  = 1,
	REGMAN_VAR_GLOBAL = 2
};

const int REGMAN_REGS[] = {
	REG_R8,
	REG_R9,
	REG_R10,
	REG_R11,
	REG_R12,
	REG_R13,
	REG_R14,
	REG_R15
};
const int REGMAN_REGS_CNT = sizeof(REGMAN_REGS) / sizeof(REGMAN_REGS[0]);

struct RegUseInfo {
	int last_use;
	int cur_id;
};

class RegManager {
private:
// data =======================================================================
	Compiler *compiler;
	RegUseInfo reg_info[REGMAN_REGS_CNT];
	Vector<int> id_to_reg;
	int max_id;
//=============================================================================


public:
	 RegManager();
	~RegManager();

	void ctor(Compiler *comp);
	static RegManager *NEW(Compiler *comp);

	void dtor();
	static void DELETE(RegManager *classname);
//=============================================================================

	int get_var_reg(int offset, REGMAN_VAR_TYPE var_type, char* var_name=nullptr);
	int get_tmp_reg(int id);

};

#endif // REG_MASTER
