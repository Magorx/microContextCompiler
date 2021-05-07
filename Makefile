CUR_PROG = kmcc

ifndef VERBOSE
.SILENT:
endif

G = general
GC = $(G)/c
GCPP = $(G)/cpp

CC = gcc
CPP = g++

WARNINGS = -Wall -Wextra -Wno-multichar
STANDARD = 
OPTIMIZATION = -O3
CFLAGS = $(STANDARD) $(WARNINGS) $(OPTIMIZATION) -lm -std=c++17

S = src/

all: kmcc

update: all
	cp $(CUR_PROG) examples

kmcc: main.cpp compiler.o micro_obj.o id_table_scope.o id_table.o compiler_options.o recursive_parser.o lexical_parser.o lex_token.o $(S)announcement.o $(S)debug.o code_node.o $(S)opcodes.h byte_buffer.o $(S)machine_codes/defines.h reg_manager.o hashtable.o $(S)ht_node.h linker.o elf_builder.o
	$(CPP) $(CFLAGS) main.cpp micro_obj.o compiler.o recursive_parser.o code_node.o compiler_options.o lex_token.o lexical_parser.o id_table.o id_table_scope.o byte_buffer.o $(S)$(G)/announcement.o reg_manager.o $(S)$(G)/debug.o hashtable.o linker.o elf_builder.o -o kmcc

%.o : $(S)%.cpp $(S)%.h
	$(CPP) $(C_FLAGS) -c $< -o $@

$(S)announcement.o: $(S)$(GC)/announcement.h $(S)$(GC)/announcement.c
	make -C $(S)general announcement.o

$(S)debug.o: $(S)$(GC)/debug.h $(S)$(GC)/debug.c
	make -C $(S)general debug.o

run: all
	./$(CUR_PROG)
clean:
	rm *.o

elf: all
	./kmcc
	xxd elf
	readelf -h elf
	readelf -l elf
