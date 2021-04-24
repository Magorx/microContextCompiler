#include "byte_buffer.h"

ByteBuffer::ByteBuffer():
capacity(8),
size(0),
data((byte*) calloc(8, sizeof(byte))),
cur_ptr(data)
{}

ByteBuffer::~ByteBuffer() {
    if (data) {
        free(data);
    }
}

void ByteBuffer::reallocate(size_t add_size) {
    if (size + add_size < capacity) {
        return;
    }

    void *new_data = realloc(data, capacity * 2);
    if (!new_data) {
        ANNOUNCE("ERR", "byte_buffer", "failed to reallocate");
        return;
    }

    data = (byte*) new_data;
    cur_ptr = data + size;
    capacity *= 2;
}

void ByteBuffer::put(byte *mem, size_t mem_size) {
    assert(mem);
    reallocate(mem_size);

    memcpy(cur_ptr, mem, mem_size);
    cur_ptr += mem_size;
    size += mem_size;
}

void ByteBuffer::put(byte mem) {
    reallocate();

    *cur_ptr = mem;
    ++cur_ptr;
    ++size;
}

void ByteBuffer::put_with_offset(byte *mem, size_t mem_size, size_t offset) {
    assert(mem);
    reallocate(mem_size);

    memcpy(data + offset, mem, mem_size);
}

size_t ByteBuffer::get_size() const {
    return size;
}

const byte *ByteBuffer::get_data() const {
    return data;
}
