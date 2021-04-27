/*
sponsored by
1) https://github.com/alpocnito/MIPT/blob/master/2_semestr/RealAsmTranslator/translator.cpp
2) https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
*/

#include <elf.h>

#pragma pack(push, 1)    // that's important to make compiler pack headers withour any alignments to preserve header's sizes

#define B1 __uint8_t    // 1 byte
#define B2 __uint16_t    // 2 bytes
#define B4 __uint32_t    // 4 bytes
#define B8 __uint64_t    // 8 bytes

struct ELF_Header {
    B4 ei_MAG        = 0x464C457F;           // signature - " ELF"
    B1 ei_CLASS      = ELFCLASS64;           // 64 bit format
    B1 ei_DATA       = ELFDATA2LSB;          // little endian
    B4 ei_VERSION    = 0x00000001;           // signature
    B2 ei_OSABI      = ELFOSABI_NONE;        // UNIX System V ABI
    B4 ei_OSABIVER   = 0x00000000;           // are not used yet (literally)
    B2 E_TYPE        = ET_EXEC;              // executable file
    B2 E_MACHINE     = EM_X86_64;            // x86_64 AMD
    B4 E_VERSION     = EV_CURRENT;           // signature
    B8 E_ENTRY       = 0x0000000000400000;   // entry point, = 400000, we will fix it before writing to file
    B8 E_PHOFF       = 0x0000000000000040;   // start pf program header table
    B8 E_SHOFF       = 0x0000000000000000;   // start of the section header table
    B4 E_FLAGS       = 0x00000000;           // signature
    B2 E_EHSIZE      = 0x0040;               // size of this header
    B2 E_PHENTSIZE   = 0x0038;               // size of program header table 
    B2 E_PHNUM       = 0x0001;               // number of entries in the progtam header file
    B2 E_SHENTSIZE   = 0x0040;               // size of the section header table
    B2 E_SHNUM       = 0x0005;               // number of entries in the section header table
    B2 E_SHSTRNDX    = 0x0004;               // section header, that contains section names
};

struct ProgHeader {
    B4 P_TYPE    = 0x00000000 | PT_LOAD;       // Segment will loaded in memory
    B4 P_FLAGS   = 0x00000000 | PF_R | PF_X;   // Read and Execute
    B8 P_OFFSET  = 0x0000000000000000;         // Offset where it should be read
    B8 P_VADDR   = 0x0000000000400000;         // Virtual address where it should be loaded
    B8 P_PADDR   = 0x0000000000400000;         // Phsical address where it should be loaded
    B8 P_FILESZ  = 0x0000000000000080;         // Size on file
    B8 P_MEMSZ   = 0x0000000000000080;         // Size in memory (??)
    B8 P_ALIGN   = 0x0000000000200000;         // P_VADDR = P_OFFSET % P_ALIGN ???
    B8 zeoroz    = 0x0000000000000000;         // (zeroes?)
};

#pragma pack(pop)


void build_elf(const char *prog, const size_t prog_size, FILE *file, bool to_add_exit_code_zero = true) {
    ELF_Header elf_h ;
    ProgHeader prog_h;

    elf_h.E_ENTRY += sizeof(ELF_Header);
    elf_h.E_ENTRY += sizeof(ProgHeader);

    prog_h.P_FILESZ += prog_size;
    prog_h.P_MEMSZ  += prog_size;

    fwrite(&elf_h,  sizeof(elf_h),  1,         file);
    fwrite(&prog_h, sizeof(prog_h), 1,         file);
    fwrite(prog,    sizeof(byte),   prog_size, file);
    
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

void build_elf(const char *prog, const size_t prog_size, const char *filename, bool to_add_exit_code_zero = true) {
    FILE *file = fopen(filename, "wb");
    assert(file);

    build_elf(prog, prog_size, file, to_add_exit_code_zero);

    fclose(file);
}
