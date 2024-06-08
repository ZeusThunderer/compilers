ast-maker: fb3.h fb3.c lexer.l parser.y
	bison -d parser.y
	flex lexer.l
	gcc -g -o $@  parser.tab.c lex.yy.c -lfl
.PHONY: clean

clean:
	rm -f ast-maker lex.yy.c parser.tab.c lexer.tab.h parser.tab.h
