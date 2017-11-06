parser : main.c syntax.tab lex.yy.c
	gcc main.c lex.yy.c syntax.tab.c -lfl -ly -o parser

lex.yy.c : lexical.l syntax.tab
	flex lexical.l
syntax.tab : syntax.y
	bison -d -v syntax.y
clean:
	rm -rf parser lex.yy.c syntax.tab*
