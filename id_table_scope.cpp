#include "id_table_scope.h"

IdData::IdData():
type(0),
id(nullptr),
offset(0),
arglist(nullptr)
{}

IdData& IdData::operator=(const IdData& other) {
	type    = other.type;
	id      = other.id;
	offset  = other.offset;
	arglist = other.arglist;

	return *this;
}

void IdData::ctor(int type_, const StringView *id_, const int offset_, const CodeNode *arglist_) {
	type    = type_;
	id      = id_;
	offset  = offset_;
	arglist = arglist_;
}

bool IdData::equal(const IdData &other) {
	if (id == nullptr || other.id == nullptr) {
		return false;
	}

	return type == other.type && id->equal(other.id);
}

//=============================================================================
// IdTableScope ==============================================================

IdTableScope::IdTableScope():
data(),
var_cnt(0),
functive(0),
offset(0)
{}

IdTableScope::~IdTableScope() {}

void IdTableScope::ctor(const int offset_, const int functive_) {
	data.ctor();
	offset = offset_;
	var_cnt = 0;
	functive = functive_;
}

IdTableScope *IdTableScope::NEW(const int offset_, const int functive_) {
	IdTableScope *cake = (IdTableScope*) calloc(1, sizeof(IdTableScope));
	if (!cake) {
		return nullptr;
	}

	cake->ctor(offset_, functive_);
	return cake;
}

void IdTableScope::dtor() {
	data.dtor();
}

void IdTableScope::DELETE(IdTableScope *scope) {
	if (!scope) {
		return;
	}

	scope->dtor();
	free(scope);
}

//=============================================================================

bool IdTableScope::find_id(const StringView *id) const {
	if (!id) {
		return false;
	}

	size_t data_size = data.size();
	for (size_t i = 0; i < data_size; ++i) {
		if (data[i].id && id->equal(data[i].id)) {
			return true;
		}
	}

	return false;
}

int IdTableScope::find(const int type, const StringView *id) const {
	IdData idat = {};
	idat.ctor(type, id, 0);

	size_t data_size = data.size();
	int id_offset = 0;
	for (size_t i = 0; i < data_size; ++i) {
		if (idat.equal(data[i])) {
			if (type == ID_TYPE_FUNC) {
				return data[i].offset;
			}

			return id_offset;
		} else {
			id_offset += data[i].offset;
		}
	}

	return NOT_FOUND;
}

int IdTableScope::get_var_cnt() const {
	return offset;
}

bool IdTableScope::declare(const int type, const StringView *id, const int size, const CodeNode *arglist_) {
	IdData idat = {};
	idat.ctor(type, id, size, arglist_);

	if (find_id(id)) {
		return false;
	} else {
		data.push_back(idat);
		if (type != ID_TYPE_FUNC) {
			offset += size;
		}
		return true;
	}
}

bool IdTableScope::add_buffer_zone(const int zone_size) {
	IdData idat = {};
	idat.ctor(ID_TYPE_NONE, nullptr, zone_size);
	data.push_back(idat);
	offset += zone_size;
	return true;
}

const CodeNode *IdTableScope::get_arglist(const StringView *id) {
	IdData idat = {};
	idat.ctor(ID_TYPE_FUNC, id, 0);

	size_t data_size = data.size();
	for (size_t i = 0; i < data_size; ++i) {
		if (idat.equal(data[i])) {
			return data[i].arglist;
		}
	}

	return nullptr;
}

int IdTableScope::size() {
	return (int)data.size();
}

int IdTableScope::is_functive() {
	return functive;
}

void IdTableScope::dump() {
	for (size_t i = 0; i < data.size(); ++i) {
		printf("[%lu] ", i);
		data[i].id->print();
		printf("\n");
	}
}
