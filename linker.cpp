#include "linker.h"

MicroLinker::MicroLinker () {}
MicroLinker::~MicroLinker() {}

void MicroLinker::ctor() {
	fixup.ctor();
	fixup_ht.ctor();

	to_fixup.ctor();

	global_data_size = 0;
}

MicroLinker *MicroLinker::NEW() {
	MicroLinker *cake = (MicroLinker*) calloc(1, sizeof(MicroLinker));
	if (!cake) {
		return nullptr;
	}

	cake->ctor();
	return cake;
}

void MicroLinker::dtor() {
	fixup.dtor();
	fixup_ht.dtor();
	to_fixup.dtor();
}

void MicroLinker::DELETE(MicroLinker *linker) {
	if (!linker) {
		return;
	}

	linker->dtor();
	free(linker);
}

void MicroLinker::add_fixup(FixupInfo fx) {
	fixup_ht.insert({fx.label, (int) fixup.size()});
	fixup.push_back(fx);

	global_data_size += fx.global_size;
}

void MicroLinker::request_fixup(const FixupInfo &info) {
	to_fixup.push_back(info);
}

void MicroLinker::link_program(char *cmd, size_t cmd_size, size_t entry_offset, const char *filename) {
	_LOG ANNOUNCE("@@@", "linker", "program linkage start");
	_LOG ANNOUNCE_NOCODE("");
	_LOG ANNOUNCE("LNK", "linker", "requested fixing start");

	int to_fixup_size = to_fixup.size();
	for (int i = 0; i < to_fixup_size; ++i) {
		FixupInfo to_fix = to_fixup[i];
		if (to_fix.displ >= cmd_size - 3) {
			RAISE_ERROR("to_fixup[%d] displ[%d] + 3 > cmd_size[%d]\n", i, to_fix.displ, (int) cmd_size);
		}

		int idx = fixup_ht.find({to_fix.label, 0});
		if (idx < 0) {
			RAISE_ERROR("label[%s] is not found in fixup table\n", to_fix.label);
			return;
		}
		if (idx > fixup.size()) {
			RAISE_ERROR("label[%s] is found in fixup table, but has extremely big index in fixup\n", to_fix.label);
			return;
		}
		

		FixupInfo fix = fixup[idx];
		_LOG ANNOUNCE("+FX", "linker", "| fixing [%s]", to_fix.label);
		if (fix.type == fxp_RELATIVE) {
			int rel_fix = fix.displ - to_fix.displ;
			_LOG ANNOUNCE_NOCODE("|- rel displ[0x%x]", rel_fix);
			memcpy(cmd + to_fix.displ, &rel_fix, 4);
		} else if (fix.type == fxp_ABSOLUTE) {
			int abs_fix = GLOBL_DISPL + fix.displ + HDR_SIZE;
			_LOG ANNOUNCE_NOCODE("|- abs displ[0x%x]", abs_fix);
			memcpy(cmd + to_fix.displ, &abs_fix, 4);
		} else if (fix.type == fxp_FIXED) {
			int fxd_fix = fix.displ;
			_LOG ANNOUNCE_NOCODE("|- fxd displ[0x%x]", fxd_fix);
			memcpy(cmd + to_fix.displ, &fxd_fix, 4);
		}
	}
	_LOG ANNOUNCE("===", "linker", "requested fixing end");
	_LOG ANNOUNCE_NOCODE("");
	_LOG ANNOUNCE("###", "linker", "program linkage end");

	_LOG ANNOUNCE("@@@", "elfbld", "calling elf builder");
	build_elf(cmd, cmd_size, entry_offset, filename, global_data_size);
	_LOG ANNOUNCE("###", "elfbld", "elf builder done\n");
}

void MicroLinker::link_objectives(const Vector<MicroObj*> &objs, const size_t entry_obj, const char *filename, ByteBuffer *result_cmd) {
	global_data_size  = 0;
	size_t prog_size  = 0;

	size_t objs_cnt = objs.size();
	for (size_t i = 0; i < objs_cnt; ++i) {
		if (!objs[i]) {
			RAISE_ERROR("one of objective files is nullptr\n");
			return;
		}

		global_data_size += objs[i]->global_data_size;
		prog_size += objs[i]->cmd_size;
	}

	if (entry_obj >= objs_cnt) {
		RAISE_ERROR("entry objective[%lu] is out of objectives vector size\n", entry_obj);
		return;
	}

	fixup.clear();
	to_fixup.clear();

	char *prog = (char*) calloc(prog_size + 1, sizeof(char));
	if (!prog) {
		RAISE_ERROR("can't allocate buffer for resulting program\n");
		return;
	}

	_LOG ANNOUNCE("@@@", "linker", "objectives linkage start\n");

	size_t cur_offset = 0;
	size_t cur_global_offset = 0;
	size_t entry_offset = 0;
	for (size_t i = 0; i < objs_cnt; ++i) {
		MicroObj *obj = objs[i];
		_LOG ANNOUNCE("LNK", "linker", "appending obj[%lu], size[%lu]", i, obj->cmd_size);

		memcpy(prog + cur_offset, obj->cmd, obj->cmd_size);

		if (i == entry_obj) {
			entry_offset = cur_offset;
		}

		_LOG ANNOUNCE("+FX", "linker", "adding fixups");
		for (int fx_i = 0; fx_i < obj->fixup.size(); ++fx_i) {
			FixupInfo fx = obj->fixup[fx_i];
			if (fixup_ht.find({fx.label}) != -1) {
				RAISE_ERROR("duplicate fixup[%s]\n", fx.label);
				return;
			}

			_LOG ANNOUNCE("+FX", "linker", "| adding fixup[%s] (displ %d)", fx.label, fx.displ);
			
			if (fx.type == fxp_RELATIVE) {
				fx.displ += cur_offset;
			} else {
				fx.displ += cur_global_offset;
			}

			_LOG ANNOUNCE_NOCODE("|- displ[0x%x]", fx.displ);

			fixup_ht.insert({fx.label, (int) fixup.size()});
			fixup.push_back(fx);
		}

		_LOG ANNOUNCE("===", "linker", "finished adding fixups");
		_LOG ANNOUNCE_NOCODE("");
		_LOG ANNOUNCE("?FX", "linker", "starting adding to_fixes");

		for (int fx_i = 0; fx_i < obj->to_fixup.size(); ++fx_i) {
			FixupInfo to_fx = obj->to_fixup[fx_i];
			to_fx.displ += cur_offset;

			_LOG ANNOUNCE("?FX", "linker", "| adding to_fixup[%s]", to_fx.label);
			_LOG ANNOUNCE_NOCODE("|- displ[0x%x]", to_fx.displ);

			to_fixup.push_back(to_fx);
		}

		_LOG ANNOUNCE("===", "linker", "finished adding to_fixes\n");

		cur_offset += obj->cmd_size;
		cur_global_offset += obj->global_data_size;
	}

	_LOG ANNOUNCE("===", "linker", "finished building prog buffer (size = %lu) & fixups\n", prog_size);

	_LOG ANNOUNCE("___", "linker", "==+=======================+==");
	_LOG ANNOUNCE("LNK", "linker", "  | entry offset [0x%04x] |   ", (int) entry_offset + global_data_size);
	_LOG ANNOUNCE("___", "linker", "==+=======================+==\n");

	link_program(prog, prog_size, entry_offset, filename);

	if (result_cmd) {
		result_cmd->put((const unsigned char*) prog, prog_size);
	}

	free(prog);

	_LOG ANNOUNCE("###", "linker", "objectives linkage end");
}