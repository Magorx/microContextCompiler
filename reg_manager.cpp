#include "reg_manager.h"

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
	// local_var_to_id = std::unordered_map<int, int>();
	// globl_var_to_id = std::unordered_map<int, int>();
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
	return ++cur_stack_size;
}

int RegManager::pop(const int reg) {
	compiler->cpl_pop_reg(reg);
	return --cur_stack_size;
}

int RegManager::get_least_used_reg() {
	int min_use = 1000000000;
	int min_idx = 0;

	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		if (!reg_info[i].enabled || reg_info[i].is_used || reg_info[i].var_type != REGMAN_TMP_REG) {
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

int RegManager::get_local_var_reg(int offset, const char* var_name) {
	int reg = get_least_used_reg();

	reg_info[reg].reg 	  = REGMAN_REGS[reg];
	reg_info[reg].is_used = 1;
	reg_info[reg].id  	  = ++max_id;
	
	id_to_reg[reg_info[reg].id] = reg_info[reg];

	compiler->cpl_mov_reg_mem(reg_info[reg].reg, REG_RBP_DISPL(offset * -8));

	return reg;
}

int RegManager::get_globl_var_reg(int offset, const char* var_name) {
	int reg = get_least_used_reg();

	reg_info[reg].reg 	  = REGMAN_REGS[reg];
	reg_info[reg].is_used = 1;
	reg_info[reg].id   	  = ++max_id;
	
	id_to_reg[reg_info[reg].id] = reg_info[reg];

	compiler->cpl_mov_reg_mem64(reg_info[reg].reg, 0);
	compiler->obj.request_fixup({var_name, (int) compiler->cmd.get_size() - 4, fxp_ABSOLUTE});

	printf("REQUEST %s\n", var_name);

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

int RegManager::get_var_reg(int offset, REGMAN_VAR_TYPE var_type, const char* var_name) {
	int reg = get_var_used_reg(offset, var_type);
	if (reg == -1) {
		if (var_type == REGMAN_VAR_LOCAL) {
			reg = get_local_var_reg(offset, var_name);
		} else {
			reg = get_globl_var_reg(offset, var_name);
		}
	}

	reg_info[reg].offset   = offset;
	reg_info[reg].var_type = var_type;
	reg_info[reg].id_name  = var_name;

	reg_info[reg].last_use = ++max_use;
	return REGMAN_REGS[reg];
}

void RegManager::release_var_reg(int reg) {
	reg = get_ind_by_reg(reg);
	reg_info[reg].is_used = 0;
	id_to_reg.erase(reg_info[reg].id);
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

int RegManager::store_reg_info(const int reg) {
	int id = reg_info[reg].id;
	ANNOUNCE("  store", "regman", "reg[%d] -> id[%d]", reg, id);

	if (reg_info[reg].var_type == REGMAN_TMP_REG) {
		if (reg_info[reg].offset < 0) {
			push(reg_info[reg].reg);
			id_to_stack_offset[id] = cur_stack_size;
			reg_info[reg].offset = cur_stack_size;
			id_to_reg[id] = reg_info[reg];
		} else {
			compiler->cpl_mov_mem_reg(REG_RSP_DISPL((id_to_stack_offset[id] - cur_stack_size) * -8), reg_info[reg].reg);
		}
	} else {
		if (reg_info[reg].var_type == REGMAN_VAR_LOCAL) {
			compiler->cpl_mov_mem_reg(REG_RBP_DISPL(reg_info[reg].offset * -8), reg_info[reg].reg);
		} else {
			compiler->cpl_mov_mem64_reg(reg_info[reg].reg, 0);
			compiler->obj.request_fixup({reg_info[reg].id_name, (int) compiler->cmd.get_size() - 4, fxp_ABSOLUTE});
		}
	}

	return 0;
}

int RegManager::restore_reg_info(const int id, bool to_store) {
	int reg = id_to_reg[id].reg;
	// ANNOUNCE("restore", "regman", "id[%d] -> reg[%d]", id, reg);

	if (reg_info[get_ind_by_reg(reg)].id == id) {
		return 0;
	}

	if (to_store && reg_info[get_ind_by_reg(reg)].is_used) {
		store_reg_info(get_ind_by_reg(id_to_reg[id].reg));
	}

	int stack_offset = id_to_stack_offset[id];
	reg_info[get_ind_by_reg(id_to_reg[id].reg)] = id_to_reg[id];

	if (reg_info[get_ind_by_reg(reg)].var_type == REGMAN_TMP_REG) {
		// ANNOUNCE("restore", "regman", "is a tmp reg");
		compiler->cpl_mov_reg_mem(reg, REG_RSP_DISPL((stack_offset - cur_stack_size) * -8));
	} else {
		// ANNOUNCE("restore", "regman", "is a local var reg");
		compiler->cpl_mov_reg_mem(reg, REG_RBP_DISPL(reg_info[get_ind_by_reg(reg)].offset * -8));
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
