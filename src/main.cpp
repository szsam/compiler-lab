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

#include "dec_check_visitor.h"
#include "ir_gen_visitor.h"

#include <fstream>

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

	DecCheckVisitor dec_checker;
	ast_root->accept(dec_checker);

	InterCodeGenVisitor inter_code_gen;
	ast_root->accept(inter_code_gen);

	ofstream fout("a.ir");
	for (const auto &p_code : ast_root->code)
	{
		fout << *p_code << endl;
	}

	return 0;
}
