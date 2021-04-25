#include "reg_manager.h"

const int REGMAN_STACK_REGS_CNT = 10;
const int REGMAN_REGS[REGMAN_REGS_CNT] = {
	REG_R8,
	REG_R9,
	REG_R10,
	REG_R11,
	REG_R12,
	REG_R13,
	REG_R14,
	REG_R15
};

RegManager::RegManager():
{}

RegManager::~RegManager()
{}

void RegManager::ctor() {

}

RegManager *RegManager::NEW() {
	RegManager *cake = (RegManager*) calloc(1, sizeof(RegManager));
	if (!cake) {
		return nullptr;
	}

	cake->ctor();
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


