#ifndef ID_TABLE_SCOPE
#define ID_TABLE_SCOPE

#include "general/cpp/stringview.hpp"
#include "general/cpp/vector.hpp"

#include "code_node.h"

enum ID_TYPE {
	ID_TYPE_NONE   = 0,

	ID_TYPE_FOUND  = 1, // dont change it - it's up with reg manager and objs
	ID_TYPE_GLOBAL = 2, // dont change it - it's up with reg manager and objs
	ID_TYPE_VAR    = 3,
	ID_TYPE_FUNC   = 4,
	ID_TYPE_STRUCT = 5,
	
	NOT_FOUND      = -999999999,
};

enum SCOPE_TYPE {
	ARG_SCOPE  = 1,
	FUNC_SCOPE = 2,
};

struct IdData {
	int type;
	const StringView *id;
	int offset;
	const CodeNode *arglist;

	IdData();

	IdData& operator=(const IdData& other);
	void ctor(int type_, const StringView *id_, const int offset_, const CodeNode *arglist_ = nullptr);
	bool equal(const IdData &other);
};

//=============================================================================
// IdTableScope ==============================================================

class IdTableScope {
private:
// data =======================================================================
	Vector<IdData> data;
	int var_cnt;
	int functive;
//=============================================================================


public:
	int offset;

	IdTableScope ();
	~IdTableScope();

	void ctor(const int offset_ = 0, const int functive_ = 0);
	static IdTableScope *NEW(const int offset_ = 0, const int functive_ = 0);

	void dtor();
	static void DELETE(IdTableScope *scope);
//=============================================================================

	bool find_id(const StringView *id) const;
	
	int find(const int type, const StringView *id) const;

	int get_var_cnt() const;

	bool declare(const int type, const StringView *id, const int size, const CodeNode *arglist_ = nullptr);

	bool add_buffer_zone(const int zone_size);

	const CodeNode *get_arglist(const StringView *id);

	int size() const;
	int is_functive();
	void dump(const int offset = 0) const;
};

#endif // ID_TABLE_SCOPE
