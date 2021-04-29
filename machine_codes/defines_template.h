//
// Sponsored by Александр Дремов
//

#ifndef MACHINE_CODE_DEFINES
#define MACHINE_CODE_DEFINES

#define cmd_BREAKPOINT 0xCD, 0x03
#define cmd_COMMANDEND 0x100

#define cmd_SYSCALL    0x0F, 0x05
#define cmd_REGSNUM {{MAX_REG_INDEX}}

{{COMMAND_DEFINES}}

#define cmd_PUSH_IMM8  0x6A
#define cmd_PUSH_IMM16 0x68

#define cmd_CALL_REL32 0xE8
#define cmd_RET        0xC3

#define cmd_JE_REL8  0x74
#define cmd_JNE_REL8 0x75
#define cmd_JL_REL8  0x7C
#define cmd_JLE_REL8 0x7E
#define cmd_JG_REL8  0x7F
#define cmd_JGE_REL8 0x7D
#define cmd_JMP_REL8 0xEB

#define cmd_JE_REL32  0x0F, 0x84
#define cmd_JNE_REL32 0x0F, 0x85
#define cmd_JL_REL32  0x0F, 0x8C
#define cmd_JLE_REL32 0x0F, 0x8E
#define cmd_JG_REL32  0x0F, 0x8F
#define cmd_JGE_REL32 0x0F, 0x8D
#define cmd_JMP_REL32 0xE9

#define REG_RSP_DISPL(x) REG_RSP, (x)
#define REG_RBP_DISPL(x) REG_RBP, (x)

{{TABLES}}

#define cmd_MOV_RAX_MEM64 0x48, 0x8B, 0x04, 0x25 
#define cmd_MOV_RCX_MEM64 0x48, 0x8B, 0x0C, 0x25 
#define cmd_MOV_RDX_MEM64 0x48, 0x8B, 0x14, 0x25 
#define cmd_MOV_RBX_MEM64 0x48, 0x8B, 0x1C, 0x25 
#define cmd_MOV_RSP_MEM64 0x48, 0x8B, 0x24, 0x25 
#define cmd_MOV_RBP_MEM64 0x48, 0x8B, 0x2C, 0x25 
#define cmd_MOV_RSI_MEM64 0x48, 0x8B, 0x34, 0x25 
#define cmd_MOV_RDI_MEM64 0x48, 0x8B, 0x3C, 0x25 

#define cmd_MOV_R8_MEM64 0x4C, 0x8B, 0x04, 0x25 
#define cmd_MOV_R9_MEM64 0x4C, 0x8B, 0x0C, 0x25 
#define cmd_MOV_R10_MEM64 0x4C, 0x8B, 0x14, 0x25 
#define cmd_MOV_R11_MEM64 0x4C, 0x8B, 0x1C, 0x25 
#define cmd_MOV_R12_MEM64 0x4C, 0x8B, 0x24, 0x25 
#define cmd_MOV_R13_MEM64 0x4C, 0x8B, 0x2C, 0x25 
#define cmd_MOV_R14_MEM64 0x4C, 0x8B, 0x34, 0x25 
#define cmd_MOV_R15_MEM64 0x4C, 0x8B, 0x3C, 0x25 

constexpr static unsigned char cmd_MOV_REG_MEM64[cmd_REGSNUM][4] = {
{cmd_MOV_RAX_MEM64},
{cmd_MOV_RCX_MEM64},
{cmd_MOV_RDX_MEM64},
{cmd_MOV_RBX_MEM64},
{cmd_MOV_RSP_MEM64},
{cmd_MOV_RBP_MEM64},
{cmd_MOV_RSI_MEM64},
{cmd_MOV_RDI_MEM64},
{cmd_MOV_R8_MEM64},
{cmd_MOV_R9_MEM64},
{cmd_MOV_R10_MEM64},
{cmd_MOV_R11_MEM64},
{cmd_MOV_R12_MEM64},
{cmd_MOV_R13_MEM64},
{cmd_MOV_R14_MEM64},
{cmd_MOV_R15_MEM64},
};

#define cmd_MOV_MEM64_RAX 0x48, 0x89, 0x04, 0x25
#define cmd_MOV_MEM64_RCX 0x48, 0x89, 0x0C, 0x25
#define cmd_MOV_MEM64_RDX 0x48, 0x89, 0x14, 0x25
#define cmd_MOV_MEM64_RBX 0x48, 0x89, 0x1C, 0x25
#define cmd_MOV_MEM64_RSP 0x48, 0x89, 0x24, 0x25
#define cmd_MOV_MEM64_RBP 0x48, 0x89, 0x2C, 0x25
#define cmd_MOV_MEM64_RSI 0x48, 0x89, 0x34, 0x25
#define cmd_MOV_MEM64_RDI 0x48, 0x89, 0x3C, 0x25

#define cmd_MOV_MEM64_R8 0x4C, 0x89, 0x04, 0x25
#define cmd_MOV_MEM64_R9 0x4C, 0x89, 0x0C, 0x25
#define cmd_MOV_MEM64_R10 0x4C, 0x89, 0x14, 0x25
#define cmd_MOV_MEM64_R11 0x4C, 0x89, 0x1C, 0x25
#define cmd_MOV_MEM64_R12 0x4C, 0x89, 0x24, 0x25
#define cmd_MOV_MEM64_R13 0x4C, 0x89, 0x2C, 0x25
#define cmd_MOV_MEM64_R14 0x4C, 0x89, 0x34, 0x25
#define cmd_MOV_MEM64_R15 0x4C, 0x89, 0x3C, 0x25

constexpr static unsigned char cmd_MOV_MEM64_REG[cmd_REGSNUM][4] = {
{cmd_MOV_MEM64_RAX},
{cmd_MOV_MEM64_RCX},
{cmd_MOV_MEM64_RDX},
{cmd_MOV_MEM64_RBX},
{cmd_MOV_MEM64_RSP},
{cmd_MOV_MEM64_RBP},
{cmd_MOV_MEM64_RSI},
{cmd_MOV_MEM64_RDI},
{cmd_MOV_MEM64_R8},
{cmd_MOV_MEM64_R9},
{cmd_MOV_MEM64_R10},
{cmd_MOV_MEM64_R11},
{cmd_MOV_MEM64_R12},
{cmd_MOV_MEM64_R13},
{cmd_MOV_MEM64_R14},
{cmd_MOV_MEM64_R15},
};

#endif //MACHINE_CODE_DEFINES

