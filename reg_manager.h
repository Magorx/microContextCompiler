#ifndef REG_MASTER
#define REG_MASTER

#include <unordered_map>
#include <map>

#include "compiler.h"
#include "labels_templates.h"

#include "general/cpp/vector.hpp"
#include "general/c/announcement.h"

class Compiler;

enum REGMAN_VAR_TYPE {
	REGMAN_VAR_LOCAL  = 1,
	REGMAN_VAR_GLOBAL = 2,
	REGMAN_TMP_REG    = 3
};

enum REGMAN_STORE_TYPE {
	REGMAN_ALL 	   = 3,
	REGMAN_VARS    = 2,
	REGMAN_GLOBALS = 1,
	REGMAN_TMPS    = 4
};

inline bool IS_VAR(const int x) {
	return (x == REGMAN_VAR_LOCAL || x == REGMAN_VAR_GLOBAL);
}

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
	int   reg;
	int   last_use;
	char  is_used;
	int   id;
	const char *id_name;
	int   offset;
	char  enabled;
	char  var_type;
};

struct RegManagerState {
	RegUseInfo regs[REGMAN_REGS_CNT];
	int id;
};

class RegManager {
private:
// data =======================================================================
	Compiler *compiler;
	RegUseInfo reg_info[REGMAN_REGS_CNT];
	std::unordered_map<int, RegUseInfo> id_to_reg;
	std::unordered_map<int, int> id_to_stack_offset;
	int max_id;
	int max_use;

	int cur_stack_size;

	Vector<RegManagerState*> states;
	int max_state_id;
//=============================================================================

	int get_local_var_reg(int offset, const char* var_name, char to_prevent_load = false);
	int get_globl_var_reg(int offset, const char *var_name, char to_prevent_load = false);

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
	int  get_least_used_reg();
	int  get_var_used_reg(int offset, REGMAN_VAR_TYPE var_type);
	void disable_reg(const int reg);
	void enable_reg (const int reg);

	int get_var_reg(int offset, REGMAN_VAR_TYPE var_type, const char* var_name, char to_prevent_load = false);
	int get_tmp_reg(int id = 0);

	void release_var_reg(int reg);
	void release_tmp_reg(int reg);

	int store_reg_info  (int reg);
	int restore_reg_info(const int id, bool to_store=true, bool force_restore=false);

	int push(const int reg);
	int pop (const int reg);
	int alter_rsp(const int drsp);

	void check_restore_reg(const int reg, const int id);

	int get_reg_id(const int reg) const;
	int get_ind_by_reg(const int reg) const;
	int get_max_state_id() const;

	int save_state();
	int load_state();
	int wipe_state();
	int rest_state();

	void flush_regs(char store_type = REGMAN_ALL, char to_wipe = true);

	int corrupt_reg(int reg);
};

#endif // REG_MASTER
