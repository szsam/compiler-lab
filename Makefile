parser : main.c syntax.tab lex.yy.c parse_tree.c parse_tree.h
	g++ -std=c++11 -g main.c lex.yy.c syntax.tab.c parse_tree.c -lfl -ly -o parser

lex.yy.c : lexical.l syntax.tab
	flex lexical.l
syntax.tab : syntax.y
	bison -d -v -t syntax.y
clean:
	rm -rf parser lex.yy.c syntax.tab* syntax.output
