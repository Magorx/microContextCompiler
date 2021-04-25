#!/usr/bin/env python3
# -*- coding: utf-8 -*-

registers = [
    ("rax", ("A", "A")),
    ("rcx", ("A", "A")),
    ("rdx", ("A", "A")),
    ("rbx", ("A", "A")),
    ("rsp", ("A", "A")),
    ("rbp", ("A", "A")),
    ("rsi", ("A", "A")),
    ("rdi", ("A", "A")),
    ("r8", ("B", "R")),
    ("r9", ("B", "R")),
    ("r10", ("B", "R")),
    ("r11", ("B", "R")),
    ("r12", ("B", "R")),
    ("r13", ("B", "R")),
    ("r14", ("B", "R")),
    ("r15", ("B", "R")),
]

regToRegCodes = {
    "AA": 0x48,
    "BA": 0x49,
    "AR": 0x4C,
    "BR": 0x4D
}

prefixRegs = {"A": 0x48, "B": 0x49}


def generalCommandOneReg(command, prefix, start=0xC0) -> str:
    table = {}
    if not isinstance(command, list):
        command = [command]
    startCode = start
    for num, elem in enumerate(registers):
        if startCode > 0xFF:
            startCode = start
        table[elem[0]] = [prefixRegs[elem[1][0]], *command, startCode]
        startCode += 1

    ret = "\n"
    for num, elem in enumerate(table):
        ret += "#define cmd_" + prefix.upper() + elem.upper() + " " + ", ".join(map(hex, table[elem])) + "\n"
    return ret


def registerDefines() -> str:
    ret = "\n"
    for num, elem in enumerate(registers):
        ret += "#define REG_" + elem[0].upper() + " " + str(num) + "\n"
    return ret


def generalCommandAddReg(command, prefix, prefixType=False) -> str:
    ret = "\n"
    for num, elem in enumerate(registers):
        com = command + num
        if num >= 8:
            com -= 8

        ret += "#define cmd_" + prefix.upper() + elem[0].upper() + " " + hex(prefixRegs[elem[1][0]]) + ", " + hex(com) + "\n"
    return ret


def registerPopDefines() -> str:
    ret = "\n"
    start = 0x58
    code = start
    for num, elem in enumerate(registers):
        if (num == 8):
            code = start
        if (num <= 7):
            ret += "#define cmd_POP_" + elem[0].upper() + " " + hex(code) + ", 0x90 \n"
        else:
            ret += "#define cmd_POP_" + elem[0].upper() + " 0x41, " + hex(code) + "\n"
        code+=1
    return ret


def registerPushDefines() -> str:
    ret = "\n"
    start = 0x50
    code = start
    for num, elem in enumerate(registers):
        if (num == 8):
            code = start
        if (num <= 7):
            ret += "#define cmd_PUSH_" + elem[0].upper() + " " + hex(code) + ", 0x90 \n"
        else:
            ret += "#define cmd_PUSH_" + elem[0].upper() + " 0x41, " + hex(code) + "\n"
        code+=1
    return ret


def idivDefines() -> str:
    return generalCommandOneReg(0xF7, "idiv_", 0xF8)


def generalCommandTwoRegs(command, start=0xC0) -> dict:
    table = {}
    if not isinstance(command, list):
        command = [command]
    startCode = start
    for i in range(len(registers)):
        if (i == 8):
            startCode = start
        for j in range(len(registers)):
            table[registers[j][0] + registers[i][0]] = [regToRegCodes[registers[j][1][0] + registers[i][1][1]],
                                                        *command, startCode]
            startCode += 1
            if (j == 7):
                startCode -= 8
    return table


def generalCommandTwoRegsRev(command, start=0xC0) -> dict:
    table = {}
    if not isinstance(command, list):
        command = [command]
    startCode = start
    modifier = 0
    for i in range(len(registers)):
        if (i == 8):
            startCode = start
        modifier = 0
        for j in range(len(registers)):
            table[registers[i][0] + registers[j][0]] = [prefixRegs[registers[i][1][0]] + modifier,
                                                        *command, startCode]
            startCode += 1
            if (j == 7):
                modifier = 1
                startCode -= 8
    return table


def addCommandDefines() -> str:
    table = {}
    addOpCodeimm8 = 0x83
    addOpCodeimm32 = 0x81
    startCode = 0xC0
    strDefine = "\n"
    for i in range(len(registers)):
        if i == 8:
            startCode = 0xC0
        table[registers[i][0]] = [prefixRegs[registers[i][1][0]], addOpCodeimm8, startCode]
        startCode += 1

    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_ADD_" + label + "IMM8" + " " + ", ".join(map(hex, opcode)) + "\n"

    table = {}

    strDefine += "\n"
    startCode = 0xC0
    for i in range(len(registers)):
        if i == 8:
            startCode = 0xC0
        table[registers[i][0]] = [prefixRegs[registers[i][1][0]], addOpCodeimm32, startCode]
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
    res = ""
    startCode = 0xB8
    codeNow = startCode
    for i in range(len(registers)):
        code = [codeNow]
        if i == 8:
            codeNow = startCode
        if i >= 8:
            code = [0x41, codeNow]
        res += "#define cmd_MOV_" + registers[i][0].upper() + "IMM32 " + ", ".join(map(hex, code)) + "\n"
        codeNow += 1
    return res

