#ifndef BYTE_BUFFER
#define BYTE_BUFFER

#include "general/c/announcement.h"
#include <cstdint>
#include <cstdlib>
#include <cassert>

typedef uint8_t byte;

class ByteBuffer {
    size_t capacity;
    size_t size;
    byte *data;
    byte *cur_ptr;

    void reallocate(size_t add_size = 1);

public:

    ByteBuffer(const ByteBuffer&) = delete;
    ByteBuffer &operator=(const ByteBuffer&) = delete;

    ByteBuffer ();
    ~ByteBuffer();

   	void put(const byte *mem, size_t mem_size);
   	void put(byte mem);
    void put(byte mem1, byte mem2);

   	void put_with_offset(const byte *mem, size_t mem_size, size_t offset);
   	
   	size_t get_size() const;
    const byte *get_data() const;

    void hexdump(FILE *file = stdout) const;
};


#endif //  BYTE_BUFFER
