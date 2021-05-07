#include <cstdlib>
#include <cstdint>
#include <cstring>
#include <cassert>

typedef int HT_RET_TYPE;

enum NODE_ALIVENESS {
	IS_DEAD = 0,
	IS_DELETED = 2,
	IS_ALIVE = 3,
};

const uint64_t MAX_KEY_LEN = 64;

struct HT_Node {
	char key[MAX_KEY_LEN + 1];
	int  val;

	uint64_t hashed;
	char     is_alive;

	HT_Node(const char *string, int value, const int) :
	val(0),
	hashed(0),
	is_alive(1)
	{
		assert(string != nullptr);

		// printf("NODE %s | %s\n", string, value);

		memset (key, 0, 	 MAX_KEY_LEN);
		strncpy(key, string, MAX_KEY_LEN - 1);
		val = value;
		is_alive = IS_ALIVE;

		hashed = 0;
	}

	HT_Node(const char *string, int value):
	val(0),
	hashed(0),
	is_alive(1)
	{
		assert(string != nullptr);

		memset (key, 0, 	 MAX_KEY_LEN);
		strncpy(key, string, MAX_KEY_LEN - 1);
		val = value;
		is_alive = IS_ALIVE;

		hashed = 0;
		hash();
	}

	HT_Node(const char *string):
	val(0),
	hashed(0),
	is_alive(1)
	{
		assert(string != nullptr);

		memset (key, 0, 	 MAX_KEY_LEN);
		strncpy(key, string, MAX_KEY_LEN - 1);
		val = 0;
		is_alive = IS_ALIVE;

		hashed = 0;
		hash();
	}

	void ctor() {
		memset (key, 0, MAX_KEY_LEN);
		val = 0;
		hashed = 0;
		is_alive = 0;
	}

	void copy_from(const HT_Node &node) {
		memcpy(key, node.key, MAX_KEY_LEN);
		val = node.val;
		is_alive = node.is_alive;
		hashed = node.hashed;
	}

	inline void count_copy(const HT_Node &node) {
		val = node.val;
	}

	uint64_t __attribute__ ((noinline)) hash() {
		if (hashed) return hashed;

		hashed = 14695981039346656037llu;
		uint64_t prime = 1099511628211llu;
		for (size_t i = 0; i < MAX_KEY_LEN; ++i) {
			hashed = hashed ^ key[i];
			hashed = hashed * prime;
		}

		return hashed;
	}

	inline char is() const {
		return is_alive;
	}

	inline char set_is(const char new_alive) {
		return is_alive = new_alive;
	}

};

inline bool operator==(const HT_Node &first, const HT_Node &second) {
	return memcmp(first.key, second.key, 32) == 0;
}

// inline bool operator==(const HT_Node &first, const HT_Node &second) {
//     bool ret = 0;
//     asm ("vmovdqu xmm7, [%1]\n"
//          "vmovdqu xmm8, [%2]\n"
//          "vpcmpeqw xmm7, xmm7, xmm8\n"
//          "vptest  xmm7, xmm7\n"
//          "xor %0, %0\n"
//          "setne %0\n"
//     :"=r"(ret):"p"(first.key), "p"(second.key): "xmm7", "xmm8");
//     return ret;
// }

// inline bool operator==(const HT_Node &first, const HT_Node &second) {
//     asm goto (
//     	 "mov rax, [%0]\n"
//          "mov rbx, [%1]\n"
//          "cmp rax, rbx\n"
//          "jne %l2\n"
//          "mov rax, [%0 + 8]\n"
//          "mov rbx, [%1 + 8]\n"
//          "cmp rax, rbx\n"
//          "jne %l2\n"
//          "mov rax, [%0 + 16]\n"
//          "mov rbx, [%1 + 16]\n"
//          "cmp rax, rbx\n"
//          "jne %l2\n"
//          "mov rax, [%0 + 24]\n"
//          "mov rbx, [%1 + 24]\n"
//          "cmp rax, rbx\n"
//          "jne %l2\n"
//     ::"p"(first.key), "p"(second.key): "cc", "memory", "rax", "rbx": label_false);
// 	return 1;

// label_false:
//     return 0;
// }

// inline bool operator==(const HT_Node &first, const HT_Node &second) {
// 	int ret = 1;
// 	int zero = 0;
//     asm (
//     	 "mov rax, [%1]\n"
//          "mov rbx, [%2]\n"
//          "cmp rax, rbx\n"
//          "cmovne %0, %3\n"
//          "mov rax, [%1 + 8]\n"
//          "mov rbx, [%2 + 8]\n"
//          "cmp rax, rbx\n"
//          "cmovne %0, %3\n"
//          "mov rax, [%1 + 16]\n"
//          "mov rbx, [%2 + 16]\n"
//          "cmp rax, rbx\n"
//          "cmovne %0, %3\n"
//          "mov rax, [%1 + 24]\n"
//          "mov rbx, [%2 + 24]\n"
//          "cmp rax, rbx\n"
//          "cmovne %0, %3\n"
//     :"=r"(ret):"p"(first.key), "p"(second.key), "r"(zero): "rax", "rbx");
// 	return ret;
// }
