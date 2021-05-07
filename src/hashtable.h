#ifndef HASHTABLE
#define HASHTABLE

#include "general/c/common.h"

#include "ht_node.h"

const size_t REHASH_CAPACITY_INCREASE_RATE = 2;
const size_t REHASH_BUCKET_INCREASE_CNT = 1;
const size_t MIN_ADDITIONAL_CAPACITY = 5;

enum QUEUE_OPERATION {
	HT_Q_INSERT = 1,
	HT_Q_ERASE,
	HT_Q_FIND,
};

#define HT_Q_INSERT_ONLY (char*) HT_Q_INSERT
#define HT_Q_ERASE_ONLY (char*) HT_Q_ERASE
#define HT_Q_FIND_ONLY (char*) HT_Q_FIND

#define HT_Q_IGNORANCE_BUFFER (HT_RET_TYPE*) 1

class Hashtable {
private:
// data =======================================================================
	size_t size;
	size_t capacity;
	size_t bucket_size;
	size_t overall_data_size;
	HT_Node *data;

	float loadrate;
	float max_loadrate;
//=============================================================================

	int rehash();
	int check_loadrate();

public:
	Hashtable(const Hashtable&) = delete;
	Hashtable &operator=(const Hashtable&) = delete;

	 Hashtable();
	~Hashtable();

	int ctor(const size_t capacity_ = 1000000, const size_t bucket_size_ = 10, const float max_loadrate_ = 0.5);
	int dtor();
	
	static Hashtable *NEW(const size_t size_ = 1000000, const size_t bucket_size_ = 10, const float max_loadrate_ = 0.5) {
		Hashtable *cake = (Hashtable*) calloc(1, sizeof(Hashtable));
		if (!cake) {
			return nullptr;
		}

		VERIFY_T(cake->ctor(size_, bucket_size_, max_loadrate_) == OK, Hashtable*);
		return cake;
	}

	static int DELETE(Hashtable *hashtable) {
		if (!hashtable) {
			return ERROR_BAD_OBJECT;
		}

		VERIFY_OK(hashtable->dtor());
		free(hashtable);

		return OK;
	}

	bool is_valid() const;

	int insert (const HT_Node &node, bool to_check_loadrate = true);
	int erase  (const HT_Node &node);
	HT_RET_TYPE	find   (const HT_Node &node) const;

	int execute_queue(HT_Node *nodes, char *operations, size_t queue_size, HT_RET_TYPE *result);

	inline size_t get_size() const {
		return size;
	}
};

#endif // HASHTABLE
