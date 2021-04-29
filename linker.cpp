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

void MicroLinker::add_fixup(FixupInfo info) {
	fixup_ht.insert({info.label, (int) fixup.size()});
	fixup.push_back(info);

	global_data_size += info.global_size;
}

void MicroLinker::request_fixup(const FixupInfo &info) {
	to_fixup.push_back(info);
}

void MicroLinker::link_programm(char *cmd, size_t cmd_size, const char *filename) {
	int to_fixup_size = to_fixup.size();
	for (int i = 0; i < to_fixup_size; ++i) {
		FixupInfo to_fix = to_fixup[i];
		if (to_fix.displ >= cmd_size - 3) {
			ANNOUNCE("ERR", "linker", "to_fixup[%d] displ[%d] + 3 > cmd_size[%d]", i, to_fix.displ, (int) cmd_size);
		}

		int idx = fixup_ht.find({to_fix.label, 0});
		if (idx < 0) {
			ANNOUNCE("ERR", "linker", "label[%s] is not found in fixup table", to_fix.label);
			return;
		}
		if (idx > fixup.size()) {
			ANNOUNCE("ERR", "linker", "label[%s] is found in fixup table, but has extremely big index in fixup", to_fix.label);
			return;
		}
		

		FixupInfo fix = fixup[idx];
		_log ANNOUNCE("FIX", "linker", "fixing [%s]", to_fix.label);
		if (fix.type == fxp_RELATIVE) {
			int rel_fix = fix.displ - to_fix.displ;
			_log ANNOUNCE("FIX", "linker", "                relative -> %d", rel_fix);
			memcpy(cmd + to_fix.displ, &rel_fix, 4);
		} else {
			int abs_fix = 0x400000 + fix.displ + sizeof(ELF_Header) + sizeof(ProgHeader);
			_log ANNOUNCE("FIX", "linker", "                absolute -> %d", abs_fix);
			memcpy(cmd + to_fix.displ, &abs_fix, 4);
		}
	}

	build_elf(cmd, cmd_size, filename, global_data_size);
}
