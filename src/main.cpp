#include <cstdio>
#include <iostream>
#include <fstream>

using namespace std;

// extern FILE *yyin;
// int yylex();
void yyrestart (FILE *input_file  );
#include "syntax.tab.h"

#include "parse_tree.h"
#include "declaration.h"
#include "dec_check_visitor.h"
#include "type_check_visitor.h"
#include "ir_gen_visitor.h"

extern int error_occurred;
extern SP<Program> ast_root;

int main(int argc, char** argv)
{
	if (argc != 3) 
	{
		fprintf(stderr, "Usage: %s input_file output_file\n", argv[0]);
		return 1;
	}

	FILE* f = fopen(argv[1], "r");
	if (!f)
	{
		perror(argv[1]);
		return 1;
	}
	yyrestart(f);
	// yydebug = 1;
	
	yyparse();

//	if (!error_occurred)
		// print_parse_tree(root, 0);
	delete_parse_tree(root);

	cout << "AST root: " << ast_root.get() << endl;	

	DecCheckVisitor dec_checker;
	ast_root->accept(dec_checker);

	TypeCheckVisitor type_checker;
	ast_root->accept(type_checker);

	InterCodeGenVisitor inter_code_gen;
	ast_root->accept(inter_code_gen);

	ofstream fout(argv[2]);
	for (const auto &p_code : ast_root->code)
	{
		fout << *p_code << endl;
	}

	return 0;
}
