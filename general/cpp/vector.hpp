#ifndef GENERAL_VECTOR
#define GENERAL_VECTOR

#include <stdexcept>

template <typename T>
class Vector {
private:
	T *buffer;
	size_t cur_size;
	size_t capacity;

	void realloc_bufer(const size_t new_capacity) {
		T* ptr = (T*) calloc(new_capacity, sizeof(T));
		if (!ptr) {
			throw std::length_error("[ERR]<vector>: realloc fail");
		}

		for (size_t i = 0; i < cur_size; ++i) {
			ptr[i] = buffer[i];
		}
		free(buffer);

		capacity = new_capacity;
		buffer= ptr;
	}

public:
	Vector           (const Vector& other) = delete;
	Vector& operator=(const Vector& other) = delete;

	Vector() :
	buffer(nullptr),
	cur_size(0),
	capacity(0)
	{}

	void ctor() {
		capacity = 32;
		buffer = (T*) malloc(capacity * sizeof(T));
		cur_size = 0;
	}

	static Vector<T> *NEW() {
		Vector<T> *cake = (Vector<T>*) calloc(1, sizeof(Vector<T>));
		if (!cake) {
			return nullptr;
		}

		cake->ctor();
		return cake;
	}

	Vector(const size_t size_) :
	buffer(nullptr),
	cur_size(0),
	capacity(0)
	{}

	void ctor(const size_t size_) {
		capacity = cur_size * 2;
		buffer = (T*) calloc(capacity, sizeof(T));
		cur_size = size_;
	}

	static Vector<T> *NEW(const size_t size_) {
		Vector<T> *cake = (Vector<T>*) calloc(1, sizeof(Vector<T>));
		if (!cake) {
			return nullptr;
		}

		cake->ctor(size_);
		return cake;
	}

	~Vector() {}

	void dtor() {
		cur_size = 0;
		free(buffer);
	}

	static void DELETE(Vector<T> *vector) {
		if (!vector) {
			return;
		}

		vector->dtor();
		free(vector);
	}

	T &operator[](const size_t i) const {
		if (i >= cur_size) {
			throw std::length_error("[ERR]<vector>: index overflow");
		}
		return buffer[i];
	}

	T &push_back(const T &val) {
		if (cur_size + 1 == capacity) {
			realloc_bufer(capacity * 2);
		}
		return buffer[cur_size++] = val;
	}

	T &pop_back() {
		if (cur_size == 0) {
			throw std::length_error("[ERR]<vector>: pop underflow");
		}
		return buffer[--cur_size];
	}

	size_t size() const {
		return cur_size;
	}

	T *get_buffer() {
		return buffer;
	}

	void quadratic_pointer_sort() {
		bool flag = false;
		for (size_t i = 0; i < size(); ++i) {
			for (size_t j = 0; j < size(); ++j) {
				if (*buffer[i] > *buffer[j]) {
					T tmp = buffer[i];
					buffer[i] = buffer[j];
					buffer[j] = tmp;
					flag = true;
				}
			}
			if (!flag) {
				return;
			}
		}
	}

	bool contains(const T &item) const {
		for (size_t i = 0; i < size(); ++i) {
			if (buffer[i] == item) {
				return true;
			}
		}
		return false;
	}

	void print_as_ints() const {
		if (!size()) {
			return;
		}

		for (size_t i = 0; i < size() - 1; ++i) {
			printf("%d ", (*this)[i]);
		}
		printf("%d", (*this)[size() - 1]);
	}
};

#endif // GENERAL_VECTOR