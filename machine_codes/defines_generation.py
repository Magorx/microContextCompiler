#!/usr/bin/env python3
# -*- coding: utf-8 -*-

class Register:
	r = "err"
	i = "err"
	o = "err"

	def __init__(self, name_, in_code_, out_code_):
		self.r = name_
		self.i = in_code_
		self.o = out_code_


a = Register('name', 'a', 'b')

REGS = [
    Register("rax", "A", "A"),
    Register("rcx", "A", "A"),
    Register("rdx", "A", "A"),
    Register("rbx", "A", "A"),
    Register("rsp", "A", "A"),
    Register("rbp", "A", "A"),
    Register("rsi", "A", "A"),
    Register("rdi", "A", "A"),
    Register("r8",  "B", "R"),
    Register("r9",  "B", "R"),
    Register("r10", "B", "R"),
    Register("r11", "B", "R"),
    Register("r12", "B", "R"),
    Register("r13", "B", "R"),
    Register("r14", "B", "R"),
    Register("r15", "B", "R"),
]

REGS_DICT = dict((reg.r, reg) for reg in REGS)

RR_CODE = {
    "AA": 0x48,
    "BA": 0x49,
    "AR": 0x4C,
    "BR": 0x4D
}

R_PREFIX = {"A": 0x48, "B": 0x49}


def generalCommandOneReg(command, prefix, start=0xC0) -> str:
    table = {}
    if not isinstance(command, list):
        command = [command]
    startCode = start
    for num, elem in enumerate(REGS):
        if startCode > 0xFF:
            startCode = start
        table[elem.r] = [R_PREFIX[elem.i], *command, startCode]
        startCode += 1

    ret = "\n"
    for num, elem in enumerate(table):
        ret += "#define cmd_" + prefix.upper() + elem.upper() + " " + ", ".join(map(hex, table[elem])) + "\n"
    return ret


def registerDefines() -> str:
    ret = "\n"
    for num, elem in enumerate(REGS):
        ret += "#define REG_" + elem.r.upper() + " " + str(num) + "\n"
    return ret


def generalCommandAddReg(command, prefix) -> str:
    ret = "\n"
    for num, elem in enumerate(REGS):
        ret += "#define cmd_" + prefix.upper() + elem.r.upper() + " " + hex(command + num) + "\n"
    return ret


def registerPopDefines() -> str:
    return generalCommandAddReg(0x58, "pop_")


def registerPushDefines() -> str:
    return generalCommandAddReg(0x50, "push_")


def idivDefines() -> str:
    return generalCommandOneReg(0xF7, "idiv_", 0xF8)


def generalCommandTwoRegs(command, start=0xC0) -> dict:
    table = {}
    if not isinstance(command, list):
        command = [command]
    startCode = start
    for i in range(len(REGS)):
        if (i == 8):
            startCode = start
        for j in range(len(REGS)):
            table[REGS[j].r + REGS[i].r] = [RR_CODE[REGS[j].i + REGS[i].o], *command, startCode]
            startCode += 1
            if (j == 7):
                startCode -= 8
    return table


def generalCommandTwoRegsRev(command, start=0xC0) -> dict:
    table = {}
    if not isinstance(command, list):
        command = [command]
    startCode = start
    for i in range(len(REGS)):
        if (i == 8):
            startCode = start
        for j in range(len(REGS)):
            table[REGS[i].r + REGS[j].r] = [RR_CODE[REGS[i].i + REGS[j].o], *command, startCode]
            startCode += 1
            if (j == 7):
                startCode -= 8
    return table


