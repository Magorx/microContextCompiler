#ifndef ID_TABLE
#define ID_TABLE

#include "id_table_scope.h"

#include "general/c/announcement.h"
#include "general/cpp/vector.hpp"

//=============================================================================
// IdTable ===================================================================

class IdTable {
private:
// data =======================================================================
	Vector<IdTableScope*> data;
	int cur_scope;
	int var_cnt;
//=============================================================================

	void add_scope(const int offset, const int functive);

public:
	IdTable ();
	~IdTable();

	void ctor();
	static IdTable *NEW();

	void dtor();
	static void DELETE(IdTable *table);
//=============================================================================

	int find_first_functive() const;
	int find_last_functive () const;

	int find_var	(const StringView *id, int *res) const;
	int find_func 	(const StringView *id) const;

	int find_in_upper_scope(const int type, const StringView *id) const;
	int find_from_prev	   (const int type, const StringView *id) const;

	int get_func_offset() const;

	const CodeNode *get_arglist(const StringView *id);

	bool declare 		(const int type, const StringView *id, const int size, const CodeNode *arglist = nullptr);
	bool declare_func	(const StringView *id, const CodeNode *arglist, const int offset = 0);
	bool declare_var	(const StringView *id, const int size, const CodeNode *fields = nullptr);
	bool declare_struct	(const StringView *id, const CodeNode *fields);

	bool add_buffer_zone(const int zone_size);
	void add_scope(int functive = 0);
	void remove_scope();

	bool shift_backward();
	bool shift_forward();

	int size();
	void dump() const;
};

#endif // ID_TABLE
