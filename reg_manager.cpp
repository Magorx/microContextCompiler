#include "reg_manager.h"

#define CMD_SIZE (int) compiler->cmd.get_size()

RegManager::RegManager()
{}

RegManager::~RegManager()
{}

void RegManager::ctor(Compiler *compiler_) {
	if (!compiler_) {
		ANNOUNCE("ERR", __FUNCTION__, "compiler is (nullptr)");
		return;
	}

	compiler = compiler_;
	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		reg_info[i] = {i, 0, 0, -1, nullptr, -1, 1, REGMAN_TMP_REG};
	}

	max_id  = 0;
	max_use = 0;
	cur_stack_size = 0;

	id_to_reg = std::unordered_map<int, RegUseInfo>();
	id_to_stack_offset = std::unordered_map<int, int>();
	
	states.ctor();
	max_state_id = 0;
}

RegManager *RegManager::NEW(Compiler *compiler_) {
	RegManager *cake = (RegManager*) calloc(1, sizeof(RegManager));
	if (!cake) {
		return nullptr;
	}

	cake->ctor(compiler_);
	return cake;
}

void RegManager::dtor() {
}

void RegManager::DELETE(RegManager *classname) {
	if (!classname) {
		return;
	}

	classname->dtor();
	free(classname);
}

int RegManager::push(const int reg) {
	compiler->cpl_push_reg(reg);
	return cur_stack_size += sizeof(long long);
}

int RegManager::pop(const int reg) {
	compiler->cpl_pop_reg(reg);
	return cur_stack_size -= sizeof(long long);
}

int RegManager::get_least_used_reg() {
	int min_use = 1000000000;
	int min_idx = 0;

	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		if (!reg_info[i].enabled || reg_info[i].is_used || IS_VAR(reg_info[i].var_type)) {
			continue;
		}

		return i;
	}

	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		if (!reg_info[i].enabled) {
			continue;
		}

		if (!reg_info[i].is_used) {
			if (reg_info[i].var_type != REGMAN_TMP_REG) {
				store_reg_info(i);
			}

			return i;
		}

		if (reg_info[i].last_use < min_use) {
			min_use = reg_info[i].last_use;
			min_idx = i;
		}
	}

	int reg = min_idx;

	store_reg_info(reg);

	return reg;
}

void RegManager::disable_reg(const int reg) {
	if (reg >= REGMAN_REGS_CNT) {
		return;
	}

	reg_info[reg].enabled = 0;
}

void RegManager::enable_reg(const int reg) {
	if (reg >= REGMAN_REGS_CNT) {
		return;
	}

	reg_info[reg].enabled = 1;
}

int RegManager::get_local_var_reg(int offset, const char* var_name, char to_prevent_load) {
	int reg = get_least_used_reg();

	reg_info[reg].reg 	  = REGMAN_REGS[reg];
	reg_info[reg].is_used = 1;
	reg_info[reg].id  	  = ++max_id;
	
	id_to_reg[reg_info[reg].id] = reg_info[reg];
	if (!to_prevent_load) {
		compiler->cpl_mov_reg_mem(reg_info[reg].reg, REG_RBP_DISPL(offset));
	}

	return reg;
}

int RegManager::get_globl_var_reg(int offset, const char* var_name, char to_prevent_load) {
	int reg = get_least_used_reg();

	reg_info[reg].reg 	  = REGMAN_REGS[reg];
	reg_info[reg].is_used = 1;
	reg_info[reg].id   	  = ++max_id;
	
	id_to_reg[reg_info[reg].id] = reg_info[reg];
	if (!to_prevent_load) {
		compiler->cpl_mov_reg_mem64(reg_info[reg].reg, 0);
		compiler->obj.request_fixup({var_name, CMD_SIZE - 4, fxp_ABSOLUTE});
	}

	return reg;
}

int RegManager::get_var_used_reg(int offset, REGMAN_VAR_TYPE var_type) {
	for (int reg = 0; reg < REGMAN_REGS_CNT; ++reg) {
		if (reg_info[reg].offset == offset && reg_info[reg].var_type == var_type) {
			reg_info[reg].is_used = 1;
			id_to_reg[reg_info[reg].id] = reg_info[reg];
			return reg;
		}
	}

	return -1;
}

