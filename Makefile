CUR_PROG = kncc

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

all: kncc

update: all
	mv $(CUR_PROG) bin

kncc: main.cpp compiler.o micro_obj.o id_table_scope.o id_table.o compiler_options.o recursive_parser.o lexical_parser.o lex_token.o announcement.o code_node.o opcodes.h byte_buffer.o machine_codes/defines.h reg_manager.o debug.o hashtable.o ht_node.h linker.o elf_builder.o
	$(CPP) $(CFLAGS) main.cpp micro_obj.o compiler.o recursive_parser.o code_node.o compiler_options.o lex_token.o lexical_parser.o id_table.o id_table_scope.o byte_buffer.o $(G)/announcement.o reg_manager.o $(G)/debug.o hashtable.o linker.o elf_builder.o -o kncc

%.o : %.cpp %.h
	$(CPP) $(C_FLAGS) -c $< -o $@

announcement.o: $(GC)/announcement.h $(GC)/announcement.c
	make -C general announcement.o

debug.o: $(GC)/debug.h $(GC)/debug.c
	make -C general debug.o

run: all
	./$(CUR_PROG)

run_: all
	./$(CUR_PROG)
	@printf "[-----]\n"
	./elf
	@printf "\n[-----]\n"

comp: all
	kasm out.kc out.tf

comp_run: comp out.kc
	kspu out.tf

crun: all
	./kncc
	./elf

valg: all
	valgrind --leak-check=full --show-leak-kinds=all -s ./$(CUR_PROG)

stable: all
	cp ./$(CUR_PROG) ./bin

clean:
	rm *.o

elf: all
	./kncc
	xxd elf
	readelf -h elf
	readelf -l elf
