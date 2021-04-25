#ifndef REG_MASTER
#define REG_MASTER

//=============================================================================
// ClassName ==================================================================

class ClassName {
private:
// data =======================================================================

//=============================================================================


public:
	ClassName():
	{}

	~ClassName() {}

	void ctor() {

	}

	static ClassName *NEW() {
		ClassName *cake = (ClassName*) calloc(1, sizeof(ClassName));
		if (!cake) {
			return nullptr;
		}

		cake->ctor();
		return cake;
	}

	void dtor() {

	}

	static void DELETE(ClassName *classname) {
		if (!classname) {
			return;
		}

		classname->dtor();
		free(classname);
	}

//=============================================================================

};

#endif // REG_MASTER
