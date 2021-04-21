#ifndef GENERAL_STRING
#define GENERAL_STRING

#include <stdexcept>

#include <cstdlib>
#include <cstdio>
#include <cstring>

class String {
private:
	char *buffer;
	size_t capacity;
	size_t size;

public:
	String() :
	buffer(nullptr),
	capacity(0),
	size(0)
	{}

	~String() {}

	void ctor() {}

	void ctor(const char *c_string) {
		if (c_string == nullptr) {
			nullify();
			return;
		}

		size = strlen(c_string);
		capacity = size + 1;
		
		buffer = (char*) malloc(sizeof(char) * (size + 1));
		if (!buffer) {
			nullify();
			return;
		}

		memcpy(buffer, c_string, size);
		buffer[size] = '\0';
	}

	static String *NEW(const char *c_string) {
		String *cake = (String*) calloc(1, sizeof(String));
		if (!cake) {
			return nullptr;
		}

		cake->ctor(c_string);
		return cake;
	}

	static String *NEW() {
		String *cake = (String*) calloc(1, sizeof(String));
		if (!cake) {
			return nullptr;
		}

		cake->ctor();
		return cake;
	}

	void dtor() {
		nullify(buffer != nullptr);
	}

	static void DELETE(String *string) {
		if (!string) {
			return;
		}

		string->dtor();
		free(string);
	}

	void nullify(const bool to_free = false) {
		if (to_free) {
			free(buffer);
		}

		buffer = nullptr;
		size = 0;
		capacity = 0;
	}

	bool is_null() const {
		return buffer == nullptr;
	}

	size_t length() const {
		return size;
	}

	char &operator[](size_t i) const {
		if (i > size) {
			throw std::length_error("[ERR]<string>: out of length indexing");
		} else {
			return buffer[i];
		}
	}

	int read(const unsigned char *c_string, const bool stop_on_space = true, const char ending_char = '\0') {
		if (c_string == nullptr) {
			return -1;
		}

		const unsigned char *c = c_string;
		size_t length = 0;
		while (*c && (!stop_on_space || !isspace(*c)) && *c != ending_char) {
			++c;
			++length;
		}

		nullify(buffer != nullptr);

		size = length;
		capacity = length + 1;

		buffer = (char*) malloc(sizeof(char) * (size + 1));
		if (!buffer) {
			nullify();
			return -1;
		}

		memcpy(buffer, c_string, size);
		buffer[size] = '\0';

		return (int) length;
	}

	void print(FILE *file_ptr = stdout, const int sidx = -1, const int eidx = -1) const {
		if (is_null()) {
			return;
		}
		if (sidx < 0 && eidx < 0) {
			fprintf(file_ptr, "%s", buffer);
		} else {
			for (int i = sidx; !(i >= (int) size || (eidx >= 0 && i < eidx)); ++i) {
				fprintf(file_ptr, "%c", buffer[i]);
			}
		}
	}

	char *get_buffer() {
		return buffer;
	}

	bool operator==(const String &other) const {
		if (length() != other.length()) {
			return false;
		}

		size_t len = length();
		for (size_t i = 0; i < len; ++i) {
			if ((*this)[i] != other[i]) {
				return false;
			}
		}

		return true;
	}
};

#endif // GENERAL_STRING