int RegManager::get_var_reg(int offset, REGMAN_VAR_TYPE var_type, const char* var_name, char to_prevent_load) {
	_LOG ANNOUNCE("GVR", "regman", "requested var reg");
	_LOG ANNOUNCE_NOCODE("|- name [%s]", var_name);
	_LOG ANNOUNCE_NOCODE("|- offs [%d]", offset);

	int reg = get_var_used_reg(offset, var_type);
	if (reg == -1) {
		if (var_type == REGMAN_VAR_LOCAL) {
			reg = get_local_var_reg(offset, var_name, to_prevent_load);
		} else {
			reg = get_globl_var_reg(offset, var_name, to_prevent_load);
		}
	}

	reg_info[reg].offset   = offset;
	reg_info[reg].var_type = var_type;
	reg_info[reg].id_name  = var_name ? strdup(var_name) : nullptr;

	_LOG ANNOUNCE_NOCODE("|- id   [%d]", reg_info[reg].id);

	id_to_reg[reg_info[reg].id] = reg_info[reg];
	reg_info[reg].last_use = ++max_use;
	return REGMAN_REGS[reg];
}

void RegManager::release_var_reg(int reg) {
	reg = get_ind_by_reg(reg);
	reg_info[reg].is_used = 0;
	// id_to_reg.erase(reg_info[reg].id);
}

void RegManager::release_tmp_reg(int reg) {
	reg = get_ind_by_reg(reg);
	reg_info[reg].is_used = 0;
	id_to_reg.erase(reg_info[reg].id);
}

int RegManager::get_tmp_reg(int id) {
	if (id != 0) {
		if (id_to_reg.find(id) != id_to_reg.end()) {
			int reg = get_ind_by_reg(id_to_reg[id].reg);
			check_restore_reg(reg, id);
			return reg;
		} else {
			ANNOUNCE("ERR", __FUNCTION__, "bad tmp reg id requested\n");
		}
	}

	int reg = get_least_used_reg();
	reg_info[reg].reg 		= REGMAN_REGS[reg];
	reg_info[reg].is_used 	= 1;
	reg_info[reg].last_use 	= ++max_use;
	reg_info[reg].offset 	= -1;
	reg_info[reg].id 		= ++max_id;
	reg_info[reg].id_name   = "/0/";
	reg_info[reg].var_type  = REGMAN_TMP_REG;

	id_to_reg[reg_info[reg].id] = reg_info[reg];

	return reg_info[reg].reg;
}

int RegManager::store_reg_info(int reg) {
	int id = reg_info[reg].id;
	_LOG ANNOUNCE("STR", "regman", "reg[%d] -> id[%d] reg_idx[%d]", reg_info[reg].reg, id, reg);

	if (reg_info[reg].var_type == REGMAN_TMP_REG) {
		_LOG ANNOUNCE_NOCODE("tmp reg");
		if (reg_info[reg].offset < 0) {
			push(reg_info[reg].reg);
			id_to_stack_offset[id] = cur_stack_size;
			reg_info[reg].offset = cur_stack_size;
			id_to_reg[id] = reg_info[reg];
		} else {
			compiler->cpl_mov_mem_reg(REG_RSP_DISPL((id_to_stack_offset[id] - cur_stack_size) * -1), reg_info[reg].reg);
		}
	} else {
		if (reg_info[reg].var_type == REGMAN_VAR_LOCAL) {
			_LOG ANNOUNCE_NOCODE("local var[%s] reg", reg_info[reg].id_name);
			compiler->cpl_mov_mem_reg(REG_RBP_DISPL(reg_info[reg].offset), reg_info[reg].reg);
		} else if (reg_info[reg].var_type == REGMAN_VAR_GLOBAL) {
			_LOG ANNOUNCE_NOCODE("globl var[%s] reg", reg_info[reg].id_name);
			compiler->cpl_mov_mem64_reg(0, reg_info[reg].reg);
			compiler->obj.request_fixup({reg_info[reg].id_name, CMD_SIZE - 4, fxp_ABSOLUTE});
		}
	}

	return 0;
}

int RegManager::restore_reg_info(const int id, bool to_store, bool force_restore) {
	if (id < 0) {
		return -1;
	}

	int reg = id_to_reg[id].reg;
	_LOG ANNOUNCE("RST", "regman", "id[%d] -> reg[%d] reg_ixd[%d]", id, reg, get_ind_by_reg(reg));

	if (!force_restore && reg_info[get_ind_by_reg(reg)].id == id) {
		return 0;
	}

	if (to_store && reg_info[get_ind_by_reg(reg)].is_used) {
		store_reg_info(get_ind_by_reg(id_to_reg[id].reg));
	}

	int stack_offset = id_to_stack_offset[id];
	reg_info[get_ind_by_reg(id_to_reg[id].reg)] = id_to_reg[id];

	if (reg_info[get_ind_by_reg(reg)].var_type == REGMAN_TMP_REG) {
		_LOG ANNOUNCE_NOCODE("tmp reg");
		compiler->cpl_mov_reg_mem(reg, REG_RSP_DISPL((stack_offset - cur_stack_size) * -1));
	} else {
		if (reg_info[get_ind_by_reg(reg)].var_type == REGMAN_VAR_LOCAL) {
			_LOG ANNOUNCE_NOCODE("local var[%s] reg", reg_info[get_ind_by_reg(reg)].id_name);
			compiler->cpl_mov_reg_mem(reg, REG_RBP_DISPL(reg_info[get_ind_by_reg(reg)].offset * -1));
		} else if (reg_info[get_ind_by_reg(reg)].var_type == REGMAN_VAR_GLOBAL) {
			_LOG ANNOUNCE_NOCODE("globl var[%s] reg", reg_info[get_ind_by_reg(reg)].id_name);
			compiler->cpl_mov_reg_mem64(reg_info[get_ind_by_reg(reg)].reg, 0);
			compiler->obj.request_fixup({reg_info[get_ind_by_reg(reg)].id_name, CMD_SIZE - 4, fxp_ABSOLUTE});
		}
	}

	return 0;
}

