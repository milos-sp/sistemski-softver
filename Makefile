SRCS = parser.cpp lexer.cpp src/assembler.cpp src/symbol.cpp src/section.cpp src/literalPool.cpp src/relocation.cpp
CC = g++
SRCSL = src/linker/main.cpp src/symbol.cpp src/linker/linker.cpp src/section.cpp src/linker/scanner.cpp src/relocation.cpp

all: $(SRCS) inc/assembler.hpp inc/symbol.hpp inc/section.hpp inc/literalPool.hpp inc/relocation.hpp
	$(CC) $(SRCS) -lfl -o asembler

parser.cpp parser.hpp: misc/parser.y inc/assembler.hpp inc/symbol.hpp inc/section.hpp
	bison -d misc/parser.y

lexer.cpp: misc/flex.l parser.hpp
	flex misc/flex.l

linker: $(SRCSL) inc/symbol.hpp inc/section.hpp inc/linker/linker.hpp inc/linker/scanner.hpp inc/relocation.hpp
	$(CC) $(SRCSL) -lfl -o linker

emulator: src/emulator/main.cpp src/emulator/emulator.cpp inc/emulator/emulator.hpp
	$(CC) src/emulator/main.cpp src/emulator/emulator.cpp -lfl -o emulator
clean:
	rm -rf *.o lexer.cpp lexer.hpp parser.cpp parser.hpp asembler