#include "micro_obj.h"

MicroObj::MicroObj () {}
MicroObj::~MicroObj() {}

void MicroObj::ctor() {
	fixup.ctor();
	fixup_ht.ctor();

	to_fixup.ctor();

	global_data_size = 0;
}

MicroObj *MicroObj::NEW() {
	MicroObj *cake = (MicroObj*) calloc(1, sizeof(MicroObj));
	if (!cake) {
		return nullptr;
	}

	cake->ctor();
	return cake;
}

void MicroObj::dtor() {
	fixup.dtor();
	fixup_ht.dtor();
	to_fixup.dtor();
}

void MicroObj::DELETE(MicroObj *micro_obj) {
	if (!micro_obj) {
		return;
	}

	micro_obj->dtor();
	free(micro_obj);
}

void MicroObj::add_fixup(FixupInfo info) {
	fixup_ht.insert({info.label, (int) fixup.size()});
	fixup.push_back(info);

	global_data_size += info.global_size;
}

void MicroObj::request_fixup(const FixupInfo &info) {
	to_fixup.push_back(info);
}

void MicroObj::set_prog(char *cmd_, size_t cmd_size_) {
	cmd = cmd_;
	cmd_size = cmd_size_;
}
