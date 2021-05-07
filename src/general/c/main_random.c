#include "general_random.h"

#include <stdio.h>

int main() {
	srand(time(NULL));
	FastRandom_set_seed(KCTF_POISON);

	printf("[TST]<gen_random>: \n");
	for (int i = 0; i < 10; ++i) {
		printf("[   ]<          >: %ld\n", randlong());
	}

	return 0;
}