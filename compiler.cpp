#include "compiler.h"

void Compiler::fprintf_asgn_additional_operation(FILE *file, const int op) {
	switch (op) {
		case OPCODE_ASGN_ADD :
			fprintf(file, "add\n");
			break;
		case OPCODE_ASGN_SUB :
			fprintf(file, "sub\n");
			break;
		case OPCODE_ASGN_MUL :
			fprintf(file, "mul\n");
			break;
		case OPCODE_ASGN_DIV :
			fprintf(file, "div\n");
			break;
		case OPCODE_ASGN_POW :
			fprintf(file, "pow\n");
			break;
		default:
			RAISE_ERROR("bad asgn operation, HOW\n");
	}
}

void Compiler::cpl_nop() {
	cmd.put(0x90);
}

void Compiler::cpl_math_op(const int reg_dst, const int reg_src, const char op) {
	switch(op) {
		case '+' : {
			cmd.put(cmd_ADDTABLE[reg_dst][reg_src], 3);
			break;
		}

		case '-' : {
			cmd.put(cmd_SUBTABLE[reg_dst][reg_src], 3);
			break;
		}

		case '*' : {
			cmd.put(cmd_IMULTABLE[reg_dst][reg_src], 4);
			break;
		}

		case '/' : {
			if (reg_dst != REG_RAX) {
				RAISE_ERROR("wrong reg_dst for IDIV: %d\n", reg_dst);
				return;
			}

			cmd.put(cmd_XORTABLE[REG_RDX][REG_RDX], 3);
			cmd.put(cmd_IDIVTABLE[reg_src], 3);
			break;
		}

		case '^' : {
			cmd.put(cmd_XORTABLE[reg_dst][reg_src], 3);
			break;
		}
	}
}

void Compiler::cpl_log_op(const int reg_dst, const int reg_src, const char op) {
	cmd.put(cmd_CMPTABLE[reg_dst][reg_src], 3);
	char log_jmp_offset = 7;

	switch(op) {
		case '<':
			cmd.put(cmd_JL_REL8);
			break;

		case '>':
			cmd.put(cmd_JG_REL8);
			break;

		case OPCODE_LE:
			cmd.put(cmd_JLE_REL8);
			break;

		case OPCODE_GE:
			cmd.put(cmd_JGE_REL8);
			break;

		case OPCODE_EQ:
			cmd.put(cmd_JE_REL8);
			break;

		case OPCODE_NEQ:
			cmd.put(cmd_JNE_REL8);
			break;

		// case OPCODE_OR:
		// 	cmd.put(cmd_JLE_REL8);
		// 	break;

		// case OPCODE_AND:
		// 	cmd.put(cmd_JLE_REL8);
		// 	break;

		default:
			RAISE_ERROR("Invalid log_op: %d\n", op);
	}

	cmd.put(log_jmp_offset + 2);    // jumping to mov rax, 1
	cpl_mov_reg_imm64(REG_RAX, 0);  // else - muov rax, 0 
	cmd.put(cmd_JMP_REL8);		    // 					 and jump 
	cmd.put(log_jmp_offset);		// 							  to the end
	cpl_mov_reg_imm64(REG_RAX, 1);
}

void Compiler::cpl_log_conn(const int reg_dst, const int reg_src, const char op) {
	if (op == OPCODE_AND) {
		cpl_test_reg_reg(reg_dst, reg_dst);
		cmd.put(cmd_JE_REL8);
		cmd.put(14);
		cpl_test_reg_reg(reg_src, reg_src);
		cmd.put(cmd_JE_REL8);
		cmd.put(9);
		cpl_mov_reg_imm64(REG_RAX, 1);
		cmd.put(cmd_JMP_REL8);
		cmd.put(7);
		cpl_mov_reg_imm64(REG_RAX, 0);
	} else if (op == OPCODE_OR) {
		cpl_test_reg_reg(reg_dst, reg_dst);
		cmd.put(cmd_JNE_REL8);
		cmd.put(14);
		cpl_test_reg_reg(reg_src, reg_src);
		cmd.put(cmd_JNE_REL8);
		cmd.put(9);
		cpl_mov_reg_imm64(REG_RAX, 0);
		cmd.put(cmd_JMP_REL8);
		cmd.put(7);
		cpl_mov_reg_imm64(REG_RAX, 1);
	} else {
		RAISE_ERROR("invalid log_conn op[%d]", op);
	}
}

void Compiler::cpl_mov_reg_imm32(const int reg_dst, const double val) {
	if (reg_dst < REG_R8) {
		cmd.put(cmd_MOVIMM32TABLE[reg_dst], 1);
	} else {
		cmd.put(cmd_MOVIMM32TABLE[reg_dst], 2);
	}
	
	int int_val = val;
	cmd.put((byte*) &int_val, 4);
}

void Compiler::cpl_mov_reg_imm64(const int reg_dst, const double val) {
	cmd.put(cmd_MOVIMM64TABLE[reg_dst], 3);
	int int_val = val;
	cmd.put((byte*) &int_val, 4);
}

void Compiler::cpl_mov_reg_reg(const int reg_dst, const int reg_src) {
	_log ANNOUNCE("MOV", "compiler", "reg[%d] <- reg[%d]\n", reg_dst, reg_src);
	cmd.put(cmd_MOV_TABLE[reg_dst][reg_src], 3);
}

void Compiler::cpl_mov_mem_reg(const int reg_dst, const int offset, const int reg_src) {
	if (reg_dst == REG_RSP) {
		cmd.put(cmd_MOV_RSPMEM_REG_DISPL32[reg_src], 4);
		cmd.put((byte*) &offset, 4);
	} else if (reg_dst == REG_RBP) {
		cmd.put(cmd_MOV_RBPMEM_REG_DISPL32[reg_src], 3);
		cmd.put((byte*) &offset, 4);
	} else if (reg_dst == REG_RAX) {
		cmd.put(cmd_MOV_RAXMEM_REG_DISPL32[reg_src], 3);
		cmd.put((byte*) &offset, 4);
	} else {
		RAISE_ERROR("invalid register to adress\n");
	}
}

void Compiler::cpl_mov_reg_mem(const int reg_dst, const int reg_src, const int offset) {
	if (reg_src == REG_RSP) {
		cmd.put(cmd_MOV_REG_RSPMEM_DISPL32[reg_dst], 4);
		cmd.put((byte*) &offset, 4);
	} else if (reg_src == REG_RBP) {
		cmd.put(cmd_MOV_REG_RBPMEM_DISPL32[reg_dst], 3);
		cmd.put((byte*) &offset, 4);
	} else if (reg_src == REG_RAX) {
		cmd.put(cmd_MOV_REG_RAXMEM_DISPL32[reg_dst], 3);
		cmd.put((byte*) &offset, 4);
	} else {
		ANNOUNCE("ERR", __FUNCTION__, "invalid register to adress");
	}
}

void Compiler::cpl_mov_reg_mem64(const int reg_dst, const int offset) {
	cmd.put(cmd_MOV_REG_MEM64[reg_dst], 4);
	cmd.put((byte*) &offset, 4);
}

void Compiler::cpl_mov_mem64_reg(const int offset, const int reg_src) {
	cmd.put(cmd_MOV_MEM64_REG[reg_src], 4);
	cmd.put((byte*) &offset, 4);
}

