#include "hashtable.h"

Hashtable::Hashtable()
{}

Hashtable::~Hashtable()
{}

int Hashtable::ctor(const size_t capacity_, const size_t bucket_size_, const float max_loadrate_) {
	size = 0;
	capacity = capacity_;
	bucket_size = bucket_size_;
	loadrate = 0;
	max_loadrate = max_loadrate_;

	overall_data_size = capacity * bucket_size;

	data = (HT_Node*) calloc(overall_data_size, sizeof(HT_Node));
	if (!data) {
		return ERROR_MALLOC_FAIL;
	}

	for (size_t i = 0; i < overall_data_size; ++i) {
		data[i].ctor();
	}

	check_loadrate();

	return OK;
}

int Hashtable::dtor() {
	if (data) {
		free(data);
	}
	data = nullptr;

	capacity = 0;
	bucket_size = 0;
	overall_data_size = 0;
	max_loadrate = 0;
	loadrate = 0;

	return OK;
}

bool Hashtable::is_valid() const {
	return !((capacity * bucket_size == overall_data_size) && (!overall_data_size || data));
}

int Hashtable::rehash() {
	VERIFY_OK(is_valid());

	HT_Node *new_data = (HT_Node*) calloc((bucket_size + REHASH_BUCKET_INCREASE_CNT) * (capacity * REHASH_CAPACITY_INCREASE_RATE), sizeof(HT_Node));
	if (!new_data) {
		return ERROR_REALLOC_FAIL;
	}

	HT_Node *old_data = data;
	size_t old_ov_size = overall_data_size;
	
	data = new_data;
	bucket_size += REHASH_BUCKET_INCREASE_CNT;
	capacity *= REHASH_CAPACITY_INCREASE_RATE;
	overall_data_size = capacity * bucket_size;
	size = 0;

	for (size_t i = 0; i < old_ov_size; ++i) {
		const HT_Node &n = old_data[i];
		if (n.is() == IS_ALIVE) {
			insert(n, false);
		}
	}

	free(old_data);
	return OK;
}

int Hashtable::check_loadrate() {
	loadrate = ((float) size) / overall_data_size;
	// printf("loadrate: %g\n", loadrate);
	if (loadrate > max_loadrate || overall_data_size - size < MIN_ADDITIONAL_CAPACITY) {
		return rehash();
	}

	return OK;
}

int Hashtable::insert(const HT_Node &node, bool to_check_loadrate) {
	VERIFY_OK(is_valid());

	size_t hash = node.hashed % capacity;
	size_t coord = hash * bucket_size;

	// printf("hash_pos(%lu) for %s is %lu\n", node.hashed, node.key, hash);

	size_t max_i = bucket_size;
	size_t first_free_index = (size_t) -1;
	for (size_t i = 0; i < max_i; ++i, ++coord) {
		HT_Node &n = data[coord];
		if (n.is() < IS_ALIVE && (first_free_index == (size_t) -1)) {
			first_free_index = coord;
		}

		if (n.is() == IS_DEAD) {
			break;
		}

		if (n == node) {
			n.count_copy(node);
			return OK;
		}
	}

	if (first_free_index != (size_t) -1) {
		data[first_free_index].copy_from(node);
		++size;
		if (to_check_loadrate) VERIFY_OK(check_loadrate());
	} else {
		VERIFY_OK(rehash());
		VERIFY_OK(insert(node, true));
	}

	return OK;
}

int Hashtable::erase(const HT_Node &node) {
	VERIFY_OK(is_valid());

	size_t hash = node.hashed % capacity;
	size_t coord = hash * bucket_size;

	size_t max_i = bucket_size;
	for (size_t i = 0; i < max_i; ++i, ++coord) {
		HT_Node &n = data[coord];
		if (n.is() == IS_DEAD) {
			return OK;
		}

		if (n == node) {
			n.set_is(IS_DELETED);
			return OK;
		}
	}
	return OK;
}

HT_RET_TYPE Hashtable::find(const HT_Node &node) const {
	VERIFY_T(is_valid() == OK, HT_RET_TYPE);

	size_t hash = node.hashed % capacity;
	size_t coord = hash * bucket_size;

	size_t max_i = bucket_size;
	for (size_t i = 0; i < max_i; ++i, ++coord) {
		HT_Node &n = data[coord];
		if (n.is() == IS_DEAD) {
			return (HT_RET_TYPE) -1;
		}
		
		if (n.is() == IS_ALIVE && n == node) {
			return n.val;
		}
	}

	return (HT_RET_TYPE) -1;
}

int Hashtable::execute_queue(HT_Node *nodes, char *operations, size_t queue_size, HT_RET_TYPE *result) {
	assert(nodes);
	assert(operations);
	assert(result);

	for (size_t i = 0; i < queue_size; ++i) {
		nodes[i].hash();
	}

	if ((long int) operations == HT_Q_INSERT) {
		for (size_t i = 0; i < queue_size; ++i, ++nodes) {
			VERIFY_OK(insert(*nodes));
		}

		return OK;
	} else if ((long int) operations == HT_Q_ERASE) {
		for (size_t i = 0; i < queue_size; ++i, ++nodes) {
			VERIFY_OK(erase(*nodes));
		}

		return OK;
	} else if ((long int) operations == HT_Q_FIND) {
		for (size_t i = 0; i < queue_size; ++i, ++nodes) {
			result[i] = find(*nodes);
		}
		return OK;
	}

	for (size_t i = 0; i < queue_size; ++i, ++nodes) {
		switch ((long int) operations[i]) {
			case HT_Q_INSERT : {
				VERIFY_OK(insert(*nodes));
				break;
			}

			case HT_Q_ERASE : {
				VERIFY_OK(erase(*nodes));
				break;
			}

			case HT_Q_FIND : {
				result[i] = find(*nodes);
				break;
			}
		}
	}

	return OK;
}