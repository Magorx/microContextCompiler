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
CFLAGS = $(STANDARD) $(WARNINGS) -lm -std=c++17

all: kncc

update: all
	mv $(CUR_PROG) bin

kncc: main.cpp compiler.o id_table_scope.o id_table.o compiler_options.o recursive_parser.o lexical_parser.o lex_token.o announcement.o code_node.o opcodes.h 
	$(CPP) $(CFLAGS) main.cpp compiler.o recursive_parser.o code_node.o compiler_options.o lex_token.o lexical_parser.o id_table.o id_table_scope.o $(G)/announcement.o -o kncc

%.o : %.cpp
	$(CPP) $(C_FLAGS) -c $< -o $@

announcement.o: $(GC)/announcement.h $(GC)/announcement.c
	make -C general announcement.o

run: all
	./$(CUR_PROG)

run_: all
	./$(CUR_PROG)
	kasm out.kc out.tf
	kspu out.tf

comp: all
	kasm out.kc out.tf

comp_run: comp out.kc
	kspu out.tf

crun:
	kspu out.tf

valg: all
	valgrind --leak-check=full --show-leak-kinds=all -s ./$(CUR_PROG)

stable: all
	cp ./$(CUR_PROG) ./bin

clean:
	rm *.o