void Compiler::cpl_cmp_reg_reg(const int reg_src, const int reg_dst) {
	cmd.put(cmd_CMPTABLE[reg_src][reg_dst], 3);
}

void Compiler::cpl_test_reg_reg(const int reg_src, const int reg_dst) {
	cmd.put(cmd_TESTTABLE[reg_src][reg_dst], 3);
}

void Compiler::cpl_xchg_rax_reg(const int reg_dst) {
	cmd.put(cmd_XCHG_RAXTABLE[reg_dst], 2);
}

void Compiler::cpl_push_reg(const int reg_src) {
	if (reg_src < REG_R8) {
		cmd.put(cmd_PUSH_TABLE[reg_src], 1);
	} else {
		cmd.put(cmd_PUSH_TABLE[reg_src], 2);
	}
}

void Compiler::cpl_pop_reg(const int reg_dst) {
	cmd.put(cmd_POP_TABLE[reg_dst], 2);
}

void Compiler::hexdump_cmd() const {
	cmd.hexdump();
}

void Compiler::cpl_je_rel32  (const int offset) {
	cmd.put(cmd_JE_REL32);
	cmd.put((byte*) &offset, 4);
}

void Compiler::cpl_jne_rel32 (const int offset) {
	cmd.put(cmd_JNE_REL32);
	cmd.put((byte*) &offset, 4);
}

void Compiler::cpl_jl_rel32  (const int offset) {
	cmd.put(cmd_JL_REL32);
	cmd.put((byte*) &offset, 4);
}

void Compiler::cpl_jle_rel32 (const int offset) {
	cmd.put(cmd_JLE_REL32);
	cmd.put((byte*) &offset, 4);
}

void Compiler::cpl_jg_rel32  (const int offset) {
	cmd.put(cmd_JG_REL32);
	cmd.put((byte*) &offset, 4);
}

void Compiler::cpl_jge_rel32 (const int offset) {
	cmd.put(cmd_JGE_REL32);
	cmd.put((byte*) &offset, 4);
}

void Compiler::cpl_jmp_rel32 (const int offset) {
	cmd.put(cmd_JMP_REL32);
	cmd.put((byte*) &offset, 4);
}


void Compiler::cpl_call_rel32(const int offset) {
	_log ANNOUNCE("CAL", "compiler", "calling rel 32");
	cmd.put(cmd_CALL_REL32);
	cmd.put((byte*) &offset, 4);
}

void Compiler::cpl_ret() {
	cmd.put(cmd_RET);
}

void Compiler::cpl_breakpoint() {
	cmd.put(cmd_BREAKPOINT);
}

void Compiler::cpl_syscall() {
	cmd.put(cmd_SYSCALL);
}