def addCommandDefines() -> str:
    table = {}
    addOpCodeimm8 = 0x83
    addOpCodeimm32 = 0x81
    startCode = 0xC0
    strDefine = "\n"
    for i in range(len(REGS)):
        if i == 8:
            startCode = 0xC0
        table[REGS[i].r] = [R_PREFIX[REGS[i].i], addOpCodeimm8, startCode]
        startCode += 1

    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_ADD_" + label + "IMM8" + " " + ", ".join(map(hex, opcode)) + "\n"

    table = {}

    strDefine += "\n"
    startCode = 0xC0
    for i in range(len(REGS)):
        if i == 8:
            startCode = 0xC0
        table[REGS[i].r] = [R_PREFIX[REGS[i].i], addOpCodeimm32, startCode]
        startCode += 1
    table["rax"] = [0x48, 0x05]

    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_ADD_" + label + "IMM32" + " " + ", ".join(map(hex, opcode)) + "\n"

    table = generalCommandTwoRegs(0x01)
    strDefine += "\n"
    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_ADD_" + label + " " + ", ".join(map(hex, opcode)) + "\n"

    return strDefine


def movImmDefines() -> str:
    ret = ""
    startCode = 0xB8
    codeNow = startCode
    for i in range(len(REGS)):
        code = [codeNow]
        if i == 8:
            codeNow = startCode
        if i >= 8:
            code = [0x41, codeNow]
        ret += "#define cmd_MOV_" + REGS[i].r.upper() + "IMM32 " + ", ".join(map(hex, code)) + "\n"
        codeNow += 1
    return ret


def xorDefines() -> str:
    table = generalCommandTwoRegs(0x31)
    strDefine = "\n"
    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_XOR_" + label + " " + ", ".join(map(hex, opcode)) + "\n"
    return strDefine

def cmpDefines() -> str:
    table = generalCommandTwoRegs(0x39)
    strDefine = "\n"
    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_CMP_" + label + " " + ", ".join(map(hex, opcode)) + "\n"
    return strDefine

def testDefines() -> str:
    table = generalCommandTwoRegs(0x85)
    strDefine = "\n"
    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_TEST_" + label + " " + ", ".join(map(hex, opcode)) + "\n"
    return strDefine


def subCommandDefines() -> str:
    table = {}
    subOpCodeimm8 = 0x83
    subOpCodeimm32 = 0x81
    start = 0xE8
    startCode = start
    strDefine = "\n"
    for i in range(len(REGS)):
        if i == 8:
            startCode = start
        table[REGS[i].r] = [R_PREFIX[REGS[i].i], subOpCodeimm8, startCode]
        startCode += 1

    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_SUB_" + label + "IMM8" + " " + ", ".join(map(hex, opcode)) + "\n"

    table = {}

    strDefine += "\n"
    startCode = start
    for i in range(len(REGS)):
        if i == 8:
            startCode = start
        table[REGS[i].r] = [R_PREFIX[REGS[i].i], subOpCodeimm32, startCode]
        startCode += 1
    table["rax"] = [0x48, 0x2D]

    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_SUB_" + label + "IMM32" + " " + ", ".join(map(hex, opcode)) + "\n"

    table = generalCommandTwoRegs(0x29, 0xC0)
    strDefine += "\n"
    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_SUB_" + label + " " + ", ".join(map(hex, opcode)) + "\n"

    return strDefine


def imulCommandDefines() -> str:
    table = {}
    imulOpCodeimm8 = 0x6B
    imulOpCodeimm32 = 0x69
    start = 0xC0
    startCode = start
    strDefine = "\n"
    for i in range(len(REGS)):
        if i == 8:
            startCode = start
        table[REGS[i].r] = [R_PREFIX[REGS[i].i], imulOpCodeimm8, startCode]
        startCode += 9

    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_IMUL_" + label + "IMM8" + " " + ", ".join(map(hex, opcode)) + "\n"

    table = {}

    strDefine += "\n"
    startCode = start
    for i in range(len(REGS)):
        if i == 8:
            startCode = start
        table[REGS[i].r] = [R_PREFIX[REGS[i].i], imulOpCodeimm32, startCode]
        startCode += 9
    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_IMUL_" + label + "IMM32" + " " + ", ".join(map(hex, opcode)) + "\n"

    table = generalCommandTwoRegsRev([0x0F, 0xAF], 0xC0)
    strDefine += "\n"
    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_IMUL_" + label + " " + ", ".join(map(hex, opcode)) + "\n"

    return strDefine


