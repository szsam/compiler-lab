#include <stdio.h>
#include "parse_tree.h"

// extern FILE *yyin;
// int yylex();
void yyrestart (FILE *input_file  );
#include "syntax.tab.h"

extern int error_occurred;

#include <iostream>
using namespace std;

#include "declaration.h"
extern SP<Program> ast_root;

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
	// yydebug = 1;
	
	// cout << ast_root.get() << endl;	

	yyparse();

//	if (!error_occurred)
		// print_parse_tree(root, 0);
	delete_parse_tree(root);

	cout << "AST root: " << ast_root.get() << endl;	

	return 0;
}
