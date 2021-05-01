#include "id_table.h"

void IdTable::add_scope(const int offset, const int functive) {
	IdTableScope *scope = IdTableScope::NEW(offset, functive);
	data.push_back(scope);
}

IdTable::IdTable():
data(),
cur_scope(0),
var_cnt(0)
{}

IdTable::~IdTable() {}

void IdTable::ctor() {
	data.ctor();
	cur_scope = 0;
	var_cnt = 0;
}

IdTable *IdTable::NEW() {
	IdTable *cake = (IdTable*) calloc(1, sizeof(IdTable));
	if (!cake) {
		return nullptr;
	}

	cake->ctor();
	return cake;
}

void IdTable::dtor() {
	for (int i = (int) data.size() - 1; i >= 0; --i) {
		IdTableScope::DELETE(data[i]);
		data.pop_back();
	}
	data.dtor();
}

void IdTable::DELETE(IdTable *table) {
	if (!table) {
		return;
	}

	table->dtor();
	free(table);
}

//=============================================================================

int IdTable::find_first_functive() const {
	const int data_size = (int) data.size();
	for (int i = 0; i < data_size; ++i) {
		if (data[i]->is_functive() == FUNC_SCOPE) {
			return i;
		}
	}

	return data_size;
}

int IdTable::find_last_functive() const {
	const int data_size = (int) data.size();
	for (int i = data_size - 1; i >= 0; --i) {
		if (data[i]->is_functive() == FUNC_SCOPE) {
			return i;
		}
	}

	return 0;
}

int IdTable::find_var(const StringView *id, int *res) const {
	if (!data.size()) {
		return NOT_FOUND;
	}

	int offset = NOT_FOUND;
	int found_index = -1;
	int first_functive = find_first_functive();
	int last_functive  = find_last_functive();

	for (int i = cur_scope; i >= 0; --i) {
		offset = data[i]->find(ID_TYPE_VAR, id);
		if (offset != NOT_FOUND && (i < first_functive || i >= last_functive)) {
			found_index = i;
			break;
		}
	}

	if (offset == NOT_FOUND) {
		return NOT_FOUND;
	}

	if (data[found_index]->is_functive() == ARG_SCOPE) {
		bool to_add_offset = false;
		for (int i = found_index - 1; i >= 0; --i) {
			if (data[i]->is_functive() == FUNC_SCOPE) {
				to_add_offset = true;
			}
		}

		if (found_index > last_functive && to_add_offset) {
			for (int i = found_index - 1; i >= last_functive; --i) {
				offset += data[i]->get_var_cnt();
			}
		}

		*res = offset * (int) sizeof(long long);
		return ID_TYPE_FOUND;
	}

	if (found_index < first_functive) {
		*res = offset * (int) sizeof(long long);
		return ID_TYPE_GLOBAL;
	}

	for (int i = found_index - 1; i >= last_functive; --i) {
		offset += data[i]->get_var_cnt();
	}

	*res = offset * (int) sizeof(long long);
	return ID_TYPE_FOUND;
}

int IdTable::find_func(const StringView *id) const {
	for (int i = cur_scope; i >= 0; --i) {
		int offset = data[i]->find(ID_TYPE_FUNC, id);
		if (offset != NOT_FOUND) {
			return offset;
		}
	}

	return NOT_FOUND;
}

int IdTable::find_in_upper_scope(const int type, const StringView *id) const {
	if (!data.size()) {
		return NOT_FOUND;
	}

	return data[cur_scope]->find(type, id);
}

int IdTable::find_from_prev(const int type, const StringView *id) const {
	for (int i = cur_scope - 1; i >= 0; --i) {
		int offset = data[i]->find(type, id);
		if (offset != NOT_FOUND) {
			return offset;
		}
	}

	return NOT_FOUND;
}

int IdTable::get_func_offset() const {
	int offset = 0;
	for (int i = cur_scope; i >= 0; --i) {
		offset += data[i]->get_var_cnt();
		if (data[i]->is_functive() == FUNC_SCOPE) {
			return offset;
		}
	}

	return 0;
}

const CodeNode *IdTable::get_arglist(const StringView *id) {
	for (int i = cur_scope; i >= 0; --i) {
		if (const CodeNode *arglist = data[i]->get_arglist(id)) {
			return arglist;
		}
	}

	return nullptr;
}

bool IdTable::declare(const int type, const StringView *id, const int size, const CodeNode *arglist) {
	if (!data.size()) {
		RAISE_ERROR("no scope to declare a variable in\n");
		return false;
	}

	return data[cur_scope]->declare(type, id, size, arglist);
}

bool IdTable::declare_func(const StringView *id, const CodeNode *arglist, const int offset) {
	return declare(ID_TYPE_FUNC, id, offset, arglist);
}

bool IdTable::declare_var(const StringView *id, const int size, const CodeNode *fields) {
	return declare(ID_TYPE_VAR, id, size, fields);
}

bool IdTable::declare_struct(const StringView *id, const CodeNode *fields) {
	return declare(ID_TYPE_STRUCT, id, 0, fields);
}

bool IdTable::add_buffer_zone(const int zone_size) {
	if (!data.size()) {
		RAISE_ERROR("adding buffer zone with on scopes alive\n");
		return false;
	}
	return data[cur_scope]->add_buffer_zone(zone_size);
}

void IdTable::add_scope(int functive) {
	add_scope(0, functive);
	cur_scope = (int)data.size() - 1;
}

void IdTable::remove_scope() {
	if (!data.size()) {
		RAISE_ERROR("removing unexistant scope\n");
		return;
	} else {
		IdTableScope::DELETE(data[data.size() - 1]);
		data.pop_back();
	}
	cur_scope = (int)data.size() - 1;
}

bool IdTable::shift_backward() {
	if (cur_scope > 0) {
		--cur_scope;
		return true;
	} else {
		return false;
	}
}

bool IdTable::shift_forward() {
	if (cur_scope < (int)data.size() - 1) {
		++cur_scope;
		return true;
	} else {
		return false;
	}
}

int IdTable::size() {
	return (int) data.size();
}

void IdTable::dump() const {
	for (size_t i = 0; i < data.size(); ++i) {
		printf("-----[%lu]\n", i);
		data[i]->dump();
	}
}
