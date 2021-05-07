#include "elf_builder.h"

const int ELFHDR_SIZE = sizeof(ELF_Header);
const int PRGHDR_SIZE = sizeof(ProgHeader);
const int PRGHDR_CNT  = 3;
const int HDR_SIZE    = ELFHDR_SIZE + PRGHDR_SIZE * PRGHDR_CNT;

const int GLOBL_DISPL = 0x400000;
const int ENTRY_POINT = 0x1000;

const int ELF_TEXT_OFFSET = 0x1000;
const int ELF_DATA_OFFSET = 0x0000;

const int ELF_BSS_OFFSET = 0;
const int ELF_BSS_VADDR = 0x500000;
const int ELF_BSS_SIZE = 5000000;

void build_elf(const char *prog, const size_t prog_size, size_t entry_offset, FILE *file, int global_data_size, bool to_add_exit_code_zero) {
    ELF_Header elf_h ;
    ProgHeader prog_h;

    elf_h.E_PHNUM = PRGHDR_CNT;
    elf_h.E_ENTRY = GLOBL_DISPL + ENTRY_POINT + entry_offset;
    fwrite(&elf_h, sizeof(elf_h), 1, file);

    prog_h.P_VADDR  = GLOBL_DISPL + ENTRY_POINT;
    prog_h.P_OFFSET = ELF_TEXT_OFFSET;
    prog_h.P_FILESZ = prog_size;
    prog_h.P_MEMSZ  = prog_size;
    prog_h.P_FLAGS  = PF_R | PF_X | PF_W;
    fwrite(&prog_h, sizeof(prog_h), 1, file);
    // fwrite(&prog_h, sizeof(prog_h), 1, file);

    prog_h.P_VADDR  = GLOBL_DISPL;
    prog_h.P_OFFSET = ELF_DATA_OFFSET;
    prog_h.P_FILESZ = global_data_size;
    prog_h.P_MEMSZ  = global_data_size;
    prog_h.P_FLAGS  = PF_R | PF_W;
    fwrite(&prog_h, sizeof(prog_h), 1, file);

    prog_h.P_VADDR  = ELF_BSS_VADDR;
    prog_h.P_OFFSET = ELF_BSS_OFFSET;
    prog_h.P_FILESZ = 0;
    prog_h.P_MEMSZ  = ELF_BSS_SIZE;
    prog_h.P_FLAGS  = PF_R | PF_W;
    fwrite(&prog_h, sizeof(prog_h), 1, file);

    char *buf = (char*) calloc(ELF_TEXT_OFFSET - HDR_SIZE, 1);
    fwrite(buf, 1, ELF_TEXT_OFFSET - HDR_SIZE, file); // writing 'global data section' to our elf file, remove if not needed
    free(buf);

    fwrite(prog, sizeof(byte), prog_size, file);

    // SectionHeader sh;
    // fwrite(&sh, sizeof(SectionHeader), 1, file);
    
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

void build_elf(const char *prog, const size_t prog_size, size_t entry_offset, const char *filename, int global_data_size, bool to_add_exit_code_zero) {
    FILE *file = fopen(filename, "wb");
    assert(file);

    build_elf(prog, prog_size, entry_offset, file, global_data_size, to_add_exit_code_zero);

    fclose(file);
}
