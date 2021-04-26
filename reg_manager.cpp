#include "reg_manager.h"

RegManager::RegManager()
{}

RegManager::~RegManager()
{}

void RegManager::ctor(Compiler *compiler_) {
	compiler = compiler_;
	for (int i = 0; i < REGMAN_REGS_CNT; ++i) {
		reg_info[i] = {0, 0};
	}
	id_to_reg.ctor();
	max_id = 0;
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
	id_to_reg.dtor();
}

void RegManager::DELETE(RegManager *classname) {
	if (!classname) {
		return;
	}

	classname->dtor();
	free(classname);
}


