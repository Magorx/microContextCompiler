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


#endif //MACHINE_CODE_DEFINES

