#include <stdio.h>
#include "parse_tree.h"

// extern FILE *yyin;
// int yylex();
void yyrestart (FILE *input_file  );
#include "syntax.tab.h"

int main(int argc, char** argv)
{
	if (argc <= 1) return 1;
	FILE* f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	int ret = yyparse();
	// printf("yyparse() returns %d", ret);

	print_syntax_tree(root, 0);
	delete_syntax_tree(root);

	return 0;
}
