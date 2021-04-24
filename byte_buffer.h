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

   	void put(byte *mem, size_t mem_size);
   	void put(byte  mem);

   	void put_with_offset(byte *mem, size_t mem_size, size_t offset);
   	
   	size_t get_size() const;
    const byte*get_data() const;
};


#endif //  BYTE_BUFFER