void RegManager::check_restore_reg(const int reg, const int id) {
	if (reg_info[reg].id != id) {
		store_reg_info(reg);
		restore_reg_info(id);
	}
}

int RegManager::get_reg_id(const int reg) const {
	return reg_info[get_ind_by_reg(reg)].id;
}

int RegManager::get_ind_by_reg(const int reg) const {
	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		if (reg == reg_info[i].reg) {
			return i;
		}
	}
	return -1;
}

int RegManager::save_state() {
	RegManagerState *state = (RegManagerState*) calloc(1, sizeof(RegManagerState));
	if (!state) {
		RAISE_ERROR("can't allocate memory for new state save");
		return -1;
	}

	memcpy(state->regs, reg_info, sizeof(RegUseInfo) * REGMAN_REGS_CNT);
	state->id = ++max_state_id;

	states.push_back(state);

	return 0;
}

int RegManager::load_state() {
	if (!states.size()) {
		RAISE_ERROR("empty saved stack buffer, nothing to load");
		return -1;
	}

	RegManagerState *st = states[states.size() - 1];
	MicroObj *obj = &compiler->obj;
	char lname[MAX_LABEL_LEN];

	/* restore logic */
	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		if (st->regs[i].id > 0 && st->regs[i].id != reg_info[i].id) {
			if (IS_VAR(reg_info[i].var_type)) {
				store_reg_info(reg_info[i].reg); //~~~
				reg_info[i] = st->regs[i];
				restore_reg_info(st->regs[i].id, false, true);
			}
		}
	}

	return 0;
}

int RegManager::corrupt_reg(int reg) {
	if (reg >= REGMAN_REGS_CNT) {
		RAISE_ERROR("reg[%d] is >= REGMAN_REGS_CNT[%d]", reg, REGMAN_REGS_CNT);
		return -1;
	}

	reg_info[reg].id = -1;
	reg_info[reg].is_used = 0;

	return 0;
}

int RegManager::wipe_state() {
	_LOG ANNOUNCE("WIP", "regman", "state wiped");

	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		reg_info[i].var_type = 0;
		reg_info[i].id = -1;
	}

	return 0;
}

int RegManager::rest_state() {
	_LOG ANNOUNCE("RET", "regman", "restoring state");

	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		// printf("rest %d -> id %d used %d var_type %d off %d\n", i, reg_info[i].id, reg_info[i].is_used, reg_info[i].var_type, reg_info[i].offset);
		if (reg_info[i].is_used || IS_VAR(reg_info[i].var_type)) {
			_LOG ANNOUNCE_NOCODE("reg_idx[%d] -> id[%d]", i, reg_info[i].id);
			restore_reg_info(reg_info[i].id, false, true);
		}
	}
	return 0;
}

void RegManager::flush_regs(char store_type, char to_wipe) {
	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		if (   store_type == REGMAN_GLOBALS && reg_info[i].var_type != REGMAN_VAR_GLOBAL
			|| store_type == REGMAN_VARS && !IS_VAR(reg_info[i].var_type)
			|| store_type == REGMAN_TMPS && IS_VAR(reg_info[i].var_type)) {
			continue;
		}

		if (!reg_info[i].is_used && !IS_VAR(reg_info[i].var_type)) {
			continue;
		}

		_LOG ANNOUNCE("FSH", "regman","reg_idx[%d] -> reg[%d] | id[%d] | offset[%d]\n", i, reg_info[i].reg, reg_info[i].id, reg_info[i].offset);
		store_reg_info(i);
	}

	if (to_wipe) {
		wipe_state();
	}
}

int RegManager::get_max_state_id() const {
	return max_state_id;
}

int RegManager::alter_rsp(const int drsp) {
	compiler->cpl_mov_reg_imm64(REG_RAX, drsp);
	compiler->cpl_math_op(REG_RSP, REG_RAX, '+');
	return cur_stack_size += drsp;
}