void Compiler::cpl_operation(const CodeNode *node, FILE *file) {
	assert(node);
	assert(file);

	#define LOG_ERROR_LINE_POS(node) RAISE_ERROR("line [%d] | pos [%d]\n", node->line, node->pos);

	#define DUMP_L() if (node->L) {printf("L] "); node->L->full_dump(); printf("\n");}
	#define DUMP_R() if (node->R) {printf("R] "); node->R->full_dump(); printf("\n");}
	#define COMPILE_L() if (node->L) compile(node->L, file)
	#define COMPILE_R() if (node->R) compile(node->R, file)
	#define COMPILE_L_COMMENT() if (node->L && (is_compiling_loggable_op(node->L->get_op()))) { fprintf(file, "\n; "); node->L->space_dump(file); fprintf(file, "\n");} COMPILE_L()
	#define COMPILE_R_COMMENT() if (node->R && (is_compiling_loggable_op(node->R->get_op()))) { fprintf(file, "\n; "); node->R->space_dump(file); fprintf(file, "\n");} COMPILE_R()
	#define COMPILE_LR() do {COMPILE_L(); COMPILE_R();} while (0)

	#define CHECK_ERROR() do {if (ANNOUNCEMENT_ERROR) {return;}} while (0)

	int LAST_COMPILED = -1;
	int L_COMPLEX = CodeNode::get_ershov_number(node->get_L());
	int R_COMPLEX = CodeNode::get_ershov_number(node->get_R());

	#define COMPILE_MORE_COMPLEX() if (L_COMPLEX > R_COMPLEX) {COMPILE_L(); LAST_COMPILED = 0;} else {COMPILE_R(); LAST_COMPILED = 1;}
	#define COMPILE_ANOTHER() if (LAST_COMPILED) {COMPILE_L(); LAST_COMPILED = 0;} else {COMPILE_R(); LAST_COMPILED = 1;}
	#define IS_ARR(node) (node->is_op(OPCODE_FUNC_CALL) && id_table.find_func(node->R->get_id()) == NOT_FOUND)

	CHECK_ERROR();

	switch (node->get_op()) {
		case '+' :
		case '*' :
		case '^' :
		
		 {
			if (node->L) {
				COMPILE_MORE_COMPLEX();
				CHECK_ERROR();

				int r1    = regman->get_tmp_reg();
				int r1_id = regman->get_reg_id(r1);
				cpl_mov_reg_reg(r1, REG_RAX);

				COMPILE_ANOTHER();
				regman->restore_reg_info(r1_id);
				cpl_math_op(REG_RAX, r1, node->get_op());

				regman->release_tmp_reg(r1);
			} else {
				if (node->get_op() == '+') {
					COMPILE_R();
				} else {
					LOG_ERROR_LINE_POS(node);
				}
			}
			break;
		}

		case '-' :
		case '/' : {
			if (node->L && node->R) {
				COMPILE_MORE_COMPLEX();
				CHECK_ERROR();

				int r1    = regman->get_tmp_reg();
				int r1_id = regman->get_reg_id(r1);
				cpl_mov_reg_reg(r1, REG_RAX);

				COMPILE_ANOTHER();
				regman->restore_reg_info(r1_id);

				if (LAST_COMPILED) {
					cpl_xchg_rax_reg(r1);
				}

				cpl_math_op(REG_RAX, r1, node->get_op());

				regman->release_tmp_reg(r1);
			} else {
				if (node->get_op() == '-') {
					cpl_mov_reg_imm64(REG_RBX, -1);
					cpl_math_op(REG_RAX, REG_RBX, '*');
				} else {
					LOG_ERROR_LINE_POS(node);
				}
			}
			break;
		}

		case '=' : {
			cpl_nop();
			cpl_nop();
			COMPILE_R();
			CHECK_ERROR();

			if (node->L->is_id()) {
				int offset, found;
				cpl_lvalue(node->L, offset, found);
				if (found == NOT_FOUND) {
					RAISE_ERROR("can't find variable, sry\n");
					LOG_ERROR_LINE_POS(node);
					return;
				}

				int r1 = 0;
				char *name = node->L->get_id()->dup();
				r1 = regman->get_var_reg(offset, found, name, true);
				free(name);

				cpl_mov_reg_reg(r1, REG_RAX);
				regman->release_var_reg(r1);
			} else if (node->L->is_op(OPCODE_FUNC_CALL) && id_table.find_func(node->L->R->get_id()) == NOT_FOUND) { // array!
				int r1    = regman->get_tmp_reg();
				int r1_id = regman->get_reg_id(r1);
				cpl_mov_reg_reg(r1, REG_RAX);

				cpl_arr_lvalue(node->L, file);

				regman->restore_reg_info(r1_id);
				cpl_mov_mem_reg(REG_RAX, 0, r1);
				regman->release_tmp_reg(r1);
			} else {
				RAISE_ERROR("bad lvalue node, opcode[%d]\n", node->L->get_op());
				LOG_ERROR_LINE_POS(node);
			}

			break;
		}

		case OPCODE_EXCHANGE : {
		    if (node->L->is_id()) {
		    	int offset1, found1;
				cpl_lvalue(node->L, offset1, found1);
				char *name1 = node->L->get_id()->dup();
				int r1 = regman->get_var_reg(offset1, found1, name1);
				free(name1);

		    	if (node->R->is_id()) {
					int offset2, found2;
					cpl_lvalue(node->R, offset2, found2);

					char *name2 = node->R->get_id()->dup();
					int r2 = regman->get_var_reg(offset2, found2, name2);
					free(name2);

					cpl_mov_reg_reg(REG_RAX, r1);
					cpl_mov_reg_reg(r1, r2);
					cpl_mov_reg_reg(r2, REG_RAX);
					
					regman->release_var_reg(r2);
		    	} else if (IS_ARR(node->R)) {
					cpl_arr_lvalue(node->R, file);
					cpl_mov_reg_mem(REG_RBX, REG_RAX, 0);
					cpl_mov_mem_reg(REG_RAX, 0, r1);
					cpl_mov_reg_reg(r1, REG_RBX);
		    	} else {
		    		RAISE_ERROR("bad exchange operand\n");
		    		LOG_ERROR_LINE_POS(node);
		    		return;
		    	}

		    	regman->release_var_reg(r1);
		    } else if (IS_ARR(node->L)) {
		    	cpl_arr_lvalue(node->L, file);

		    	if (node->R->is_id()) {
		    		int offset2, found2;
					cpl_lvalue(node->R, offset2, found2);

					char *name2 = node->R->get_id()->dup();
					int r2 = regman->get_var_reg(offset2, found2, name2);
					free(name2);

					cpl_mov_reg_mem(REG_RBX, REG_RAX, 0);
					cpl_mov_mem_reg(REG_RAX, 0, r2);
					cpl_mov_reg_reg(r2, REG_RBX);

					regman->release_var_reg(r2);
		    	} else if (IS_ARR(node->R)) {
		    		cpl_push_reg(REG_RAX);

		    		cpl_arr_lvalue(node->R, file);

		    		int r1    = regman->get_tmp_reg();
					int r1_id = regman->get_reg_id(r1);
					cpl_mov_reg_mem(r1, REG_RAX, 0);

					cpl_push_reg(REG_RAX);
					cpl_mov_reg_mem(REG_RAX, REG_RSP, 8);
					cpl_mov_reg_mem(REG_RBX, REG_RAX, 0);
					cpl_mov_mem_reg(REG_RAX, 0, r1);
					cpl_pop_reg(REG_RAX);
					cpl_mov_mem_reg(REG_RAX, 0, REG_RBX);

					regman->release_tmp_reg(r1);

					cpl_rps_add(8);
		    	} else {
		    		RAISE_ERROR("bad exchange operand\n");
		    		LOG_ERROR_LINE_POS(node);
		    		return;
		    	}
		    } else {
	    		RAISE_ERROR("bad exchange operand\n");
	    		LOG_ERROR_LINE_POS(node);
	    		return;
		    }
			break;
		}

		case OPCODE_ASGN_ADD :
		case OPCODE_ASGN_SUB :
		case OPCODE_ASGN_MUL :
		case OPCODE_ASGN_DIV :
		case OPCODE_ASGN_POW : {
			COMPILE_R();
			CHECK_ERROR();

			int offset, found;
			cpl_lvalue(node->L, offset, found);
			if (found == NOT_FOUND) {
				RAISE_ERROR("can't find variable, sry\n");
				LOG_ERROR_LINE_POS(node);
				return;
			}

			int r1 = 0;
			char *name = node->L->get_id()->dup();
			r1 = regman->get_var_reg(offset, found, name);
			free(name);

			int op = asgn_op_to_op(node->get_op());

			if (op != '/') {
				cpl_math_op(r1, REG_RAX, op);
			} else {
				cpl_xchg_rax_reg(r1);
				cpl_math_op(REG_RAX, r1, op);
				cpl_mov_reg_reg(r1, REG_RAX);
			}

			cpl_mov_reg_reg(REG_RAX, r1);
			regman->release_var_reg(r1);

			break;
		}

		case OPCODE_SLEEP : {
			if (!node->L || !node->R) {
				RAISE_ERROR("bad sleep argument\n");
				LOG_ERROR_LINE_POS(node);
				return;
			}

			cpl_push_reg(REG_RSI);
			cpl_push_reg(REG_RDI);
			cpl_push_reg(REG_R11);

			COMPILE_R();
			cpl_push_reg(REG_RAX);

			COMPILE_L();
			
			cpl_push_reg(REG_RAX);
			cpl_mov_reg_reg(REG_RDI, REG_RSP);

			cpl_math_op(REG_RSI, REG_RSI, '^');

			cpl_mov_reg_imm64(REG_RAX, 35);

			cpl_syscall();

			cpl_pop_reg(REG_RAX);
			cpl_pop_reg(REG_RAX);

			cpl_pop_reg(REG_R11);
			cpl_pop_reg(REG_RDI);
			cpl_pop_reg(REG_RSI);

			break;
		}

		case '<':
		case '>' :
		case OPCODE_LE :
		case OPCODE_GE :
		case OPCODE_EQ :
		case OPCODE_NEQ : {
			COMPILE_MORE_COMPLEX();

			int r1    = regman->get_tmp_reg();
			int r1_id = regman->get_reg_id(r1);
			cpl_mov_reg_reg(r1, REG_RAX);

			COMPILE_ANOTHER();
			regman->restore_reg_info(r1_id);

			if (LAST_COMPILED) {
				cpl_xchg_rax_reg(r1);
			}

			cpl_log_op(REG_RAX, r1, node->get_op());
			regman->release_tmp_reg(r1);
			break;
		}

		case OPCODE_OR :
		case OPCODE_AND : {
			COMPILE_MORE_COMPLEX();

			int r1    = regman->get_tmp_reg();
			int r1_id = regman->get_reg_id(r1);
			cpl_mov_reg_reg(r1, REG_RAX);

			COMPILE_ANOTHER();
			regman->restore_reg_info(r1_id);

			cpl_log_conn(REG_RAX, r1, node->get_op());
			regman->release_tmp_reg(r1);
			break;
		}

		case OPCODE_EXPR : {
		    cpl_expr(node, file, true);
			break;
		}

		case OPCODE_VAR_DEF : {
			if (!node->L || !node->L->is_id()) {
				RAISE_ERROR("bad variable definition [\n");
				node->space_dump();
				printf("]\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}

			bool ret = id_table.declare_var(node->L->get_id(), 1);
			if (!ret) {
				RAISE_ERROR("Redefinition of the id [");
				node->L->get_id()->print();
				printf("]\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}

			int offset, found;
			cpl_lvalue(node->L, offset, found, true);
			char *name = node->L->get_id()->dup();

			if (found == NOT_FOUND) {
				printf("WTFWTFWTF\n");
				return;
			}

			if (found == ID_TYPE_GLOBAL) {
				obj.add_fixup({name, offset, fxp_ABSOLUTE, sizeof(long long)});
			} else {
				cpl_mov_reg_imm64(REG_RAX, 8);
				cpl_math_op(REG_RSP, REG_RAX, '-');
			}

			if (node->R) {
				COMPILE_R();

				int r1 = regman->get_var_reg(offset, found, name, true);
			    cpl_mov_reg_reg(r1, REG_RAX);
			    regman->release_var_reg(r1);
			}

			free(name);

			break;
		}

		case OPCODE_ARR_DEF : {
			CodeNode *arr_name = node->L->R;
			if (!node->L || !node->L->is_op(OPCODE_ARR_INFO)) {
				RAISE_ERROR("bad variable definition [\n");
				node->space_dump();
				printf("]\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}
			
			bool ret = id_table.declare_var(arr_name->get_id(), 1);
			if (!ret) {
				RAISE_ERROR("Redefinition of the id [");
				arr_name->get_id()->print();
				printf("]\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}
			
			int offset = 0;
			int found = 0;
			cpl_lvalue(arr_name, offset, found);

			char *vname = arr_name->get_id()->dup();

			obj.add_fixup({vname, offset, fxp_ABSOLUTE, sizeof(long long)});

			int r1 = regman->get_var_reg(offset, found, vname, true);
			int arr_size = node->L->L->get_val();
			cpl_calloc(arr_size);
			cpl_mov_reg_reg(r1, REG_RAX);

			regman->release_var_reg(r1);

			free(vname);
			break;
		}

		case OPCODE_WHILE : {
			regman->save_state();

			int cur_while_cnt = ++while_cnt;
			cycles_end_stack.push_back(Loop(LOOP_TYPE_WHILE, cur_while_cnt));
			char lname[MAX_LABEL_LEN] = {};

			regman->save_state();

			sprintf(lname, WHILE_CONDITION_TEMPLATE, cur_while_cnt);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			COMPILE_L();

			cpl_test_reg_reg(REG_RAX, REG_RAX);

			cpl_je_rel32(0);
			sprintf(lname, WHILE_END_TEMPLATE, cur_while_cnt);
			obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			COMPILE_R();

			regman->load_state();

			cpl_jmp_rel32(0);
			sprintf(lname, WHILE_CONDITION_TEMPLATE, cur_while_cnt);
			obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			sprintf(lname, WHILE_END_TEMPLATE, cur_while_cnt);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			regman->load_state();

			cycles_end_stack.pop_back();
			break;
		}

		case OPCODE_BREAK : {
			if (!cycles_end_stack.size()) {
				RAISE_ERROR("You can't use |< outside of the loop\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}

			Loop cur_cycle = cycles_end_stack[cycles_end_stack.size() - 1];
			char lname[MAX_LABEL_LEN] = {};
			if (cur_cycle.type == LOOP_TYPE_WHILE) {
				cpl_jmp_rel32(0);
				sprintf(lname, WHILE_END_TEMPLATE, cur_cycle.number);
				obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});
			} else if (cur_cycle.type == LOOP_TYPE_FOR) {
				cpl_jmp_rel32(0);
				sprintf(lname, FOR_END_TEMPLATE, cur_cycle.number);
				obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});
			} else {
				RAISE_ERROR("What cycle are you using??\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}

			break;
		}

		case OPCODE_CONTINUE : {
			if (!cycles_end_stack.size()) {
				RAISE_ERROR("You can't use |< outside of the loop\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}

			Loop cur_cycle = cycles_end_stack[cycles_end_stack.size() - 1];
			char lname[MAX_LABEL_LEN] = {};
			if (cur_cycle.type == LOOP_TYPE_WHILE) {
				cpl_jmp_rel32(0);
				sprintf(lname, WHILE_CONDITION_TEMPLATE, cur_cycle.number);
				obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});
			} else if (cur_cycle.type == LOOP_TYPE_FOR) {
				cpl_jmp_rel32(0);
				sprintf(lname, FOR_ACTION_TEMPLATE, cur_cycle.number);
				obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});
			} else {
				RAISE_ERROR("What cycle are you using??\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}

			break;
		}

		case OPCODE_IF : {
			int cur_if_cnt = ++if_cnt;
			char lname[MAX_LABEL_LEN] = {};

			COMPILE_L();

			cpl_test_reg_reg(REG_RAX, REG_RAX);
			
			cpl_je_rel32(0);
			sprintf(lname, IF_FALSE_TEMPLATE, cur_if_cnt);
			obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			regman->save_state();

			compile(node->get_R()->get_R(), file);

			regman->load_state();

			if (node->get_R()->get_L()) {
				cpl_jmp_rel32(0);
				sprintf(lname, IF_END_TEMPLATE, cur_if_cnt);
				obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});
			}

			sprintf(lname, IF_FALSE_TEMPLATE, cur_if_cnt);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			if (node->get_R()->get_L()) {
				regman->save_state();
			
				sprintf(lname, SAVE_STATE_RETR_TEMPLATE, regman->get_max_state_id());
				obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

				compile(node->get_R()->get_L(), file);

				regman->load_state();
			}

			sprintf(lname, IF_END_TEMPLATE, cur_if_cnt);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			break;
		}

		case OPCODE_FOR : {
			regman->save_state();

			int cur_for_cnt = ++for_cnt;
			cycles_end_stack.push_back(Loop(LOOP_TYPE_FOR, cur_for_cnt));
			char lname[MAX_LABEL_LEN] = {};

			if (!node->L || !node->R || !node->L->L || !node->L->R || !node->L->L->L || !node->L->L->R) {
				RAISE_ERROR("bad FOR node, something is missing\n");
				break;
			}
			
			id_table.add_scope();

			sprintf(lname, FOR_INIT_TEMPLATE, cur_for_cnt);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			compile(node->L->L->L, file);

			regman->save_state();

			sprintf(lname, FOR_START_TEMPLATE, cur_for_cnt);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			sprintf(lname, FOR_CONDITION_TEMPLATE, cur_for_cnt);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			compile(node->L->L->R, file);
			
			cpl_test_reg_reg(REG_RAX, REG_RAX);
			
			cpl_je_rel32(0);
			sprintf(lname, FOR_END_TEMPLATE, cur_for_cnt);
			obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			compile(node->R, file);

			sprintf(lname, FOR_ACTION_TEMPLATE, cur_for_cnt);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

		    cpl_expr(node->L->R, file);

		    regman->load_state();

		    cpl_jmp_rel32(0);
			sprintf(lname, FOR_CONDITION_TEMPLATE, cur_for_cnt);
			obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});
			
			sprintf(lname, FOR_END_TEMPLATE, cur_for_cnt);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			cycles_end_stack.pop_back();
			int locals_size = id_table.get_upper_offset();
			cpl_rps_add(locals_size);
			id_table.remove_scope();

			regman->load_state();
			break;
		}

		case OPCODE_ELEM_EXIT : {
			cpl_mov_reg_imm64(REG_RAX, 60);
			cpl_math_op(REG_RDI, REG_RDI, '^');
			cpl_syscall();
			break;
		}

		case OPCODE_ELEM_RANDOM : {
			cpl_mov_reg_imm64(REG_RAX, ((long long) rand() << 31) ^ rand());
			break;
		}

		case OPCODE_ELEM_INPUT : {
			if (node->R) {
				RAISE_ERROR("bad, input can't have arguments\n");
				LOG_ERROR_LINE_POS(node);
			} else {
				cpl_push_reg(REG_RSI);
				cpl_push_reg(REG_RDX);
				cpl_push_reg(REG_RDI);
				cpl_push_reg(REG_R11);

				cpl_mov_reg_imm64(REG_RAX, 0);
				cpl_mov_reg_imm64(REG_RDI, 0);
				cpl_rps_add(-8);
				cpl_mov_reg_reg(REG_RSI, REG_RSP);
				cpl_mov_reg_imm64(REG_RDX, 1);
				cpl_syscall();
				cpl_pop_reg(REG_RAX);
				cmd.put(0x48, 0x0f);
				cmd.put(0xb6, 0xc0);

				cpl_pop_reg(REG_R11);
				cpl_pop_reg(REG_RDI);
				cpl_pop_reg(REG_RDX);
				cpl_pop_reg(REG_RSI);
			}

			break;
		}

		case OPCODE_ELEM_PUTC : {
			if (node->R) {
				COMPILE_R();
			} else {
				cpl_mov_reg_imm64(REG_RAX, '\n');
			}

			cpl_push_reg(REG_R11);
			cpl_push_reg(REG_RSI);
			cpl_push_reg(REG_RDI);
			cpl_push_reg(REG_RCX);
			
			cpl_push_reg(REG_RAX);
			cpl_mov_reg_reg(REG_RSI, REG_RSP);
			cpl_mov_reg_imm64(REG_RAX, 1);
			cpl_mov_reg_imm64(REG_RDI, 1);
			cpl_mov_reg_imm64(REG_RDX, 4);
			cpl_syscall();
			cpl_pop_reg(REG_RAX);

			cpl_pop_reg(REG_RCX);
			cpl_pop_reg(REG_RDI);
			cpl_pop_reg(REG_RSI);
			cpl_pop_reg(REG_R11);

			break;
		}

		case OPCODE_ELEM_MALLOC : {
			if (node->R) {
				COMPILE_R();
				cmd.put(0x48, 0x6b); // rax = rax * 8 - long long adressing
				cmd.put(0xc0, 0x08);
				cpl_mov_reg_reg(REG_RBX, REG_RAX);
				cpl_mov_reg_mem64(REG_RAX, ELF_BSS_VADDR);
				cpl_math_op(REG_RBX, REG_RAX, '+');
				cpl_mov_mem64_reg(ELF_BSS_VADDR, REG_RBX);
			} else {
				cpl_mov_reg_mem64(REG_RAX, ELF_BSS_VADDR);
			}

			break;
		}

		// case OPCODE_ELEM_INPUT : {
		// 	fprintf(file, "in\n");

		// 	break;
		// }

		// case OPCODE_ELEM_G_INIT : {
		// 	if (!node->R || !node->L) {
		// 		RAISE_ERROR("graphics initialization is invalid without paramets");
		// 		break;
		// 	}
			
		// 	COMPILE_L();
		// 	fprintf(file, "dup\n");
		// 	fprintf(file, "pop rax\n");
		// 	COMPILE_R();
		// 	fprintf(file, "dup\n");
		// 	fprintf(file, "pop rbx\n");
		// 	fprintf(file, "g_init\n");
		// 	fprintf(file, "push rax\n");
		// 	fprintf(file, "push rbx\n");
		// 	fprintf(file, "mul\n");

		// 	break;
		// }

		// case OPCODE_ELEM_G_DRAW_TICK : {
		// 	fprintf(file, "g_draw\n");
		// 	fprintf(file, "push 0\n");

		// 	break;
		// }

		// case OPCODE_ELEM_G_PUT_PIXEL : {
		// 	if (!node->R || !node->L) {
		// 		RAISE_ERROR("graphics pixel put is invalid without paramets");
		// 		break;
		// 	}
			
		// 	COMPILE_L();
		// 	fprintf(file, "pop rax\n");
		// 	COMPILE_R();
		// 	fprintf(file, "dup\n");
		// 	fprintf(file, "pop (rax)\n");

		// 	break;
		// }

		// case OPCODE_ELEM_G_FILL : {
		// 	if (node->R) {
		// 		COMPILE_R();
		// 	} else {
		// 		fprintf(file, "push 256\n");
		// 		break;
		// 	}

		// 	fprintf(file, "dup\n");
		// 	fprintf(file, "g_fill\n");

		// 	break;
		// }

		case OPCODE_RET : {
			if (!node->R) {
				cpl_mov_reg_imm64(REG_RAX, 0);
			} else {
				COMPILE_R();
			}

			cpl_func_ret();

			break;
		}

		case OPCODE_FUNC_DECL : {
			if (!node->L) {
				RAISE_ERROR("bad func decl node, func info node id is absent\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}

			if (!node->L->R) {
				RAISE_ERROR("bad func info node, func id id is absent\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}

			StringView *id = node->L->R->get_id();
			if (id_table.find_in_upper_scope(ID_TYPE_FUNC, id) != NOT_FOUND) {
				RAISE_ERROR("Redefinition of function [");
				id->print();
				printf("]\n");
				LOG_ERROR_LINE_POS(node);
			}

			regman->flush_regs();
			_Log ANNOUNCE("WIP", "compiler", "wiping regman due to func compiling start");
			regman->wipe_state();

			char lname[MAX_LABEL_LEN] = {};
			char *func_name = id->dup();

			id_table.declare_func(id, node->L->L, id_table.size());
			int offset = id_table.find_func(id);

			cpl_jmp_rel32(0);
			sprintf(lname, FUNC_JUMPGUARD_TEMPLATE, func_name, offset);
			obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			sprintf(lname, FUNC_START_TEMPLATE, func_name, offset);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			id_table.add_scope(ARG_SCOPE);
			COMPILE_L();
			id_table.add_scope(FUNC_SCOPE);

			regman->push(REG_RBP);
			cpl_mov_reg_reg(REG_RBP, REG_RSP);

			COMPILE_R();
			cpl_func_ret();
			id_table.remove_scope();
			id_table.remove_scope();
			
			sprintf(lname, FUNC_JUMPGUARD_TEMPLATE, func_name, offset);
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			free(func_name);

			_Log ANNOUNCE("WIP", "compiler", "wiping regman due to func being compiled");
			regman->wipe_state();
			break;
		}

		case OPCODE_FUNC_INFO : {
			COMPILE_L();
			break;
		}

		case OPCODE_FUNC_ARG_DECL : {
			if (!node->L) {
				if (node->R) {
					RAISE_ERROR("bad argument node, arg is absent\n");
					LOG_ERROR_LINE_POS(node);
				}
				break;
			}

			if (node->L->is_op(OPCODE_VAR_DEF)) {
				if (!node->L->L) {
					RAISE_ERROR("bad argument node, name of arg is absent\n");
					LOG_ERROR_LINE_POS(node);
					break;
				}

				id_table.declare_var(node->L->L->get_id(), 1);
			} else if (node->L->is_id()) {
				id_table.declare_var(node->L->get_id(), 1);
			} else {
				RAISE_ERROR("bad agrument node, unknown type\n");
				LOG_ERROR_LINE_POS(node);
				break;
			}

			COMPILE_R();
			
			break;
		}

		case OPCODE_FUNC_CALL : {
			if (id_table.find_func(node->R->get_id()) == NOT_FOUND) {
			    cpl_arr_rvalue(node, file);
			    // RAISE_ERROR("NO ARRAYS YET MAN\n");
			    // LOG_ERROR_LINE_POS(node);
			} else {
				_log ANNOUNCE("CAL", "compiler", "calling func from line %d\n", node->line);
			    cpl_func_call(node, file);
			    _log ANNOUNCE("===", "compiler", "done func from line %d\n", node->line);
			}
			break;
		}

		case '{' : {
			id_table.add_scope();
			COMPILE_L_COMMENT();
			int locals_size = id_table.get_upper_offset();
			cpl_rps_add(locals_size);
			id_table.remove_scope();
			COMPILE_R_COMMENT();

			break;
		}

		case ';' : {
			COMPILE_L_COMMENT();
			COMPILE_R_COMMENT();

			break;
		}

		default : {
			RAISE_ERROR("bad operation: [");
			node->space_dump();
			printf("]\n");
			LOG_ERROR_LINE_POS(node);
			break;
		}
	}
}

void Compiler::cpl_expr(const CodeNode *node, FILE *file, const bool to_pop) {
	if (node->is_op(OPCODE_EXPR)) {
		COMPILE_L();
	} else {
		compile(node, file);
	}
}

void Compiler::cpl_rps_add(const int imm32) {
	cmd.put(0x48);
	cmd.put(0x81);
	cmd.put(0xc4);
	cmd.put((byte*) &imm32, 4);
}

void Compiler::cpl_func_ret() {
	regman->flush_regs(REGMAN_GLOBALS, false);

	int locals_size = id_table.get_func_locals_size();
	cpl_rps_add(locals_size);

	regman->pop(REG_RBP);
	cpl_ret();
}

void Compiler::cpl_calloc(const int size) {
	cpl_mov_reg_mem64(REG_RAX, ELF_BSS_VADDR);
	cpl_mov_reg_imm64(REG_RBX, size);
	cpl_math_op(REG_RBX, REG_RAX, '+');
	cpl_mov_mem64_reg(ELF_BSS_VADDR, REG_RBX);
}

void Compiler::cpl_func_call(const CodeNode *node, FILE *file) {
	assert(node);
	assert(file);

	if (false && !node->L) {
		RAISE_ERROR("bad func call, arglist is absent\n");
		LOG_ERROR_LINE_POS(node);
		return;
	}

	const StringView *id = nullptr;
	if (!node->R) {
		if (node->is_id()) {
			id = node->get_id();
		} else {
			RAISE_ERROR("bad func call, func name is absent\n");
			LOG_ERROR_LINE_POS(node);
			return;
		}
	} else {
		if (!node->R->is_id()) {
			RAISE_ERROR("bad func call, func name is not a name lol\n");
			LOG_ERROR_LINE_POS(node);
			return;
		}
		id = node->R->get_id();
	}

	const CodeNode *arglist = node->L;

	int func_offset = 0;
	if ((func_offset = id_table.find_func(id)) == NOT_FOUND) {
		RAISE_ERROR("bad func call, func not declared [");
		id->print();
		printf("]\n");
		LOG_ERROR_LINE_POS(node);
		return;
	}

	const CodeNode *func_arglist = id_table.get_arglist(id);
	if (!func_arglist) {
		RAISE_ERROR("bad func call, declared func arglist is absent\n");
		LOG_ERROR_LINE_POS(node);
		return;
	}

 	//=====================================================================
 	// here we definetly will compile a function

	regman->flush_regs(REGMAN_TMPS, false);

	id_table.add_scope();

	int i = 1;
	int args_offset = 0;

	while (arglist && func_arglist && arglist->L && func_arglist->L) {
		const CodeNode *arg  = arglist->L;
		const CodeNode *prot = func_arglist->L;

		if (arg->is_op(OPCODE_DEFAULT_ARG)) {
		    cpl_default_arg(arg, prot, file);
		} else if (arg->is_op(OPCODE_CONTEXT_ARG)) {
		    cpl_context_arg(arg, prot, file);
		} else if (arg->is_op(OPCODE_EXPR)) {
		    cpl_expr_arg(arg, prot, file);
		}

		++args_offset;

		arglist = arglist->R;
		func_arglist = func_arglist->R;
		CHECK_ERROR();
	}

	while (func_arglist && func_arglist->L) {
	    cpl_default_arg(node, func_arglist->L, file);
		func_arglist = func_arglist->R;
		CHECK_ERROR();
	}

	args_offset *= 8;

	id_table.remove_scope();

	regman->flush_regs(REGMAN_VARS, false);

	char lname[MAX_LABEL_LEN] = {};
	char *func_name = id->dup();

	cpl_call_rel32(0);
	sprintf(lname, FUNC_START_TEMPLATE, func_name, func_offset);
	obj.request_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

	regman->alter_rsp(args_offset);
	regman->rest_state();

	free(func_name);
}

void Compiler::cpl_default_arg(const CodeNode *arg, const CodeNode *prot, FILE *file) {
	if (prot->is_id()) {
		id_table.shift_backward();
		cpl_rvalue(prot);
		CHECK_ERROR();
		regman->push(REG_RAX);
		id_table.shift_forward();
	} else if (prot->is_op(OPCODE_VAR_DEF)) {
		if (!prot->R) {
			RAISE_ERROR("bad func call, required arg [");
			prot->L->get_id()->print();
			printf("] has no default set\n");
			LOG_ERROR_LINE_POS(arg);
			return;
		}

		id_table.shift_backward();
		compile(prot->R, file);
		regman->push(REG_RAX);
		id_table.shift_forward();
	} else {
		RAISE_ERROR("bad func call, unexpected PROT type [");
		printf("%d]\n", prot->get_op());
		LOG_ERROR_LINE_POS(prot);
		return;
	}
}

void Compiler::cpl_context_arg(const CodeNode *arg, const CodeNode *prot, FILE *file) {
	if (prot->is_id()) {
		id_table.shift_backward();
		cpl_rvalue(prot);
		CHECK_ERROR();
		regman->push(REG_RAX);
		id_table.shift_forward();
	} else if (prot->is_op(OPCODE_VAR_DEF)) {
		id_table.shift_backward();
		cpl_rvalue(prot->L);
		CHECK_ERROR();
		regman->push(REG_RAX);
		id_table.shift_forward();
	} else {
		RAISE_ERROR("bad func call, unexpected PROT type [");
		printf("%d]\n", prot->get_op());
		LOG_ERROR_LINE_POS(arg);
		return;
	}
}

void Compiler::cpl_expr_arg(const CodeNode *arg, const CodeNode *prot, FILE *file) {
	if (!arg->L) {
		RAISE_ERROR("bad func call, expr node has no expression inside\n");
		LOG_ERROR_LINE_POS(arg);
		return;
	}

	CodeNode *expr = arg->L;
	id_table.shift_backward();
	compile(expr, file);
	regman->push(REG_RAX);
	id_table.shift_forward();
}

int found_to_var_type(int found) {
	switch (found) {
		case ID_TYPE_FOUND:
			return REGMAN_VAR_LOCAL;
		case ID_TYPE_GLOBAL:
			return REGMAN_VAR_GLOBAL;
		default:
			return NOT_FOUND;
	}
}

void Compiler::cpl_arr_rvalue(const CodeNode *node, FILE *file) {
	if (!node->R) {
		RAISE_ERROR("bad arr_rvalue, where is name, you are worthless [");
		printf("%d]\n", node->get_op());
		LOG_ERROR_LINE_POS(node);
		return;
	}

	CodeNode *id = node->R;
	CodeNode *args = node->L;

	int offset = 0;
	int found = id_table.find_var(id->get_id(), &offset);
	if (found == NOT_FOUND) {
		RAISE_ERROR("variable does not exist [");
		id->get_id()->print();
		printf("]\n");
		LOG_ERROR_LINE_POS(node);
		return;
	}

	char *vname = id->get_id()->dup();
	int r1 = regman->get_var_reg(offset, found_to_var_type(found), vname);
	cpl_mov_reg_reg(REG_RBX, r1);
	regman->release_var_reg(r1);

	while (args && args->L) {
		CodeNode *arg = args->L;
	    cpl_expr(arg, file);

	    cmd.put(0x48, 0x6b); // rax = rax * 8 - long long adressing
		cmd.put(0xc0, 0x08);

	    cpl_math_op(REG_RAX, REG_RBX, '+');
		
		cpl_mov_reg_mem(REG_RBX, REG_RAX, 0);
		args = args->R;
	}

	cpl_mov_reg_reg(REG_RAX, REG_RBX);

	free(vname);
}

void Compiler::cpl_arr_lvalue(const CodeNode *node, FILE *file) {
	if (!node->R) {
		RAISE_ERROR("bad arr_lvalue, where is name, you are worthless [");
		printf("%d]\n", node->get_op());
		LOG_ERROR_LINE_POS(node);
		return;
	}

	CodeNode *id = node->R;
	CodeNode *args = node->L;

	int offset = 0;
	int found = id_table.find_var(id->get_id(), &offset);
	if (found == NOT_FOUND) {
		RAISE_ERROR("variable does not exist [");
		id->get_id()->print();
		printf("]\n");
		LOG_ERROR_LINE_POS(node);
		return;
	}

	char *vname = id->get_id()->dup();
	_log regman->dump();
	int r1 = regman->get_var_reg(offset, found_to_var_type(found), vname);
	cpl_mov_reg_reg(REG_RBX, r1);
	_log regman->release_var_reg(r1);

	while (args && args->L) {
		CodeNode *arg = args->L;
	    cpl_expr(arg, file);

	    cmd.put(0x48, 0x6b); // rax = rax * 8 - long long adressing
		cmd.put(0xc0, 0x08);

	    cpl_math_op(REG_RAX, REG_RBX, '+');
		
		args = args->R;
		if (args && args->L) {
			cpl_mov_reg_mem(REG_RBX, REG_RAX, 0);
		}
	}

	free(vname);
}

// bool Compiler::cpl_push(const CodeNode *node, FILE *file) {
// 	assert(node);
// 	assert(file);

// 	fprintf(file, "push ");
// 	bool result = false;
// 	if (node->type == VALUE) {
// 		if (node->get_val() < 0) {
// 			fprintf(file, "0\npush ");
			
// 			CodeNode tmp = {};
// 			tmp.ctor(VALUE, fabs(node->get_val()), nullptr, nullptr, node->line, node->pos);
// 		    cpl_value(&tmp, file);
// 			tmp.dtor();

// 			fprintf(file, "\nsub\n");
// 		} else {
// 			result = cpl_value(node, file);
// 		}
// 	} else if (node->type == ID) {
// 		result = cpl_lvalue(node, file, false, false, true);
// 	}
// 	fprintf(file, "\n");
// 	return result;
// }

bool Compiler::cpl_value(const CodeNode *node, FILE *file) {
	assert(node);
	assert(file);

	cpl_mov_reg_imm64(REG_RAX, node->get_val());
	return true;
}

bool Compiler::cpl_rvalue(const CodeNode *node) {
	if (node->is_id()) {
		char *vname = node->get_id() ? node->get_id()->dup() : nullptr;
		_log ANNOUNCE("RVL", "cpl_rvalue", "searching for var[%s] in id_table:", vname);
		if (vname) free(vname);

		_log ANNOUNCE("DMP", "id_table", "==================");
		_log id_table.dump(THE_LAST_ANNOUNCER_LEN + THE_LAST_CODE_LEN + 5);

		int offset = 0;
		int found = id_table.find_var(node->get_id(), &offset);

		_log ANNOUNCE_NOCODE("found = %d", found);
		_log ANNOUNCE_NOCODE("offst = %d", offset);
		_log ANNOUNCE("===", "id_table", "==================");

		if (found == NOT_FOUND) {
			RAISE_ERROR("can't find varname [");
			node->get_id()->print();
			printf("]\n");
			LOG_ERROR_LINE_POS(node);
			return false;
		}

		char *name = node->get_id()->dup();
		int r1 = regman->get_var_reg(offset, found_to_var_type(found), name);
		free(name);

		_log ANNOUNCE("RVL", "cpl_rvalue", "moving from rvalue to rax");
		cpl_mov_reg_reg(REG_RAX, r1);
		regman->release_var_reg(r1);
	} else {
		RAISE_ERROR("bad rvalue node found\n");
		LOG_ERROR_LINE_POS(node);
		return false;
	}

	return true;
}

bool Compiler::cpl_lvalue(const CodeNode *node, int &offset, int &found, char is_initialization) {
	assert(node);

	if (node->is_id()) {
		if (node->get_id()->starts_with("_") && !node->get_id()->starts_with("_)") && !is_initialization) {
			RAISE_ERROR("_varname is a constant, dont change it please: [");
			node->get_id()->print();
			printf("]\n");
			LOG_ERROR_LINE_POS(node);
		}
		// printf("\n\n==================\n");
		// printf("compile var |");
		// node->get_id()->print();
		// printf("|\n");
		// printf("~~~~~~~~~~~~~~~~~~\n");
		// printf("cur id_table:\n");
		// id_table.dump();

		// printf("finding [");
		// node->get_id()->print();
		// printf("]\n");

		int is_found = id_table.find_var(node->get_id(), &offset);
		found = found_to_var_type(is_found);

		// printf("\nfound = %d", offset);
		// printf("\n==================\n");



		if (is_found == NOT_FOUND) {
			RAISE_ERROR("variable does not exist [");
			node->get_id()->print();
			printf("]\n");
			LOG_ERROR_LINE_POS(node);

			return false;
		}

		// if (is_found == ID_TYPE_GLOBAL) {
		// 	fprintf(file, "[%d]\n", GLOBAL_VARS_OFFSET + offset);
		// } else {
		// 	fprintf(file, "[rvx + %d]", offset);
		// }
		return true;
	} else if (node->is_op(OPCODE_FUNC_CALL) && id_table.find_func(node->R->get_id()) == NOT_FOUND) { // that's an array
		// CodeNode *id  = node->R;
		// CodeNode *args = node->L;

		// int offset = 0;
		// int ret = id_table.find_var(id->get_id(), &offset);
		// if (ret == NOT_FOUND) {
		// 	RAISE_ERROR("variable does not exist [");
		// 	id->get_id()->print();
		// 	printf("]\n");
		// 	LOG_ERROR_LINE_POS(node);
		// 	return false;
		// }

		// // we are called by assign, so there's 'pop ' already written in assembler, let's fix it
		// if (for_asgn_dup) {
		// 	fprintf(file, "[rcx]\n");
		// 	//fprintf(file, "pop rzx\n");
		// 	// fprintf(file, "push [rax + %d + 1]\n", offset);
		// 	return true;
		// }

		// if (to_push) {
		// 	fprintf(file, "0\n");
		// 	fprintf(file, "pop rzx\n");
		// } else {
		// 	fprintf(file, "rbx\n");
		// 	fprintf(file, "push rbx\n");
		// }

		// fprintf(file, "push rvx + %d\n", offset);
		// fprintf(file, "pop rax\n");
		// while (args && args->L) {
		// 	fprintf(file, "push [rax]\n");
		// 	CodeNode *arg = args->L;
		//     cpl_expr(arg, file);
		// 	// TODO wtf is this... it works... so let it be... for 2d arrs... but not anyhow more...
		// 	//if (args->R->L) {
		// 		fprintf(file, "push 1\n");
		// 		fprintf(file, "add\n");
		// 	//}
		// 	// -------------------------------------------------------------------------------------
		// 	fprintf(file, "add\n");
		// 	fprintf(file, "pop rax\n");
		// 	//fprintf(file, "push [rax]\n");
		// 	args = args->R;
		// };

		// fprintf(file, "push rax\n");
		// fprintf(file, "pop rcx\n");
		// if (to_push) {
		// 	fprintf(file, "push [rax]\n");
		// } else {
		// 	fprintf(file, "pop [rax]\n");
		// }
		// return true;
	} else {
		RAISE_ERROR("bad compiling type, node is [%d]\n", node->get_type());
		LOG_ERROR_LINE_POS(node);
		return false;
	}

	offset = -0x111111;
	found = NOT_FOUND;
	return false;
}

void Compiler::cpl_id(const CodeNode *node, FILE *file) {
	assert(node);
	assert(file);

	fprintf(file, "[%d]", node->get_var_from_id());
}

void Compiler::compile(const CodeNode *node, FILE *file) {
	if (!node) {
		return;
	}
	CHECK_ERROR();

	//node->gv_dump();

	switch (node->type) {
		case VALUE : {
		    cpl_value(node, file);
			break;
		}

		case OPERATION : {
		    cpl_operation(node, file);
			break;
		}

		case VARIABLE : {
		    cpl_rvalue(node);
		    CHECK_ERROR();
			break;
		}

		case ID : {
			if (id_table.find_func(node->get_id()) != NOT_FOUND) {
			    cpl_func_call(node, file);
			} else if (node->R) {
			    cpl_arr_rvalue(node, file);
			} else {
			    cpl_rvalue(node);
			    CHECK_ERROR();
			}
			break;
		}

		default: {
			printf("A strange node detected... [");
			node->space_dump();
			printf("]\n");
			return;
		}
	}

	CHECK_ERROR();
}

Compiler::Compiler():
prog_text(nullptr),
rec_parser(),
lex_parser(),
id_table(),
cycles_end_stack(),
if_cnt(0),
while_cnt(0),
for_cnt(0),
// regstack(),
cmd()
{}

Compiler::~Compiler() {}

void Compiler::ctor() {
	prog_text = nullptr;
	rec_parser.ctor();
	lex_parser.ctor();

	cycles_end_stack.ctor();

	if_cnt    = 0;
	while_cnt = 0;
	for_cnt   = 0;

	regman = RegManager::NEW(this);
	obj.ctor();
}

Compiler *Compiler::NEW() {
	Compiler *cake = (Compiler*) calloc(1, sizeof(Compiler));
	if (!cake) {
		return nullptr;
	}

	cake->ctor();
	return cake;
}

void Compiler::dtor() {
	id_table.dtor();
	obj.dtor();
	RegManager::DELETE(regman);
}

void Compiler::DELETE(Compiler *compiler) {
	if (!compiler) {
		return;
	}

	compiler->dtor();
	free(compiler);
}

//=============================================================================

CodeNode *Compiler::read_to_nodes(const File *file) {
	Vector<Token> *tokens = lex_parser.parse(file->data);
	// for (size_t i = 0; i < tokens->size(); ++i) {
	// 	(*tokens)[i].dump(stdout, false);
	// 	printf(" ");
	// }
	// printf("\n");

	CodeNode *ret = rec_parser.parse(tokens);

	Vector<Token>::DELETE(tokens);

	return ret;
}

bool Compiler::compile(const CodeNode *prog, const char *filename) {
	if (filename == nullptr) {
		RAISE_ERROR("[filename](nullptr)\n");
		return false;
	}

	FILE *file = fopen(filename, "w");
	if (!file) {
		RAISE_ERROR("[filename](%s) can't be opened\n", filename);
		return false;
	}

	if_cnt    = 0;
	while_cnt = 0;
	for_cnt   = 0;
	id_table.dtor();
	id_table.ctor();
	cycles_end_stack.dtor();
	cycles_end_stack.ctor();

	obj.add_fixup({MEMVAR, ELF_BSS_VADDR, fxp_FIXED});

	cpl_mov_reg_imm64(REG_RAX, ELF_BSS_VADDR + sizeof(long long));
	cpl_mov_mem64_reg(ELF_BSS_VADDR, REG_RAX);

	compile(prog, file);
	fclose(file);

	cpl_mov_reg_imm64(REG_RAX, 60);
	cpl_math_op(REG_RDI, REG_RDI, '^');
	cpl_syscall();

	if (ANNOUNCEMENT_ERROR) {
		fprintf(file, "AN ERROR OCCURED DURING COMPILATION IUCK\n");
		ANNOUNCE("ERR", "kncc", "An error occured during compilation");
		return false;
	}

	MicroLinker linker;
	linker.ctor();

	Vector<MicroObj*> objs = {};
	objs.ctor();

	obj.set_prog((byte*) cmd.get_data(), cmd.get_size());

	objs.push_back(&obj);

	ByteBuffer result_cmd;

	linker.link_objectives(objs, 0, filename, &result_cmd);
	_log result_cmd.hexdump();

	char chmod_exec_command[100];
	sprintf(chmod_exec_command, "chmod +x %s", filename);
	system(chmod_exec_command);
	
	return true;
}

#undef LOG_ERROR_LINE_POS
#undef DUMP_L
#undef DUMP_R
#undef COMPILE_L
#undef COMPILE_R
#undef COMPILE_L_COMMENT
#undef COMPILE_R_COMMENT
#undef COMPILE_LR
#undef CHECK_ERROR
