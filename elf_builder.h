#ifndef ELF_BUILDER
#define ELF_BUILDER

/*
sponsored by
1) https://github.com/alpocnito/MIPT/blob/master/2_semestr/RealAsmTranslator/translator.cpp
2) https://en.wikipedia.org/wiki/Executable_and_Linkable_Format
*/

#include <elf.h>
#include <cstdio>
#include <cstdlib>
#include <cassert>

#pragma pack(push, 1)    // that's important to make compiler pack headers withour any alignments to preserve header's sizes

#define B1 __uint8_t     // 1 byte
#define B2 __uint16_t    // 2 bytes
#define B4 __uint32_t    // 4 bytes
#define B8 __uint64_t    // 8 bytes

typedef unsigned char byte;

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
    B2 E_PHNUM       = 0x0002;               // number of entries in the progtam header file
    B2 E_SHENTSIZE   = 0x0040;               // size of the section header table
    B2 E_SHNUM       = 0x0000;               // number of entries in the section header table
    B2 E_SHSTRNDX    = 0x0000;               // section header, that contains section names
};

struct ProgHeader {
    B4 P_TYPE    = 0x00000000 | PT_LOAD;       // Segment will loaded in memory
    B4 P_FLAGS   = 0x00000000;   // Read and Execute
    B8 P_OFFSET  = 0x0000000000000000;         // Offset where it should be read
    B8 P_VADDR   = 0x0000000000400000;         // Virtual address where it should be loaded
    B8 P_PADDR   = 0x0000000000400000;         // Phsical address where it should be loaded
    B8 P_FILESZ  = 0x0000000000000080;         // Size on file
    B8 P_MEMSZ   = 0x0000000000000080;         // Size in memory (??)
    B8 P_ALIGN   = 0x0000000000000001;         // P_VADDR = P_OFFSET % P_ALIGN ???
};

struct SectionHeader {
    B4 SH_NAME      = 0x00000000;
    B4 SH_TYPE      = 0x00000000 | SHT_PROGBITS;
    B8 SH_FLAGS     = 0x0000000000000000 | SHF_WRITE | SHF_EXECINSTR | SHF_ALLOC;
    B8 SH_ADDR      = 0x0000000000000000;
    B8 SH_OFFSET    = 0x0000000000000078;
    B8 SH_SIZE      = 0x0000000000100000;
    B4 SH_LINK      = 0x00000000;
    B4 SH_INFO      = 0x00000000;
    B8 SH_ADDRALIGN = 0x0000000000000000;
    B8 SH_ENTSIZE   = 0x0000000000000000;
};

extern const int ELFHDR_SIZE;
extern const int PRGHDR_SIZE;
extern const int PRGHDR_CNT;
extern const int HDR_SIZE;

extern const int GLOBL_DISPL;
extern const int ENTRY_POINT;

extern const int ELF_TEXT_OFFSET;
extern const int ELF_DATA_OFFSET;

extern const int ELF_BSS_OFFSET;
extern const int ELF_BSS_VADDR;
extern const int ELF_BSS_SIZE;

struct SectionShstrtab {
    const char names[2][10] = {
        "aaaa",
        "bbbb"
    };
};

#pragma pack(pop)


void build_elf(const char *prog, 
               const size_t prog_size,
               size_t entry_offset,
               FILE *file, 
               int global_data_size, 
               bool to_add_exit_code_zero = true);

void build_elf(const char *prog, 
               const size_t prog_size,
               size_t entry_offset,
               const char *filename,
               int global_data_size, 
               bool to_add_exit_code_zero = true);

#endif // ELF_BUILDER
