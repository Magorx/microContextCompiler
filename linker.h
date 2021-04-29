#ifndef LINKER_H
#define LINKER_H

#include <cstring>

#include "general/cpp/vector.hpp"
#include "byte_buffer.h"
#include "hashtable.h"
#include "general/c/announcement.h"

#include "elf_builder.h"
#include "micro_obj.h"

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
