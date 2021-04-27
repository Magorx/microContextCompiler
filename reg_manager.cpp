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
		reg_info[i] = {i, 0, 0, -1, nullptr, -1, 1, -1};
	}

	max_id  = 0;
	max_use = 0;
	cur_stack_size = 0;

	id_to_reg = std::unordered_map<int, RegUseInfo>();
	id_to_stack_offset = std::unordered_map<int, int>();
	local_var_to_id = std::unordered_map<int, int>();
	globl_var_to_id = std::unordered_map<int, int>();
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
		if (!reg_info[i].enabled) {
			continue;
		}

		if (!reg_info[i].is_used) {
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
	if (local_var_to_id.find(offset) != local_var_to_id.end()) {
		int id = local_var_to_id[offset];
		int reg = id_to_reg[id].reg;
		check_restore_reg(get_ind_by_reg(reg), id);
		return reg;
	}

	int reg = get_least_used_reg();

	reg_info[reg].reg 	   = REGMAN_REGS[reg];
	reg_info[reg].is_used  = 1;
	reg_info[reg].is_local = 1;
	reg_info[reg].id_name  = var_name;
	reg_info[reg].offset   = offset;
	reg_info[reg].id  	   = ++max_id;
	
	id_to_reg[reg_info[reg].id] = reg_info[reg];
	local_var_to_id[offset] = reg_info[reg].id;

	return reg_info[reg].reg;
}

int RegManager::get_globl_var_reg(int offset, const char* var_name) {
	if (globl_var_to_id.find(offset) != globl_var_to_id.end()) {
		int id = globl_var_to_id[offset];
		int reg = id_to_reg[id].reg;
		check_restore_reg(get_ind_by_reg(reg), id);
		return reg;
	}

	int reg = get_least_used_reg();

	reg_info[reg].reg 	   = REGMAN_REGS[reg];
	reg_info[reg].is_used  = 1;
	reg_info[reg].is_local = 0;
	reg_info[reg].id_name  = var_name;
	reg_info[reg].offset   = offset;
	reg_info[reg].id   	   = ++max_id;
	
	id_to_reg[reg_info[reg].id] = reg_info[reg];
	globl_var_to_id[offset] = reg_info[reg].id;

	return reg_info[reg].reg;
}

int RegManager::get_var_reg(int offset, REGMAN_VAR_TYPE var_type, const char* var_name) {
	int reg = 0;
	if (var_type == REGMAN_VAR_LOCAL) {
		reg = get_local_var_reg(offset, var_name);
	} else {
		printf("[ERR]<=>: THATS A FUCKEN ERROR = REGMAN CANT USE GLOBAL VARIABLES\n");
		reg = get_globl_var_reg(offset, var_name);
	}

	reg_info[get_ind_by_reg(reg)].last_use = ++max_use;
	return reg;
}

void RegManager::release_var_reg(int reg) {
	if (!reg_info[reg].is_used) {
		return;
	}

	int offset = reg_info[reg].offset;
	if (reg_info[reg].is_local) {
		local_var_to_id.erase(offset);
	} else {
		globl_var_to_id.erase(offset);
	}

	reg_info[reg].is_used = 0;
	reg_info[reg].offset = -1;
}

void RegManager::release_tmp_reg(int reg) {
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
	reg_info[reg].is_local  = -1;

	id_to_reg[reg_info[reg].id] = reg_info[reg];

	return reg_info[reg].reg;
}

int RegManager::store_reg_info(const int reg) {
	printf("reg to store: %d\n", reg);
	int id = reg_info[reg].id;

	if (reg_info[reg].is_local == -1) {
		if (reg_info[reg].offset < 0) {
			printf("pushing\n");
			push(reg_info[reg].reg);
			id_to_stack_offset[id] = cur_stack_size;
			reg_info[reg].offset = cur_stack_size;
			id_to_reg[id] = reg_info[reg];
		} else {
			printf("put\n");
			compiler->cpl_mov_mem_reg(REG_RSP_DISPL(id_to_stack_offset[id] - cur_stack_size), reg_info[reg].reg);
		}
	} else {
		compiler->cpl_mov_mem_reg(REG_RBP_DISPL(reg_info[reg].offset), reg_info[reg].reg);
	}

	return 0;
}

int RegManager::restore_reg_info(const int id) {
	int reg = id_to_reg[id].reg;
	int stack_offset = id_to_stack_offset[id];
	reg_info[get_ind_by_reg(id_to_reg[id].reg)] = id_to_reg[id];

	if (reg_info[reg].is_local == -1) {
		compiler->cpl_mov_reg_mem(reg, REG_RSP_DISPL(stack_offset - cur_stack_size));
	} else {
		compiler->cpl_mov_reg_mem(reg, REG_RBP_DISPL(reg_info[get_ind_by_reg(reg)].offset));
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
