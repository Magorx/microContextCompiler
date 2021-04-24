#include "reg_stack.h"

int RSTACK_REGS[REG_STACK_REGS_CNT] = {
	REG_R10,
	REG_R11,
	REG_R12,
	REG_R13,
	REG_R14,
	REG_R15
};

RegStack::RegStack():
size(0),
top(-1)
{}

RegStack::~RegStack() {}

void RegStack::ctor() {
	size =  0;
	top  = -1;
}

RegStack *RegStack::NEW() {
	RegStack *cake = (RegStack*) calloc(1, sizeof(RegStack));
	if (!cake) {
		return nullptr;
	}

	cake->ctor();
	return cake;
}

void RegStack::dtor() {
	size = -1;
	top  = -1;
}

void RegStack::DELETE(RegStack *reg_stack) {
	if (!reg_stack) {
		return;
	}

	reg_stack->dtor();
	free(reg_stack);
}

void RegStack::push(ByteBuffer &cmd, int reg) {
	top = (top + 1) % REG_STACK_REGS_CNT;

	if (size >= REG_STACK_REGS_CNT) {
		cmd.put(cmd_PUSH_TABLE[RSTACK_REGS[top]]);
	}
	++size;

	const cmd_MovCommand &com = cmd_MOV_TABLE[RSTACK_REGS[top]][reg];
    cmd.put(((byte*) com.bytecode), 3);
}

void RegStack::pop(ByteBuffer &cmd, int reg) {
	const cmd_MovCommand &com = cmd_MOV_TABLE[reg][RSTACK_REGS[top]];
	cmd.put(((byte*) com.bytecode), 3);

	--size;
	if (size >= REG_STACK_REGS_CNT) {
		cmd.put(cmd_POP_TABLE[RSTACK_REGS[top]]);
	}

	top = (top - 1) % REG_STACK_REGS_CNT;
}

void RegStack::save(ByteBuffer &cmd) const {
	for (int i = 0; i < REG_STACK_REGS_CNT; ++i) {
		cmd.put(cmd_PUSH_TABLE[RSTACK_REGS[i]]);
	}
}

void RegStack::load(ByteBuffer &cmd) const {
	for (int i = REG_STACK_REGS_CNT - 1; i >= 0; --i) {
		cmd.put(cmd_POP_TABLE[RSTACK_REGS[i]]);
	}
}

int RegStack::top_reg() const {
	return RSTACK_REGS[top];
}
