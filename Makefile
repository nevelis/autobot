all : compiler assembler

compiler : compiler-parser.o compiler-tokens.o botscript.o
	g++ -o $@ compiler-parser.o compiler-tokens.o botscript.o

compiler-parser.o : compiler-parser.tab.c compiler-parser.tab.h botscript.h
	g++ -g -c -o $@ compiler-parser.tab.c

botscript.o : botscript.cpp botscript.h compiler-parser.tab.h
	g++ -g -c -o $@ botscript.cpp

compiler-parser.tab.c compiler-parser.tab.h : compiler-parser.y
	bison -v -d compiler-parser.y

compiler-tokens.o : compiler-tokens.c botscript.h
	g++ -g -c -o $@ compiler-tokens.c

compiler-tokens.c : compiler-tokens.l compiler-parser.tab.h
	flex -o $@ compiler-tokens.l

assembler : assembler-parser.o assembler-tokens.o
	g++ -o $@ assembler-parser.o assembler-tokens.o

assembler-parser.o : assembler-parser.tab.c assembler-parser.tab.h
	g++ -g -c -o $@ assembler-parser.tab.c

assembler-parser.tab.c assembler-parser.tab.h : assembler-parser.y
	bison -v -d assembler-parser.y

assembler-tokens.o : assembler-tokens.c
	g++ -g -c -o $@ assembler-tokens.c

assembler-tokens.c : assembler-tokens.l
	flex -o $@ assembler-tokens.l


clean :
	rm -f *.o *.output *.tab.* *tokens.c compiler assembler


