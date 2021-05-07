#ifndef GENERAL_STRINGVIEW
#define GENERAL_STRINGVIEW

#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cctype>

class StringView {
private:
	const char *buffer;
	char null_char;
	size_t size;

public:
	StringView(const StringView& other) :
	buffer(other.buffer),
	null_char(other.null_char),
	size(other.size)
	{}

	StringView& operator=(const StringView& other) {
		buffer = other.buffer;
		null_char = other.null_char;
		size = other.size;

		return *this;
	}

	StringView() :
	buffer(nullptr),
	null_char('\0'),
	size(0)
	{}

	~StringView() {}

	void ctor() {dtor();}

	static StringView *NEW() {
		StringView *cake = (StringView*) calloc(1, sizeof(StringView));
		if (!cake) {
			return nullptr;
		}

		cake->ctor();
		return cake;
	}

	void ctor(const char *c_string, bool to_count_len = true) {
		if (c_string == nullptr) {
			dtor();
			return;
		}

		if (to_count_len) {
			size = strlen(c_string);
		}
		buffer = c_string;
	}

	static StringView *NEW(const char *c_string, bool to_count_len = true) {
		StringView *cake = (StringView*) calloc(1, sizeof(StringView));
		if (!cake) {
			return nullptr;
		}

		cake->ctor(c_string, to_count_len);
		return cake;
	}

	void dtor() {
		buffer = nullptr;
		null_char = '\0';
		size = 0;
	}

	static void DELETE(StringView *stringview) {
		if (!stringview) {
			return;
		}

		stringview->dtor();
		free(stringview);
	}

	static bool starts_with(const char *first, const char *second) {
		if (!first || !second) {
			return false;
		}

		size_t len_1 = strlen(first);
		size_t len_2 = strlen(second);
		return len_1 < len_2 || (!len_2) ? false : memcmp(first, second, len_2) == 0;
	}

	bool is_null() const {
		return buffer == nullptr;
	}

	bool starts_with(const StringView &other) {
		size_t len_1 = size;
		size_t len_2 = other.length();
		return len_1 < len_2 ? false : memcmp(buffer, other.get_buffer(), len_2) == 0;
	}

	bool starts_with(const char *other) {
		if (!other) {
			return false;
		}

		size_t len_1 = size;
		size_t len_2 = strlen(other);
		return len_1 < len_2 ? false : memcmp(buffer, other, len_2) == 0;
	}

	size_t length() const {
		return size;
	}

	const char &operator[](size_t i) const {
		if (i >= size) {
			return null_char;
		} else {
			return buffer[i];
		}
	}

	int read(char *c_string, const bool stop_on_space = true, const char ending_char = '\0') {
		if (c_string == nullptr) {
			return -1;
		}

		char *c = c_string;
		size_t length = 0;
		while (*c && (!stop_on_space || !isspace(*c)) && *c != ending_char) {
			++c;
			++length;
		}

		dtor();

		size = length;
		buffer = c_string;

		return (int) length;
	}

	void generate_length_format(char *string) {
		char format[24]; // to simulate the certain lenth of out stringview
		sprintf(format, "%%.%lus", size);

		size_t length = strlen(string);

		char *string_copy = (char*) calloc(length + 1, sizeof(char));
		memcpy(string_copy, string, length);

		sprintf(string, string_copy, format);

		free(string_copy);
	}

	void print(FILE *file_ptr = stdout, const int sidx = -1, const int eidx = -1) const {
		if (is_null()) {
			return;
		}

		if (sidx < 0 && eidx < 0) {
			char format[24]; // to simulate the certain lenth of out stringview
			sprintf(format, "%%.%lus", size);
			fprintf(file_ptr, format, buffer);
		} else {
			for (int i = sidx; !(i >= (int) size || (eidx >= 0 && i < eidx)); ++i) {
				fprintf(file_ptr, "%c", buffer[i]);
			}
		}
	}

	const char *get_buffer() const {
		return buffer;
	}

	void set_length(const size_t length_) {
		size = length_;
	}

	bool operator==(const StringView &other) const {
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

	bool equal(const StringView *other) const {
		return (*this) == (*other);
	}

	bool equal(const char *other) const {
		size_t len = strlen(other);
		if (len != size) {
			return false;
		}

		for (size_t i = 0; i < len; ++i) {
			if (buffer[i] != other[i]) {
				return false;
			}
		}

		return true;
	}

	char *dup(char *output = nullptr) const {
		if (!output) output = (char*) calloc(length() + 2, sizeof(char));
		memcpy(output, buffer, size);
		return output;
	}
};

#endif // GENERAL_STRINGVIEW