def movImm64Defines() -> str:
    res = ""
    for i in range(len(registers)):
        if (i < 8):
            res += "#define cmd_MOV_" + registers[i][0].upper() + "IMM64 " + ", ".join(map(hex, [0x48, 0xc7, 0xc0 + i])) + "\n"
        else:
            res += "#define cmd_MOV_" + registers[i][0].upper() + "IMM64 " + ", ".join(map(hex, [0x49, 0xc7, 0xc0 + i - 8])) + "\n"
    return res


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
    for i in range(len(registers)):
        if i == 8:
            startCode = start
        table[registers[i][0]] = [prefixRegs[registers[i][1][0]], subOpCodeimm8, startCode]
        startCode += 1

    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_SUB_" + label + "IMM8" + " " + ", ".join(map(hex, opcode)) + "\n"

    table = {}

    strDefine += "\n"
    startCode = start
    for i in range(len(registers)):
        if i == 8:
            startCode = start
        table[registers[i][0]] = [prefixRegs[registers[i][1][0]], subOpCodeimm32, startCode]
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
    for i in range(len(registers)):
        if i == 8:
            startCode = start
        table[registers[i][0]] = [prefixRegs[registers[i][1][0]], imulOpCodeimm8, startCode]
        startCode += 9

    for label, opcode in table.items():
        label = label.upper()
        strDefine += "#define cmd_IMUL_" + label + "IMM8" + " " + ", ".join(map(hex, opcode)) + "\n"

    table = {}

    strDefine += "\n"
    startCode = start
    for i in range(len(registers)):
        if i == 8:
            startCode = start
        table[registers[i][0]] = [prefixRegs[registers[i][1][0]], imulOpCodeimm32, startCode]
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
    for i in range(len(registers)):
        if (i == 8):
            startCode = 0x80
        for j in range(len(registers)):
            table["MEM_" + registers[j][0] + "_displ32" + registers[i][0]] = [
                regToRegCodes[registers[j][1][0] + registers[i][1][1]],
                movOpCode, startCode]
            if registers[j][0] == "rsp":
                table["MEM_" + registers[j][0] + "_displ32" + registers[i][0]].append(0x24)
            startCode += 1
            if (j == 7):
                startCode -= 8

    startCode = 0x40
    for i in range(len(registers)):
        if (i == 8):
            startCode = 0x40
        for j in range(len(registers)):
            table["MEM_" + registers[j][0] + "_displ8" + registers[i][0]] = [
                regToRegCodes[registers[j][1][0] + registers[i][1][1]],
                movOpCode, startCode]
            if registers[j][0] == "rsp":
                table["MEM_" + registers[j][0] + "_displ8" + registers[i][0]].append(0x24)
            startCode += 1
            if (j == 7):
                startCode -= 8

    movRegToMemCode = 0x8B
    startCode = 0x80
    for i in range(len(registers)):
        if (i == 8):
            startCode = 0x80
        for j in range(len(registers)):
            table[registers[i][0] + registers[j][0] + "_mem_displ32"] = [
                regToRegCodes[registers[j][1][0] + registers[i][1][1]],
                movRegToMemCode, startCode]
            if registers[j][0] == "rsp":
                table[registers[i][0] + registers[j][0] + "_mem_displ32"].append(0x24)

            startCode += 1
            if (j == 7):
                startCode -= 8

    startCode = 0x40
    for i in range(len(registers)):
        if (i == 8):
            startCode = 0x40
        for j in range(len(registers)):
            table[registers[i][0] + registers[j][0] + "_mem_displ8"] = [
                regToRegCodes[registers[j][1][0] + registers[i][1][1]],
                movRegToMemCode, startCode]
            if (registers[j][0] == "rsp"):
                table[registers[i][0] + registers[j][0] + "_mem_displ8"].append(0x24)
            startCode += 1
            if (j == 7):
                startCode -= 8

    return table


def movTable() -> str:
    res = "constexpr static unsigned char cmd_MOV_TABLE[cmd_REGSNUM][cmd_REGSNUM][3] = {\n"
    for outReg, _ in registers:
        res += "{ "
        for inReg, _ in registers:
            res += "{" + "cmd_MOV_" + outReg.upper() + inReg.upper() + "},"
        res += "},\n"
    return res + "};\n\n"


