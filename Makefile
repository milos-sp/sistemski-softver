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

clean:
	rm -rf *.o lexer.cpp lexer.hpp parser.cpp parser.hpp asembler

# parser.cpp parser.hpp: misc/parser.y
# 	bison -d misc/parser.y

# lexer.cpp: misc/flex.l parser.hpp
# 	flex misc/flex.l

# asembler: lexer.cpp parser.cpp parser.hpp
# 	g++ parser.cpp lexer.cpp src/assembler.cpp -lfl -o asembler