# Makefile for prog
CC = gcc
LEX = flex
YACC = bison
CFLAGS = -g -DYYDEBUG=1
LDFLAGS = -L/opt/homebrew/opt/flex/lib -L/opt/homebrew/opt/bison/lib
CPPFLAGS = -I/opt/homebrew/opt/flex/include

PROGRAM = prog

.PHONY: all clean debug graph help
default: all

help:
	@echo all - build the program
	@echo clean - remove the program and temporary files
	@echo debug - build the program with debug flags
	@echo graph - generate a graph of the parser
	@echo help - display this message

all: clean ${PROGRAM}

${PROGRAM}: ast.h parser.tab.c lexer.c main.c
	${CC} -o ${PROGRAM} main.c ast.c parser.tab.c lexer.c

parser.tab.c parser.tab.h: parser.y
	${YACC} -d -Wcounterexamples parser.y

lexer.c: lexer.l
	${LEX} -o lexer.c lexer.l

graph: parser.y
	${YACC} -d -Wcounterexamples --graph parser.y
	dot -Tpng parser.gv -o parser.png

clean:
	rm -f ${PROGRAM} parser.tab.c parser.tab.h lexer.c

debug: ast.h parser.y lexer.l
	${LEX} -d -o lexer.c lexer.l
	${YACC} --debug -d -Wcounterexamples parser.y
	${CC} -o ${PROGRAM} ${CFLAGS} main.c ast.c parser.tab.c lexer.c