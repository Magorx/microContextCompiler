#ifndef CPP_FILE_H
#define CPP_FILE_H

#include <cstdlib>
#include <cctype>
#include <sys/stat.h>

//=============================================================================
// File =======================================================================

class File {
public:
// data =======================================================================
	char *data;
	const char *name;
	FILE *fileptr;
	struct stat info;
//=============================================================================

	File            (const File&) = delete;
	File &operator= (const File&) = delete;

	File():
	data(nullptr),
	name(nullptr),
	fileptr(nullptr),
	info()
	{}

	~File() {}

	void ctor() {
		data = nullptr;
		name = nullptr;
		fileptr = nullptr;
	}

	static File *NEW() {
		File *cake = (File*) calloc(1, sizeof(File));
		if (!cake) {
			return nullptr;
		}

		cake->ctor();
		return cake;
	}

	void ctor(const char *name_) {
		name = name_;
		if (!name) {
			return;
		}

		fileptr = fopen(name, "rb");
		if (!fileptr) {
			return;
		}

		fstat(fileno(fileptr), &(info));

		data = (char*) calloc((size_t) info.st_size + 2, sizeof(char));
	    if (!data) {
	    	fclose(fileptr);
	        return;
	    }

		size_t a = fread(data, (size_t) info.st_size, 1, fileptr);
		if (!a) {} // just for warning...
	}

	static File *NEW(const char *name_) {
		File *cake = (File*) calloc(1, sizeof(File));
		if (!cake) {
			return nullptr;
		}

		cake->ctor(name_);
		return cake;
	}

	void dtor() {
		if (name) {
			name = nullptr;
		}

		if (data) {
			free(data);
			data = nullptr;
		}

		if (fileptr) {
			fclose(fileptr);
			fileptr = nullptr;
		}
	}

	static void DELETE(File *file) {
		if (!file) {
			return;
		}

		file->dtor();
		free(file);
	}

};

#endif // CPP_FILE_H