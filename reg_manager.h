#ifndef REG_MASTER
#define REG_MASTER

#include <unordered_map>
#include <map>

#include "compiler.h"
#include "general/cpp/vector.hpp"

#include "general/c/announcement.h"

class Compiler;

enum REGMAN_VAR_TYPE {
	REGMAN_VAR_LOCAL  = 1,
	REGMAN_VAR_GLOBAL = 2
};

const int REGMAN_REGS[] = {
	REG_R8,
	REG_R9,
};
const int REGMAN_REGS_CNT = sizeof(REGMAN_REGS) / sizeof(REGMAN_REGS[0]);

struct RegUseInfo {
	int   reg;
	int   last_use;
	char  is_used;
	int   id;
	const char *id_name;
	int   offset;
	char  enabled;
	char  is_local;
};

class RegManager {
private:
// data =======================================================================
	Compiler *compiler;
	RegUseInfo reg_info[REGMAN_REGS_CNT];
	std::unordered_map<int, RegUseInfo> id_to_reg;
	std::unordered_map<int, int> id_to_stack_offset;
	std::unordered_map<int, int> local_var_to_id;
	std::unordered_map<int, int> globl_var_to_id;
	int max_id;
	int max_use;

	int cur_stack_size;
//=============================================================================

	int get_local_var_reg(int offset, const char* var_name=nullptr);
	int get_globl_var_reg(int offset, const char *var_name=nullptr);

public:
	 RegManager();
	~RegManager();

	RegManager(const RegManager&) = delete;
	RegManager &operator=(const RegManager&) = delete;

	void ctor(Compiler *comp);
	static RegManager *NEW(Compiler *comp);

	void dtor();
	static void DELETE(RegManager *classname);
//=============================================================================
	int get_least_used_reg();
	void disable_reg(const int reg);
	void enable_reg (const int reg);

	int get_var_reg(int offset, REGMAN_VAR_TYPE var_type, const char* var_name=nullptr);
	int get_tmp_reg(int id = 0);

	void release_var_reg(int reg);
	void release_tmp_reg(int reg);

	int store_reg_info  (const int reg);
	int restore_reg_info(const int id);

	int push(const int reg);
	int pop (const int reg);

	void check_restore_reg(const int reg, const int id);

	int get_reg_id(const int reg) const;
	int get_ind_by_reg(const int reg) const;
};

#endif // REG_MASTER
