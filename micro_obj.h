#ifndef MICRO_OBJ
#define MICRO_OBJ

#include <cstring>

#include "general/cpp/vector.hpp"
#include "byte_buffer.h"
#include "hashtable.h"
#include "general/c/announcement.h"

#include "general/log.h"
#include "elf_builder.h"

enum FIXUP_INFO {
	fxp_RELATIVE = 1,
	fxp_ABSOLUTE = 2
};

struct FixupInfo {
	char *label;
	int   displ;
	int	  type;
	int   global_size = 0;

	FixupInfo(const char *label_, const int displ_, const int type_, int global_size_=0):
	label(label_ ? strdup(label_) : nullptr),
	displ(displ_),
	type(type_),
	global_size(global_size_)
	{
		printf("CREATED WITH %s LABEL\n", label);
	}

	FixupInfo(const FixupInfo& other):
	label(other.label),
	displ(other.displ),
	type(other.type),
	global_size(other.global_size)
	{}

	void inline dtor() {
		if (label) free(label);
	}
};

struct MicroObj {
	Vector<FixupInfo> fixup;
	Hashtable fixup_ht;
	
	Vector<FixupInfo> to_fixup;

	int global_data_size;

	byte *cmd;
	size_t cmd_size;

	MicroObj ();
	~MicroObj();

	MicroObj           (const MicroObj& other) = delete;
	MicroObj& operator=(const MicroObj& other) = delete;

	void ctor();
	static MicroObj *NEW();

	void dtor();
	static void DELETE(MicroObj *table);
//=============================================================================

	void add_fixup     (FixupInfo info);
	void request_fixup (const FixupInfo &info);

	void set_prog(byte *cmd_, size_t cmd_size_);
};

#endif // MICRO_OBJ
