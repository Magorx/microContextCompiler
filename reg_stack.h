#ifndef REG_STACK
#define REG_STACK

#include "machine_codes/defines.h"
#include "byte_buffer.h"

const int REG_STACK_REGS_CNT = 6;
const int REG_STACK_INIREG = REG_R10;

extern int RSTACK_REGS[REG_STACK_REGS_CNT];

class RegStack {
private:
// data =======================================================================

	int size;
	int top;

//=============================================================================

public:
	RegStack ();
	~RegStack();

	void ctor();
	static RegStack *NEW();

	void dtor();
	static void DELETE(RegStack *reg_stack);
// =========================================

	void push(ByteBuffer &cmd, int reg);
	void pop (ByteBuffer &cmd, int reg);

	void save(ByteBuffer &cmd) const;
	void load(ByteBuffer &cmd) const;

	int top_reg() const;
};

#endif // REG_STACK