def genMovTable() -> dict:
    movOpCode = 0x89
    table = generalCommandTwoRegs(movOpCode)

    startCode = 0x80
    for i in range(len(REGS)):
        if (i == 8):
            startCode = 0x80
        for j in range(len(REGS)):
            table["MEM_" + REGS[j].r + "_displ32" + REGS[i].r] = [
                RR_CODE[REGS[j].i + REGS[i].o],
                movOpCode, startCode]
            if REGS[j].r == "rsp":
                table["MEM_" + REGS[j].r + "_displ32" + REGS[i].r].append(0x24)
            startCode += 1
            if (j == 7):
                startCode -= 8

    startCode = 0x40
    for i in range(len(REGS)):
        if (i == 8):
            startCode = 0x40
        for j in range(len(REGS)):
            table["MEM_" + REGS[j].r + "_displ8" + REGS[i].r] = [
                RR_CODE[REGS[j].i + REGS[i].o],
                movOpCode, startCode]
            if REGS[j].r == "rsp":
                table["MEM_" + REGS[j].r + "_displ8" + REGS[i].r].append(0x24)
            startCode += 1
            if (j == 7):
                startCode -= 8

    movRegToMemCode = 0x8B
    startCode = 0x80
    for i in range(len(REGS)):
        if (i == 8):
            startCode = 0x80
        for j in range(len(REGS)):
            table[REGS[i].r + REGS[j].r + "_mem_displ32"] = [
                RR_CODE[REGS[j].i + REGS[i].o],
                movRegToMemCode, startCode]
            if REGS[j].r == "rsp":
                table[REGS[i].r + REGS[j].r + "_mem_displ32"].append(0x24)

            startCode += 1
            if (j == 7):
                startCode -= 8

    startCode = 0x40
    for i in range(len(REGS)):
        if (i == 8):
            startCode = 0x40
        for j in range(len(REGS)):
            table[REGS[i].r + REGS[j].r + "_mem_displ8"] = [
                RR_CODE[REGS[j].i + REGS[i].o],
                movRegToMemCode, startCode]
            if (REGS[j].r == "rsp"):
                table[REGS[i].r + REGS[j].r + "_mem_displ8"].append(0x24)
            startCode += 1
            if (j == 7):
                startCode -= 8

    return table


def movTable() -> str:
    ret = "constexpr cmd_MovCommand cmd_MOV_TABLE[cmd_REGSNUM][cmd_REGSNUM] = {\n"
    for outReg in REGS:
        ret += "{ "
        for inReg in REGS:
            ret += "{" + "cmd_MOV_" + outReg.r.upper() + inReg.r.upper() + "},"
        ret += "},\n"
    return ret + "};\n\n"


def pushPopTable() -> str:
    ret = "constexpr char cmd_PUSH_TABLE[cmd_REGSNUM] = {\n"
    for outReg in REGS:
        ret += "cmd_PUSH_" + outReg.r.upper() + ",\n"
    ret += "};\n\n"
    ret += "constexpr char cmd_POP_TABLE[cmd_REGSNUM] = {\n"
    for outReg in REGS:
        ret += "cmd_POP_" + outReg.r.upper() + ",\n"
    ret += "};\n\n"
    return ret


if "__main__" == __name__:
    with open("defines_template.h", "r") as file:
        template = file.read()
    movRegTable = genMovTable()
    movDefines = ""

    for label, opcode in movRegTable.items():
        label = label.upper()
        movDefines = movDefines + "#define cmd_MOV_" + label + " " + ", ".join(map(hex, opcode)) + "\n"

    template = template.replace("{{COMMAND_DEFINES}}",
                                registerDefines() +
                                movDefines +
                                movImmDefines() +
                                registerPopDefines() +
                                registerPushDefines() +
                                addCommandDefines() +
                                subCommandDefines() +
                                imulCommandDefines() +
                                idivDefines() +
                                xorDefines()+
                                cmpDefines()+
                                testDefines())
    template = template.replace("{{MAX_REG_INDEX}}", str(len(REGS)))

    template = template.replace("{{TABLES}}", movTable() + pushPopTable())

    with open("defines.h", "w") as outputFile:
        outputFile.write(template)
