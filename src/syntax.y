%{
#include <stdio.h>
// #include <memory>
// #include <string.h>
#include <iostream>
// #include <vector>
// #include <map>

#include "parse_tree.h"
// #include "Type.h"
// #include "Env.h"

// #include "expression.h"

/* defined in lex.yy.c */
int yylex();
extern int yylineno;

/* defined later in this file */
void yyerror(const char *);

/* set if an error (including lexical err and syntax err) has occured during parse */
int error_occurred = 0;

SP<Program> ast_root;


void semantic_error(int type, int lineno, std::string msg);


%}

/* let Bison define yylloc */
// %locations

/* Error messages report the unexpected token, and possibly the expected ones */
%define parse.error verbose
/* Enable LAC (lookahead correction) to improve syntax error handling */
%define parse.lac full

/* declared types */
//%union {
///    int int_value;
////    float float_value;
////    char string_value[32];
//    struct syntax_node *node_value;
//}

/* declared tokens */
%token INT 
%token FLOAT 
%token ID TYPE RELOP
%token SEMI COMMA ASSIGNOP
%token PLUS MINUS STAR DIV
%token AND OR DOT NOT
%token LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE

/* attribute type of non-terminals */
// %type <node_value> Program ExtDefList ExtDef ExtDecList 
// Specifier StructSpecifier Tag OptTag 
// FunDec VarDec VarList ParamDec 
// CompSt StmtList Stmt 
// DefList Def Dec DecList
// Exp Args

/* precedence and associativity */
%right ASSIGNOP
%left OR
%left AND
%left RELOP
%left PLUS MINUS
%left STAR DIV
%right NOT NEG
%left DOT LB RB LP RP

/* deal with 'dangling else' */
%nonassoc LOWER_THAN_ELSE
%nonassoc ELSE

/* free discarded symbols during error recovery */
%destructor { 
	/* Bison will discard the start symbol when the parser succeeds,
     * but of course we don't want to call delete_parse_tree() on it.
	 */
	if ($$->node_type != eProgram)
	{
		// printf("destructor: %s %d\n", get_syntax_node_name($$->node_type), $$->loc); 
		delete_parse_tree($$); 
	}
} <>

%%
/* productions */
/* High-level Definitions */
Program 	: ExtDefList { $$ = create_nonterminal_node(eProgram, 1, $1); root = $$; 
						   ast_root = std::make_shared<Program>(*$1->ext_def_list);
						 }
			;
ExtDefList	: ExtDef ExtDefList { $$ = create_nonterminal_node(eExtDefList, 2, $1, $2); 
								  $$->ext_def_list = $2->ext_def_list;
								  $$->ext_def_list->push_back($1->ext_def);
								}
			| { $$ = create_nonterminal_node(eExtDefList, 0);
				$$->ext_def_list = std::make_shared<VEC<SP<ExtDef>>>();
			  }
			;
ExtDef		: Specifier ExtDecList SEMI { $$ = create_nonterminal_node(eExtDef, 3, $1, $2, $3); 
										  $$->ext_def = std::make_shared<GlobalVar>(
													$1->specifier, *$2->var_dec_list);
										}
			| Specifier SEMI { $$ = create_nonterminal_node(eExtDef, 2, $1, $2); 
							   $$->ext_def = $1->specifier;
							 }
			| Specifier FunDec CompSt { $$ = create_nonterminal_node(eExtDef, 3, $1, $2, $3);
										$2->fun_dec->ret_type = $1->specifier;
										$2->fun_dec->body = $3->comp_st;
										$$->ext_def = $2->fun_dec;
									  }
			| Specifier FunDec error { $$ = create_nonterminal_node(eExtDef, 2, $1, $2); }
			;
ExtDecList	: VarDec { $$ = create_nonterminal_node(eExtDefList, 1, $1);
					   $$->var_dec_list = std::make_shared<VEC<VarDec>>();
					 }
			| VarDec COMMA ExtDecList { $$ = create_nonterminal_node(eExtDefList, 3, $1, $2, $3); 
										$$->var_dec_list = $3->var_dec_list;
										$$->var_dec_list->push_back(*$1->var_dec);
									  }
			;
			
/* Specifiers */
Specifier	: TYPE { $$ = create_nonterminal_node(eSpecifier, 1, $1); 
					 $$->specifier = std::make_shared<BasicType>($1->string_value);
				   }
			| StructSpecifier { $$ = create_nonterminal_node(eSpecifier, 1, $1);
								$$->specifier = $1->struct_specifier;
							  }
			;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = create_nonterminal_node(eStructSpecifier, 
													 5, $1, $2, $3, $4, $5);
												$$->struct_specifier = std::make_shared<StructSpecifier>(
													$2->string_value, *$4->def_list);
											  }
				| STRUCT Tag { $$ = create_nonterminal_node(eStructSpecifier, 2, $1, $2);
							   $$->struct_specifier = std::make_shared<StructSpecifier>(
									$2->string_value);
							 }
				;
