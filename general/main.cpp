#include "general_cpp/string.h"

int main() {
	String s("abc!\n");

	s[0] = 'A';
	s.print();

	printf("done.\n");
	return 0;
}