def pushPopTable() -> str:
    res = "constexpr static unsigned char cmd_PUSH_TABLE[cmd_REGSNUM][2] = {\n"
    for outReg, _ in registers:
        res += "{cmd_PUSH_" + outReg.upper() + "},\n"
    res += "};\n\n"
    res += "constexpr static unsigned char cmd_POP_TABLE[cmd_REGSNUM][2] = {\n"
    for outReg, _ in registers:
        res += "{cmd_POP_" + outReg.upper() + "},\n"
    res += "};\n\n"
    return res

def idivPopTable() -> str:
    res = "constexpr static unsigned char cmd_IDIVTABLE[cmd_REGSNUM][3] = {\n"
    for outReg, _ in registers:
        res += "{cmd_IDIV_" + outReg.upper() + "},\n"
    res += "};\n\n"
    return res

def movImm32Table() -> str:
    res = "constexpr static unsigned char cmd_MOVIMM32TABLE[cmd_REGSNUM][3] = {\n"
    for outReg, _ in registers:
        res += "{cmd_MOV_" + outReg.upper() + "IMM32},\n"
    res += "};\n\n"
    return res

def movImm64Table() -> str:
    res = "constexpr static unsigned char cmd_MOVIMM64TABLE[cmd_REGSNUM][3] = {\n"
    for outReg, _ in registers:
        res += "{cmd_MOV_" + outReg.upper() + "IMM64},\n"
    res += "};\n\n"
    return res


def movRspMemTable() -> str:
    res = "constexpr static unsigned char cmd_MOV_REG_RSPMEM_DISPL32[cmd_REGSNUM][4] = {\n"
    for outReg, _ in registers:
        res += "{cmd_MOV_" + outReg.upper() + "RSP_MEM_DISPL32},\n"
    res += "};\n\n"
    res += "constexpr static unsigned char cmd_MOV_RSPMEM_REG_DISPL32[cmd_REGSNUM][4] = {\n"
    for outReg, _ in registers:
        res += "{cmd_MOV_MEM_RSP_DISPL32" + outReg.upper() + "},\n"
    res += "};\n\n"
    return res


def movRbpMemTable() -> str:
    res = "constexpr static unsigned char cmd_MOV_REG_RBPMEM_DISPL32[cmd_REGSNUM][3] = {\n"
    for outReg, _ in registers:
        res += "{cmd_MOV_" + outReg.upper() + "RBP_MEM_DISPL32},\n"
    res += "};\n\n"
    res += "constexpr static unsigned char cmd_MOV_RBPMEM_REG_DISPL32[cmd_REGSNUM][3] = {\n"
    for outReg, _ in registers:
        res += "{cmd_MOV_MEM_RBP_DISPL32" + outReg.upper() + "},\n"
    res += "};\n\n"
    return res


def twoRegTable(name, prefix, size=3) -> str:
    res = "constexpr static unsigned char cmd_" + name.upper() + "[cmd_REGSNUM][cmd_REGSNUM][" + str(size) + "] = {\n"
    for outReg, _ in registers:
        res += "{ "
        for inReg, _ in registers:
            res += "{" + prefix + outReg.upper() + inReg.upper() + "},"
        res += "},\n"
    res += "};\n\n"
    return res


def xchgRaxTable() -> str:
    res = "constexpr static unsigned char cmd_XCHG_RAXTABLE[cmd_REGSNUM][2] = {\n"
    for outReg, _ in registers:
        res += "{cmd_XCHG_RAX" + outReg.upper() + "},\n"
    res += "};\n\n"
    return res


def addTable() -> str:
    return twoRegTable("ADDTABLE", "cmd_ADD_")


def subTable() -> str:
    return twoRegTable("SUBTABLE", "cmd_SUB_")


def xorTable() -> str:
    return twoRegTable("XORTABLE", "cmd_XOR_")


def testTable() -> str:
    return twoRegTable("TESTTABLE", "cmd_TEST_")


def cmpTable() -> str:
    return twoRegTable("CMPTABLE", "cmd_CMP_", 3)


def imulTable() -> str:
    return twoRegTable("IMULTABLE", "cmd_IMUL_", 4)


def xchgRax() -> str:
    return generalCommandAddReg(0x90, "XCHG_RAX", True)


if "__main__" == __name__:
    maxOpcode = 5

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
                                movImm64Defines() +
                                registerPopDefines() +
                                registerPushDefines() +
                                addCommandDefines() +
                                subCommandDefines() +
                                imulCommandDefines() +
                                idivDefines() +
                                xorDefines() +
                                cmpDefines() +
                                testDefines() +
                                xchgRax())
    template = template.replace("{{MAX_REG_INDEX}}", str(len(registers)))

    template = template.replace("{{TABLES}}",
                                movTable() +
                                pushPopTable() +
                                movRspMemTable() +
                                movRbpMemTable() +
                                subTable() +
                                addTable() +
                                xorTable() +
                                testTable() +
                                cmpTable() +
                                imulTable() +
                                xchgRaxTable()+
                                idivPopTable() +
                                movImm32Table() +
                                movImm64Table())

    with open("defines.h", "w") as outputFile:
        outputFile.write(template)