OptTag	: ID { $$ = create_nonterminal_node(eOptTag, 1, $1);
			   $$->string_value = $1->string_value;
			 }
		| { $$ = create_nonterminal_node(eOptTag, 0); 
			$$->string_value = std::string(); 
		  }
		;
Tag	: ID { $$ = create_nonterminal_node(eTag, 1, $1);
		   $$->string_value = $1->string_value;
	   	 }
	;
	
/* Declarators */
VarDec	: ID { $$ = create_nonterminal_node(eVarDec, 1, $1);
			   $$->var_dec = std::make_shared<VarDec>($1->string_value);
			 }
		| VarDec LB INT RB { $$ = create_nonterminal_node(eVarDec, 4, $1, $2, $3, $4); 
							 $$->var_dec = $1->var_dec;
							 $$->var_dec->indices.push_back($3->int_value);
						   }
		;
FunDec	: ID LP VarList RP { $$ = create_nonterminal_node(eFunDec, 4, $1, $2, $3, $4); 
							 $$->fun_dec = std::make_shared<FunDec>($1->string_value, *$3->var_list);
					       }
		| ID LP RP { $$ = create_nonterminal_node(eFunDec, 3, $1, $2, $3); 
					 $$->fun_dec = std::make_shared<FunDec>($1->string_value);	
				   }
		;
VarList	: ParamDec COMMA VarList { $$ = create_nonterminal_node(eVarList, 3, $1, $2, $3); 
								   $$->var_list = $3->var_list;
								   $$->var_list->push_back($1->param_dec);
								 }
		| ParamDec { $$ = create_nonterminal_node(eVarList, 1, $1); 
					 $$->var_list = std::make_shared<VEC<Param>>();
					 $$->var_list->push_back($1->param_dec);
				   }
		;
ParamDec	: Specifier VarDec { $$ = create_nonterminal_node(eParamDec, 2, $1, $2); 
								 $$->param_dec = { $1->specifier, $2->var_dec};
							   }
			;
/* Statements */
CompSt	: LC DefList StmtList RC { $$ = create_nonterminal_node(eCompSt, 4, $1, $2, $3, $4);
								   $$->comp_st = std::make_shared<CompSt>(*$2->def_list, *$3->stmt_list);
								 }
	    | error RC { $$ = create_nonterminal_node(eCompSt, 1, $2); }
		;
StmtList: Stmt StmtList { $$ = create_nonterminal_node(eStmtList, 2, $1, $2);
						  $$->stmt_list = $2->stmt_list;
						  $$->stmt_list->push_back($1->stmt);
						}
		| { $$ = create_nonterminal_node(eStmtList, 0); 
			$$->stmt_list = std::make_shared<VEC<SP<Statement>>>();
		  }
		;
Stmt	: Exp SEMI { $$ = create_nonterminal_node(eStmt, 2, $1, $2); $$->stmt = $1->exp; }
		| CompSt { $$ = create_nonterminal_node(eStmt, 1, $1); $$->stmt = $1->comp_st; }
		| RETURN Exp SEMI { $$ = create_nonterminal_node(eStmt, 3, $1, $2, $3); 
							$$->stmt = std::make_shared<Return>($2->exp);
						  }
		| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = create_nonterminal_node(eStmt, 
													 5, $1, $2, $3, $4, $5);
													$$->stmt = std::make_shared<Branch>(
														$3->exp, $5->stmt);
												  }
		| IF LP Exp RP Stmt ELSE Stmt { $$ = create_nonterminal_node(eStmt, 7, $1, $2, $3, $4, $5, $6, $7);
										$$->stmt = std::make_shared<Branch>(
												$3->exp, $5->stmt, $7->stmt);
									  }
		| WHILE LP Exp RP Stmt { $$ = create_nonterminal_node(eStmt, 5, $1, $2, $3, $4, $5);
								 $$->stmt = std::make_shared<While>($3->exp, $5->stmt);
							   }
		| error SEMI { $$ = create_nonterminal_node(eStmt, 1, $2); }
		;
		
