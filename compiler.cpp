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
	cmd.put(cmd_MOV_TABLE[reg_dst][reg_src], 3);
}

void Compiler::cpl_mov_mem_reg(const int reg_dst, const int offset, const int reg_src) {
	if (reg_dst == REG_RSP) {
		cmd.put(cmd_MOV_RSPMEM_REG_DISPL32[reg_src], 4);
		cmd.put((byte*) &offset, 4);
	} else if (reg_dst == REG_RBP) {
		cmd.put(cmd_MOV_RBPMEM_REG_DISPL32[reg_src], 3);
		cmd.put((byte*) &offset, 4);
	} else {
		ANNOUNCE("ERR", __FUNCTION__, "invalid register to adress");
	}
}

void Compiler::cpl_mov_reg_mem(const int reg_dst, const int reg_src, const int offset) {
	if (reg_src == REG_RSP) {
		cmd.put(cmd_MOV_REG_RSPMEM_DISPL32[reg_dst], 4);
		cmd.put((byte*) &offset, 4);
	} else if (reg_src == REG_RBP) {
		cmd.put(cmd_MOV_REG_RBPMEM_DISPL32[reg_dst], 3);
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
	cmd.put(cmd_PUSH_TABLE[reg_src], 2);
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

	switch (node->get_op()) {
		case '+' :
		case '*' :
		case '^' :
		
		 {
			if (node->L) {
				COMPILE_MORE_COMPLEX();

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
					int r1 = regman->get_tmp_reg();

					cpl_mov_reg_imm64(r1, -1);
					cpl_math_op(REG_RAX, r1, '*');

					regman->release_tmp_reg(r1);
				} else {
					LOG_ERROR_LINE_POS(node);
				}
			}
			break;
		}

		case '=' : {
			COMPILE_R();

			int offset, found;
			cpl_lvalue(node->L, offset, found);
			if (found == NOT_FOUND) {
				RAISE_ERROR("can't find variable, sry\n");
				LOG_ERROR_LINE_POS(node);
				return;
			}

			int r1 = 0;
			char *name = node->L->get_id()->dup();
			r1 = regman->get_var_reg(offset, found == ID_TYPE_GLOBAL ? REGMAN_VAR_GLOBAL : REGMAN_VAR_LOCAL, name, true);
			free(name);

			cpl_mov_reg_reg(r1, REG_RAX);
			regman->release_var_reg(r1);

			break;
		}

		case OPCODE_ASGN_ADD :
		case OPCODE_ASGN_SUB :
		case OPCODE_ASGN_MUL :
		case OPCODE_ASGN_DIV :
		case OPCODE_ASGN_POW : {
			COMPILE_R();

			int offset, found;
			cpl_lvalue(node->L, offset, found);
			if (found == NOT_FOUND) {
				RAISE_ERROR("can't find variable, sry\n");
				LOG_ERROR_LINE_POS(node);
				return;
			}

			int r1 = 0;
			char *name = node->L->get_id()->dup();
			r1 = regman->get_var_reg(offset, found == ID_TYPE_GLOBAL ? REGMAN_VAR_GLOBAL : REGMAN_VAR_LOCAL, name);
			free(name);

			int op = asgn_op_to_op(node->get_op());

			if (op != '/') {
				cpl_math_op(r1, REG_RAX, op);
			} else {
				cpl_xchg_rax_reg(r1);
				cpl_math_op(REG_RAX, r1, op);
				cpl_mov_reg_reg(r1, REG_RAX);
			}
			regman->release_var_reg(r1);

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
			cpl_lvalue(node->L, offset, found);
			char *name = node->L->get_id()->dup();

			if (!found) {
				printf("WTFWTFWTF\n");
				return;
			}

			if (found == ID_TYPE_GLOBAL) {
				obj.add_fixup({name, offset, fxp_ABSOLUTE, sizeof(long long)});
			}

			if (node->R) {
				COMPILE_R();

				int r1 = regman->get_var_reg(offset, found == ID_TYPE_GLOBAL ? REGMAN_VAR_GLOBAL : REGMAN_VAR_LOCAL, name, true);
			    cpl_mov_reg_reg(r1, REG_RAX);
			    regman->release_var_reg(r1);
			}

			free(name);

			break;
		}

		// case OPCODE_ARR_DEF : {
		// 	CodeNode *arr_name = node->L->R;
		// 	if (!node->L || !node->L->is_op(OPCODE_ARR_INFO)) {
		// 		RAISE_ERROR("bad variable definition [\n");
		// 		node->space_dump();
		// 		printf("]\n");
		// 		LOG_ERROR_LINE_POS(node);
		// 		break;
		// 	}
			
		// 	bool ret = id_table.declare_var(arr_name->get_id(), 1);
		// 	if (!ret) {
		// 		RAISE_ERROR("Redefinition of the id [");
		// 		arr_name->get_id()->print();
		// 		printf("]\n");
		// 		LOG_ERROR_LINE_POS(node);
		// 		break;
		// 	}
		// 	id_table.add_buffer_zone((int) node->L->L->get_val());

		// 	int offset = 0;
		// 	id_table.find_var(arr_name->get_id(), &offset);
		// 	fprintf(file, "push rvx + %d\n", offset);
		// 	//fprintf(file, "add\n");
		// 	// fprintf(file, "dup\n");
		// 	// fprintf(file, "out\n");
		// 	// fprintf(file, "out_n\n");
		// 	fprintf(file, "pop [rvx + %d]\n", offset);

		// 	break;
		// }

		// case OPCODE_WHILE : {
		// 	int cur_while_cnt = ++while_cnt;

		// 	cycles_end_stack.push_back(Loop(LOOP_TYPE_WHILE, cur_while_cnt));
		// 	fprintf(file, "while_%d_cond:\n", cur_while_cnt);

		// 	COMPILE_L();
		// 	fprintf(file, "\npush 0\n");
		// 	fprintf(file, "je while_%d_end\n", cur_while_cnt);

		// 	COMPILE_R();
		// 	fprintf(file, "jmp while_%d_cond\n", cur_while_cnt);

		// 	fprintf(file, "\nwhile_%d_end:\n", cur_while_cnt);
		// 	cycles_end_stack.pop_back();
		// 	break;
		// }

		// case OPCODE_BREAK : {
		// 	if (!cycles_end_stack.size()) {
		// 		RAISE_ERROR("You can't use |< outside of the loop\n");
		// 		LOG_ERROR_LINE_POS(node);
		// 		break;
		// 	}

		// 	Loop cur_cycle = cycles_end_stack[cycles_end_stack.size() - 1];
		// 	if (cur_cycle.type == LOOP_TYPE_WHILE) {
		// 		fprintf(file, "jmp while_%d_end\n", cur_cycle.number);
		// 	} else if (cur_cycle.type == LOOP_TYPE_FOR) {
		// 		fprintf(file, "jmp for_%d_end\n", cur_cycle.number);
		// 	} else {
		// 		RAISE_ERROR("What cycle are you using??\n");
		// 		LOG_ERROR_LINE_POS(node);
		// 		break;
		// 	}

		// 	break;
		// }

		// case OPCODE_CONTINUE : {
		// 	if (!cycles_end_stack.size()) {
		// 		RAISE_ERROR("You can't use |< outside of the loop\n");
		// 		LOG_ERROR_LINE_POS(node);
		// 		break;
		// 	}

		// 	Loop cur_cycle = cycles_end_stack[cycles_end_stack.size() - 1];
		// 	if (cur_cycle.type == LOOP_TYPE_WHILE) {
		// 		fprintf(file, "jmp while_%d_cond\n", cur_cycle.number);
		// 	} else if (cur_cycle.type == LOOP_TYPE_FOR) {
		// 		fprintf(file, "jmp for_%d_action\n", cur_cycle.number);
		// 	} else {
		// 		RAISE_ERROR("What cycle are you using??\n");
		// 		LOG_ERROR_LINE_POS(node);
		// 		break;
		// 	}

		// 	break;
		// }

		case OPCODE_IF : {
			regman->save_state();

			char lname[MAX_LABEL_LEN];
			sprintf(lname, SAVE_STATE_RETR_TEMPLATE, regman->get_max_state_id());
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			compile(node->get_R()->get_R(), file);

			sprintf(lname, LOAD_STATE_RETR_TEMPLATE, regman->get_max_state_id());
			obj.add_fixup({lname, (int) cmd.get_size() - 4, fxp_RELATIVE});

			regman->corrupt_reg(0);
			regman->load_state();

			// int cur_if_cnt = ++if_cnt;
			// fprintf(file, "if_%d_cond:\n", cur_if_cnt);
			// fprintf(file, "\npush 0\n");
			// fprintf(file, "jne if_%d_true\n", cur_if_cnt);
			// COMPILE_R();
			// fprintf(file, "\nif_%d_end:\n", cur_if_cnt);
			break;
		}

		// case OPCODE_FOR : {
		// 	int cur_for_cnt = ++for_cnt;
		// 	cycles_end_stack.push_back(Loop(LOOP_TYPE_FOR, cur_for_cnt));

		// 	if (!node->L || !node->R || !node->L->L || !node->L->R || !node->L->L->L || !node->L->L->R) {
		// 		RAISE_ERROR("bad for node, something is missing\n");
		// 		break;
		// 	}
			
		// 	id_table.add_scope();
		// 	fprintf(file, "\nfor_%d_init_block:\n", cur_for_cnt);
		// 	compile(node->L->L->L, file);

		// 	fprintf(file, "\nfor_%d_start:\n", cur_for_cnt);
		// 	fprintf(file, "\nfor_%d_cond:\n", cur_for_cnt);
		// 	compile(node->L->L->R, file);
		// 	fprintf(file, "\npush 0\n");
		// 	fprintf(file, "je for_%d_end\n", cur_for_cnt);

		// 	compile(node->R, file);

		// 	fprintf(file, "for_%d_action:\n", cur_for_cnt);
		//     cpl_expr(node->L->R, file, true);
		// 	fprintf(file, "jmp for_%d_cond\n", cur_for_cnt);
			
		// 	fprintf(file, "\nfor_%d_end:\n", cur_for_cnt);
		// 	cycles_end_stack.pop_back();
		// 	id_table.remove_scope();
		// 	break;
		// }

		// case OPCODE_COND_DEPENDENT : {
		// 	int cur_if_cnt = if_cnt;
		// 	fprintf(file, "if_%d_false:\n", cur_if_cnt);
		// 	COMPILE_L_COMMENT();
		// 	fprintf(file, "\njmp if_%d_end\n", cur_if_cnt);
		// 	fprintf(file, "\nif_%d_true:\n", cur_if_cnt);
		// 	COMPILE_R_COMMENT();
		// 	break;
		// }

		// case OPCODE_ELEM_EXIT : {
		// 	fprintf(file, "halt\n");
		// 	break;
		// }

		// case OPCODE_ELEM_RANDOM : {
		// 	//fprintf(file, "push\n");
		// 	COMPILE_L();
		// 	COMPILE_R();
		// 	fprintf(file, "bin_op $\n");
		// 	break;
		// }

		// case OPCODE_ELEM_PUTN : {
		// 	if (node->R) {
		// 		COMPILE_R();
		// 		fprintf(file, "dup\n");
		// 		fprintf(file, "out\n");
		// 	} else {
		// 		fprintf(file, "push %d\n", '\n');
		// 		fprintf(file, "dup\n");
		// 		fprintf(file, "out_c\n");
		// 	}

		// 	break;
		// }

		// case OPCODE_ELEM_PUTC : {
		// 	if (node->R) {
		// 		COMPILE_R();
		// 	} else {
		// 		fprintf(file, "push %d\n", ' ');
		// 	}
		// 	fprintf(file, "dup\n");
		// 	fprintf(file, "out_c\n");

		// 	break;
		// }

		// case OPCODE_ELEM_MALLOC : {
		// 	if (node->R) {
		// 		COMPILE_R();
		// 	} else {
		// 		fprintf(file, "push rmx\n");
		// 		break;
		// 	}
			
		// 	fprintf(file, "pop rax\n");
		// 	fprintf(file, "push rmx\n");
		// 	fprintf(file, "push rmx\n");
		// 	fprintf(file, "push rax\n");
		// 	fprintf(file, "add\n");
		// 	fprintf(file, "pop rmx\n");

		// 	break;
		// }

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

		// case OPCODE_RET : {
		// 	if (!node->R) {
		// 		fprintf(file, "push 0\n");
		// 	} else {
		// 		COMPILE_R();
		// 	}

		// 	fprintf(file, "swp\n");
		// 	fprintf(file, "ret\n");

		// 	break;
		// }

		// case OPCODE_FUNC_DECL : {
		// 	if (!node->L) {
		// 		RAISE_ERROR("bad func decl node, func info node id is absent\n");
		// 		LOG_ERROR_LINE_POS(node);
		// 		break;
		// 	}

		// 	if (!node->L->R) {
		// 		RAISE_ERROR("bad func info node, func id id is absent\n");
		// 		LOG_ERROR_LINE_POS(node);
		// 		break;
		// 	}

		// 	StringView *id = node->L->R->get_id();
		// 	if (id_table.find_in_upper_scope(ID_TYPE_FUNC, id) != NOT_FOUND) {
		// 		RAISE_ERROR("Redifenition of function [");
		// 		id->print();
		// 		printf("]\n");
		// 		LOG_ERROR_LINE_POS(node);
		// 	}

		// 	id_table.declare_func(id, node->L->L, id_table.size());
		// 	int offset = id_table.find_func(id);

		// 	fprintf(file, "jmp _func_");
		// 	id->print(file);
		// 	fprintf(file, "_%d_END\n", offset);
		// 	fprintf(file, "_func_");
		// 	id->print(file);
		// 	fprintf(file, "_%d_BEGIN:\n", offset);

		// 	id_table.add_scope(FUNC_SCOPE);
		// 	COMPILE_L();
		// 	COMPILE_R();
		// 	id_table.remove_scope();
		// 	fprintf(file, "push 0\n");
		// 	fprintf(file, "swp\n");
		// 	fprintf(file, "ret\n");
		// 	fprintf(file, "_func_");
		// 	id->print(file);
		// 	fprintf(file, "_%d_END:\n", offset);
		// 	break;
		// }

		// case OPCODE_FUNC_INFO : {
		// 	COMPILE_L();

		// 	node->R->get_id()->print(file);
		// 	fprintf(file, "_%d:\n", id_table.find_func(node->R->get_id()));
		// 	break;
		// }

		// case OPCODE_FUNC_ARG_DECL : {
		// 	if (!node->L) {
		// 		if (node->R) {
		// 			RAISE_ERROR("bad argument node, arg is absent\n");
		// 			LOG_ERROR_LINE_POS(node);
		// 		}
		// 		break;
		// 	}

		// 	if (node->L->is_op(OPCODE_VAR_DEF)) {
		// 		if (!node->L->L) {
		// 			RAISE_ERROR("bad argument node, name of arg is absent\n");
		// 			LOG_ERROR_LINE_POS(node);
		// 			break;
		// 		}

		// 		id_table.declare_var(node->L->L->get_id(), 1);
		// 	} else if (node->L->is_id()) {
		// 		id_table.declare_var(node->L->get_id(), 1);
		// 	} 

		// 	COMPILE_R();
		// 	break;
		// }

		// case OPCODE_FUNC_CALL : {
		// 	if (id_table.find_func(node->R->get_id()) == NOT_FOUND) {
		// 	    cpl_arr_call(node, file);
		// 	} else {
		// 	    cpl_func_call(node, file);
		// 	}
		// 	break;
		// }

		case '{' : {
			id_table.add_scope();
			COMPILE_L_COMMENT();
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

// void Compiler::cpl_func_call(const CodeNode *node, FILE *file) {
// 	assert(node);
// 	assert(file);

// 	if (false && !node->L) {
// 		RAISE_ERROR("bad func call, arglist is absent\n");
// 		LOG_ERROR_LINE_POS(node);
// 		return;
// 	}

// 	const StringView *id = nullptr;
// 	if (!node->R) {
// 		if (node->is_id()) {
// 			id = node->get_id();
// 		} else {
// 			RAISE_ERROR("bad func call, func name is absent\n");
// 			LOG_ERROR_LINE_POS(node);
// 			return;
// 		}
// 	} else {
// 		if (!node->R->is_id()) {
// 			RAISE_ERROR("bad func call, func name is not a name lol\n");
// 			LOG_ERROR_LINE_POS(node);
// 			return;
// 		}
// 		id = node->R->get_id();
// 	}

// 	const CodeNode *arglist = node->L;

// 	int func_offset = 0;
// 	if ((func_offset = id_table.find_func(id)) == NOT_FOUND) {
// 		RAISE_ERROR("bad func call, func not declared [");
// 		id->print();
// 		printf("]\n");
// 		LOG_ERROR_LINE_POS(node);
// 		return;
// 	}

// 	const CodeNode *func_arglist = id_table.get_arglist(id);
// 	if (!func_arglist) {
// 		RAISE_ERROR("bad func call, declared func arglist is absent\n");
// 		LOG_ERROR_LINE_POS(node);
// 		return;
// 	}

// 	//=====================================================================
// 	// here we definetly will compile a function

// 	id_table.add_scope(ARG_SCOPE);

// 	while (arglist && func_arglist && arglist->L && func_arglist->L) {
// 		const CodeNode *arg  = arglist->L;
// 		const CodeNode *prot = func_arglist->L;

// 		if (arg->is_op(OPCODE_DEFAULT_ARG)) {
// 		    cpl_default_arg(arg, prot, file);
// 		} else if (arg->is_op(OPCODE_CONTEXT_ARG)) {
// 		    cpl_context_arg(arg, prot, file);
// 		} else if (arg->is_op(OPCODE_EXPR)) {
// 		    cpl_expr_arg(arg, prot, file);
// 		}

// 		arglist = arglist->R;
// 		func_arglist = func_arglist->R;
// 		CHECK_ERROR();
// 	}

// 	while (func_arglist && func_arglist->L) {
// 	    cpl_default_arg(node, func_arglist->L, file);
// 		func_arglist = func_arglist->R;
// 		CHECK_ERROR();
// 	}

// 	id_table.remove_scope();

// 	fprintf(file, "push rvx\n");
// 	fprintf(file, "push %d\n", id_table.get_func_offset());
// 	fprintf(file, "add\n");
// 	fprintf(file, "pop rvx\n");

// 	fprintf(file, "call ");
// 	id->print(file);
// 	fprintf(file, "_%d\n", func_offset);

// 	fprintf(file, "push rvx\n");
// 	fprintf(file, "push %d\n", id_table.get_func_offset());
// 	fprintf(file, "sub\n");
// 	fprintf(file, "pop rvx\n");
// }

// void Compiler::cpl_default_arg(const CodeNode *arg, const CodeNode *prot, FILE *file) {
// 	if (prot->is_id()) {
// 		id_table.shift_backward();
// 		bool ret = cpl_push(prot, file);
// 		id_table.shift_forward();

// 		if (!ret) {
// 			RAISE_ERROR("func call arg position:\n");
// 			LOG_ERROR_LINE_POS(arg);
// 			return;
// 		}

// 		id_table.declare_var(prot->get_id(), 1);
// 		fprintf(file, "pop ");
// 	    cpl_lvalue(prot, file, false, false, true);
// 		fprintf(file, "\n");
// 	} else if (prot->is_op(OPCODE_VAR_DEF)) {
// 		if (!prot->R) {
// 			RAISE_ERROR("bad func call, required arg [");
// 			prot->L->get_id()->print();
// 			printf("] has no default set\n");
// 			LOG_ERROR_LINE_POS(arg);
// 			return;
// 		}

// 		id_table.shift_backward();
// 		compile(prot->R, file);
// 		id_table.shift_forward();

// 		id_table.declare_var(prot->L->get_id(), 1);
// 		fprintf(file, "pop ");
// 	    cpl_lvalue(prot->L, file, false, false, true);
// 		fprintf(file, "\n");
// 	} else {
// 		RAISE_ERROR("bad func call, unexpected PROT type [");
// 		printf("%d]\n", prot->get_op());
// 		LOG_ERROR_LINE_POS(prot);
// 		return;
// 	}
// }

// void Compiler::cpl_context_arg(const CodeNode *arg, const CodeNode *prot, FILE *file) {
// 	if (prot->is_id()) {
// 		id_table.shift_backward();
// 		bool ret = cpl_push(prot, file);
// 		id_table.shift_forward();

// 		if (!ret) {
// 			RAISE_ERROR("func call arg position:\n");
// 			LOG_ERROR_LINE_POS(arg);
// 			return;
// 		}

// 		id_table.declare_var(prot->get_id(), 1);
// 		fprintf(file, "pop ");
// 	    cpl_lvalue(prot, file, false, false, true);
// 		fprintf(file, "\n");
// 	} else if (prot->is_op(OPCODE_VAR_DEF)) {
// 		id_table.shift_backward();
// 		bool ret = cpl_push(prot->L, file);
// 		id_table.shift_forward();

// 		if (!ret) {
// 			RAISE_ERROR("func call arg position:\n");
// 			LOG_ERROR_LINE_POS(arg);
// 			return;
// 		}

// 		id_table.declare_var(prot->L->get_id(), 1);
// 		fprintf(file, "pop ");
// 	    cpl_lvalue(prot->L, file, false, false, true);
// 		fprintf(file, "\n");
// 	} else {
// 		RAISE_ERROR("bad func call, unexpected PROT type [");
// 		printf("%d]\n", prot->get_op());
// 		LOG_ERROR_LINE_POS(arg);
// 		return;
// 	}
// }

// void Compiler::cpl_expr_arg(const CodeNode *arg, const CodeNode *prot, FILE *file) {
// 	if (!arg->L) {
// 		RAISE_ERROR("bad func call, expr node has no expression inside\n");
// 		LOG_ERROR_LINE_POS(arg);
// 		return;
// 	}
// 	CodeNode *expr = arg->L;
// 	id_table.shift_backward();
// 	compile(expr, file);
// 	id_table.shift_forward();

// 	if (prot->is_id()) {
// 		id_table.declare_var(prot->get_id(), 1);
// 		fprintf(file, "pop ");
// 	    cpl_lvalue(prot, file, false, false, true);
// 		fprintf(file, "\n");
// 	} else if (prot->is_op(OPCODE_VAR_DEF)) {
// 		id_table.declare_var(prot->L->get_id(), 1);
// 		fprintf(file, "pop ");
// 	    cpl_lvalue(prot->L, file, false, false, true);
// 		fprintf(file, "\n");
// 	} else {
// 		RAISE_ERROR("bad func call, unexpected PROT type [");
// 		printf("%d]\n", prot->get_op());
// 		LOG_ERROR_LINE_POS(arg);
// 		return;
// 	}
// }

// void Compiler::cpl_arr_call(const CodeNode *node, FILE *file) {
// 	if (!node->R) {
// 		RAISE_ERROR("bad arr call, where is name, you are worthless [");
// 		printf("%d]\n", node->get_op());
// 		LOG_ERROR_LINE_POS(node);
// 		return;
// 	}

// 	CodeNode *id = node->R;
// 	CodeNode *args = node->L;
// 	// if (!arg->is_op(OPCODE_EXPR)) {
// 	// 	RAISE_ERROR("bad arr call, argument is not an expr [");
// 	// 	printf("%d]\n", node->get_op());
// 	// 	LOG_ERROR_LINE_POS(node);
// 	// 	return;
// 	// }

// 	int offset = 0;
// 	int ret = id_table.find_var(id->get_id(), &offset);
// 	if (ret == NOT_FOUND) {
// 		RAISE_ERROR("variable does not exist [");
// 		id->get_id()->print();
// 		printf("]\n");
// 		LOG_ERROR_LINE_POS(node);
// 		return;
// 	}

// 	fprintf(file, "push [rvx + %d]\n", offset);

// 	while (args && args->L) {
// 		CodeNode *arg = args->L;
// 	    cpl_expr(arg, file);
// 		fprintf(file, "add\n");
// 		fprintf(file, "pop rax\n");
// 		fprintf(file, "push [rax + 1]\n");
// 		args = args->R;
// 	}
// }

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
		int offset = 0;
		int found = id_table.find_var(node->get_id(), &offset);

		if (!found) {
			RAISE_ERROR("can't find varname [");
			node->get_id()->print();
			printf("]\n");
			LOG_ERROR_LINE_POS(node);
			return false;
		}

		char *name = node->get_id()->dup();
		int r1 = regman->get_var_reg(offset, found == ID_TYPE_GLOBAL ? REGMAN_VAR_GLOBAL : REGMAN_VAR_LOCAL, name);
		free(name);

		cpl_mov_reg_reg(REG_RAX, r1);
		regman->release_var_reg(r1);
	}

	return true;
}

bool Compiler::cpl_lvalue(const CodeNode *node, int &offset, int &found) {
	assert(node);

	if (node->is_id()) {
		if (node->get_id()->starts_with("_") && !node->get_id()->starts_with("_)")) {
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

		int is_found = id_table.find_var(node->get_id(), &offset);
		found = is_found;

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
	}
	// } else if (node->is_op(OPCODE_FUNC_CALL) && id_table.find_func(node->R->get_id()) == NOT_FOUND) { // so that's an array
	// 	CodeNode *id  = node->R;
	// 	CodeNode *args = node->L;

	// 	if (!initialization && id->get_id()->starts_with("_") && !id->get_id()->starts_with("_)")) {
	// 		RAISE_ERROR("_varname is a constant, dont change it please: [");
	// 		node->get_id()->print();
	// 		printf("]\n");
	// 		LOG_ERROR_LINE_POS(node);
	// 	}

	// 	int offset = 0;
	// 	int ret = id_table.find_var(id->get_id(), &offset);
	// 	if (ret == NOT_FOUND) {
	// 		RAISE_ERROR("variable does not exist [");
	// 		id->get_id()->print();
	// 		printf("]\n");
	// 		LOG_ERROR_LINE_POS(node);
	// 		return false;
	// 	}

	// 	// we are called by assign, so there's 'pop ' already written in assembler, let's fix it
	// 	if (for_asgn_dup) {
	// 		fprintf(file, "[rcx]\n");
	// 		//fprintf(file, "pop rzx\n");
	// 		// fprintf(file, "push [rax + %d + 1]\n", offset);
	// 		return true;
	// 	}

	// 	if (to_push) {
	// 		fprintf(file, "0\n");
	// 		fprintf(file, "pop rzx\n");
	// 	} else {
	// 		fprintf(file, "rbx\n");
	// 		fprintf(file, "push rbx\n");
	// 	}

	// 	fprintf(file, "push rvx + %d\n", offset);
	// 	fprintf(file, "pop rax\n");
	// 	while (args && args->L) {
	// 		fprintf(file, "push [rax]\n");
	// 		CodeNode *arg = args->L;
	// 	    cpl_expr(arg, file);
	// 		// TODO wtf is this... it works... so let it be... for 2d arrs... but not anyhow more...
	// 		//if (args->R->L) {
	// 			fprintf(file, "push 1\n");
	// 			fprintf(file, "add\n");
	// 		//}
	// 		// -------------------------------------------------------------------------------------
	// 		fprintf(file, "add\n");
	// 		fprintf(file, "pop rax\n");
	// 		//fprintf(file, "push [rax]\n");
	// 		args = args->R;
	// 	};

	// 	fprintf(file, "push rax\n");
	// 	fprintf(file, "pop rcx\n");
	// 	if (to_push) {
	// 		fprintf(file, "push [rax]\n");
	// 	} else {
	// 		fprintf(file, "pop [rax]\n");
	// 	}
	// 	return true;
	// } else {
	// 	RAISE_ERROR("bad compiling type, node is [%d]\n", node->get_type());
	// 	LOG_ERROR_LINE_POS(node);
	// 	return false;
	// }

	offset = -0x111111;
	found = 0;
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
			break;
		}

		case ID : {
			if (id_table.find_func(node->get_id()) != NOT_FOUND) {
			    // cpl_func_call(node, file);
			} else if (node->R) {
			    // cpl_arr_call(node, file);
			} else {
			    cpl_rvalue(node);
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

	fprintf(file, "push %d\n", INIT_RVX_OFFSET);
	fprintf(file, "pop rvx\n");
	fprintf(file, "push %d\n", INIT_RMX_OFFSET);
	fprintf(file, "pop rmx\n");

	compile(prog, file);
	fclose(file);

	if (ANNOUNCEMENT_ERROR) {
		fprintf(file, "AN ERROR OCCURED DURING COMPILATION IUCK\n");
		ANNOUNCE("ERR", "kncc", "An error occured during compilation");
		return false;
	}
	
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
