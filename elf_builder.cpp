#include "elf_builder.h"

void build_elf(const char *prog, const size_t prog_size, FILE *file, int global_data_size, bool to_add_exit_code_zero) {
    ELF_Header elf_h ;
    ProgHeader prog_h;

    elf_h.E_ENTRY += sizeof(ELF_Header);
    elf_h.E_ENTRY += sizeof(ProgHeader);
    elf_h.E_ENTRY += global_data_size;

    prog_h.P_FILESZ += prog_size;
    prog_h.P_MEMSZ  += prog_size;

    fwrite(&elf_h,     sizeof(elf_h),     1,         file);
    fwrite(&prog_h,    sizeof(prog_h),    1,         file);

    char *buf = (char*) calloc(global_data_size, 1);
    fwrite(buf, 1, global_data_size, file); // writing 'global data section' to our elf file, remove if not needed
    free(buf);

    fwrite(prog, sizeof(byte), prog_size, file);
    
    // don't forget, your prog has to finish itself with:
    // 
    // mov rax, 60 <- exit syscall
    // mov rdi, 0  <- exit code
    // syscall
    //
    // YYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYYY

    if (to_add_exit_code_zero) {
        unsigned char buffer[] = {
            0x48, 0xc7, 0xc0, 0x3c, 0x00, 0x00, 0x00, // mov rax, 60
            0x48, 0xc7, 0xc7, 0x00, 0x00, 0x00, 0x00, // mov rdi, 0
            0x0f, 0x05                                // syscall
        };

        fwrite(buffer, sizeof(char), sizeof(buffer), file);
    }
}

void build_elf(const char *prog, const size_t prog_size, const char *filename, int global_data_size, bool to_add_exit_code_zero) {
    FILE *file = fopen(filename, "wb");
    assert(file);

    build_elf(prog, prog_size, file, global_data_size, to_add_exit_code_zero);

    fclose(file);
}