/* Local Definitions */
DefList	: Def DefList { $$ = create_nonterminal_node(eDefList, 2, $1, $2); 
						$$->def_list = $2->def_list;
						if (!$1->def) break;
						$$->def_list->push_back(*$1->def);
					  }
		| { $$ = create_nonterminal_node(eDefList, 0); 
			$$->def_list = std::make_shared<VEC<Def>>();
		  }
		;
Def		: Specifier DecList SEMI { $$ = create_nonterminal_node(eDef, 3, $1, $2, $3); 
								   $$->def = std::make_shared<Def>($1->specifier, *$2->dec_list);
								 }
		| error SEMI { $$ = create_nonterminal_node(eDef, 1, $2); }
		;
DecList	: Dec { $$ = create_nonterminal_node(eDecList, 1, $1);
				$$->dec_list = std::make_shared<VEC<Dec>>(1, *$1->dec);
			  }
		| Dec COMMA DecList { $$ = create_nonterminal_node(eDecList, 3, $1, $2, $3); 
							  $$->dec_list = $3->dec_list;
							  $$->dec_list->push_back(*$1->dec);
							}
		;
Dec		: VarDec { $$ = create_nonterminal_node(eDec, 1, $1); 
				   $$->dec = std::make_shared<Dec>(*$1->var_dec);
				 }
		| VarDec ASSIGNOP Exp { $$ = create_nonterminal_node(eDec, 3, $1, $2, $3); 
								$$->dec = std::make_shared<Dec>(*$1->var_dec, $3->exp);
							  }
		;
		
/* Expressions */
Exp	: Exp ASSIGNOP Exp	{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); 
						  $$->exp = std::make_shared<Assign>($1->exp, $3->exp);
						}
	| Exp AND Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); 
						  $$->exp = std::make_shared<And>($1->exp, $3->exp);
						}
	| Exp OR Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3);
						  $$->exp = std::make_shared<Or>($1->exp, $3->exp);
						}
	| Exp RELOP Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3);
						  $$->exp = std::make_shared<Relop>($1->exp, $3->exp, $2->string_value);
						}
	| Exp PLUS Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); 
						  $$->exp = std::make_shared<Plus>($1->exp, $3->exp);
						}
	| Exp MINUS Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3);
						  $$->exp = std::make_shared<Minus>($1->exp, $3->exp);
						}
	| Exp STAR Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); 
						  $$->exp = std::make_shared<Multiply>($1->exp, $3->exp);
						}
	| Exp DIV Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); 
						  $$->exp = std::make_shared<Divide>($1->exp, $3->exp);
						}
	| LP Exp RP			{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3);
						  $$->exp = $2->exp;
						}
	| MINUS Exp %prec NEG { $$ = create_nonterminal_node(eExp, 2, $1, $2);
							$$->exp = std::make_shared<Negative>($2->exp);
						  }
	| NOT Exp		{ $$ = create_nonterminal_node(eExp, 2, $1, $2); 
					  $$->exp = std::make_shared<Not>($2->exp);
					}
	| ID LP Args RP { $$ = create_nonterminal_node(eExp, 4, $1, $2, $3, $4); 
					  $$->exp = std::make_shared<FunCall>($1->string_value, *$3->args);
				    }
	| ID LP RP		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3);
					  $$->exp = std::make_shared<FunCall>($1->string_value);
				    }
	| Exp LB Exp RB { $$ = create_nonterminal_node(eExp, 4, $1, $2, $3, $4); 
					  $$->exp = std::make_shared<Subscript>($1->exp, $3->exp);
					}
	| Exp DOT ID	{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); 
					  $$->exp = std::make_shared<MemberAccess>($1->exp, $3->exp);
					}
	| ID			{ $$ = create_nonterminal_node(eExp, 1, $1); 
					  $$->exp = std::make_shared<Identifier>($1->string_value);
					}
	| INT			{ $$ = create_nonterminal_node(eExp, 1, $1); 
					  $$->exp = std::make_shared<Integer>($1->int_value);
					}
	| FLOAT			{ $$ = create_nonterminal_node(eExp, 1, $1);
					  $$->exp = std::make_shared<Float>($1->float_value);
					}
	;
Args: Exp COMMA Args { $$ = create_nonterminal_node(eArgs, 3, $1, $2, $3); 
					   $$->args = $3->args;
					   $$->args->push_back($1->exp);
					 }
	| Exp { $$ = create_nonterminal_node(eArgs, 1, $1); 
			$$->args = std::make_shared<VEC<SP<Expression>>>(1, $1->exp);
		  }
	;

%%
void yyerror(const char *msg)
{
	error_occurred = 1;
	fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}

void semantic_error(int type, int lineno, std::string msg)
{
	std::cerr << "Error type " << type << " at Line " << lineno << ": " << msg << ".\n";
}
