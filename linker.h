#ifndef LINKER_H
#define LINKER_H

#include <cstring>

#include "general/cpp/vector.hpp"
#include "byte_buffer.h"
#include "hashtable.h"
#include "general/c/announcement.h"

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
	{}

	FixupInfo(const FixupInfo& other):
	label(other.label),
	displ(other.displ),
	type(other.type),
	global_size(other.global_size)
	{}

	// FixupInfo &operator=(const FixupInfo& other) {
	// 	label = other.label;
	// 	displ = other.displ;
	// 	type = other.type;
	// 	global_size = other.global_size;
	// 	return *this;
	// }

	void inline dtor() {
		if (label) free(label);
	}
};

class MicroLinker {
private:
// data =======================================================================
	Vector<FixupInfo> fixup;
	Hashtable fixup_ht;
	
	Vector<FixupInfo> to_fixup;

	int global_data_size;
//=============================================================================

public:
	MicroLinker ();
	~MicroLinker();

	void ctor();
	static MicroLinker *NEW();

	void dtor();
	static void DELETE(MicroLinker *table);
//=============================================================================

	void add_fixup     (FixupInfo info);
	void request_fixup (const FixupInfo &info);

	void link_programm(char *cmd, size_t cmd_size, const char *filename);
};

#endif // LINKER_H
