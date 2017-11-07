%{
#include <stdio.h>
#include "parse_tree.h"


int yylex();
void yyerror();
extern int yylineno;


%}
/* let Bison define yylloc */
%locations

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

%%
/* productions */
/* High-level Definitions */
Program 	: ExtDefList { $$ = create_nonterminal_node(eProgram, 1, $1); root = $$; }
			;
ExtDefList	: ExtDef ExtDefList { $$ = create_nonterminal_node(eExtDefList, 2, $1, $2); }
			| { $$ = create_nonterminal_node(eExtDefList, 0); }
			;
ExtDef		: Specifier ExtDecList SEMI { $$ = create_nonterminal_node(eExtDef, 3, $1, $2, $3); }
			| Specifier SEMI { $$ = create_nonterminal_node(eExtDef, 2, $1, $2); }
			| Specifier FunDec CompSt { $$ = create_nonterminal_node(eExtDef, 3, $1, $2, $3); }
			;
ExtDecList	: VarDec { $$ = create_nonterminal_node(eExtDefList, 1, $1); }
			| VarDec COMMA ExtDecList { $$ = create_nonterminal_node(eExtDefList, 3, $1, $2, $3); }
			;
			
/* Specifiers */
Specifier	: TYPE { $$ = create_nonterminal_node(eSpecifier, 1, $1); }
			| StructSpecifier { $$ = create_nonterminal_node(eSpecifier, 1, $1); }
			;
StructSpecifier : STRUCT OptTag LC DefList RC { $$ = create_nonterminal_node(eStructSpecifier, 
													 5, $1, $2, $3, $4, $5); }
				| STRUCT Tag { $$ = create_nonterminal_node(eStructSpecifier, 2, $1, $2); }
				;
OptTag	: ID { $$ = create_nonterminal_node(eOptTag, 1, $1); }
		| { $$ = create_nonterminal_node(eOptTag, 0); }
		;
Tag	: ID { $$ = create_nonterminal_node(eTag, 1, $1); }
	;
	
/* Declarators */
VarDec	: ID { $$ = create_nonterminal_node(eVarDec, 1, $1); }
		| VarDec LB INT RB { $$ = create_nonterminal_node(eVarDec, 4, $1, $2, $3, $4); }
		;
FunDec	: ID LP VarList RP { $$ = create_nonterminal_node(eFunDec, 4, $1, $2, $3, $4); }
		| ID LP RP { $$ = create_nonterminal_node(eFunDec, 3, $1, $2, $3); }
		;
VarList	: ParamDec COMMA VarList { $$ = create_nonterminal_node(eVarList, 3, $1, $2, $3); }
		| ParamDec { $$ = create_nonterminal_node(eVarList, 1, $1); }
		;
ParamDec	: Specifier VarDec { $$ = create_nonterminal_node(eParamDec, 2, $1, $2); }
			;

/* Statements */
CompSt	: LC DefList StmtList RC { $$ = create_nonterminal_node(eCompSt, 4, $1, $2, $3, $4); }
		;
StmtList: Stmt StmtList { $$ = create_nonterminal_node(eStmtList, 2, $1, $2); }
		| { $$ = create_nonterminal_node(eStmtList, 0); }
		;
Stmt	: Exp SEMI { $$ = create_nonterminal_node(eStmt, 2, $1, $2); }
		| CompSt { $$ = create_nonterminal_node(eStmt, 1, $1); }
		| RETURN Exp SEMI { $$ = create_nonterminal_node(eStmt, 3, $1, $2, $3); }
		| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = create_nonterminal_node(eStmt, 
													5, $1, $2, $3, $4, $5); }
		| IF LP Exp RP Stmt ELSE Stmt { $$ = create_nonterminal_node(eStmt, 7, $1, $2, $3, $4, $5, $6, $7); }
		| WHILE LP Exp RP Stmt { $$ = create_nonterminal_node(eStmt, 5, $1, $2, $3, $4, $5); }
		| error SEMI { }
		;
		
/* Local Definitions */
DefList	: Def DefList { $$ = create_nonterminal_node(eDefList, 2, $1, $2); }
		| { $$ = create_nonterminal_node(eDefList, 0); }
		;
Def		: Specifier DecList SEMI { $$ = create_nonterminal_node(eDef, 3, $1, $2, $3); }
		;
DecList	: Dec { $$ = create_nonterminal_node(eDecList, 1, $1); }
		| Dec COMMA DecList { $$ = create_nonterminal_node(eDecList, 3, $1, $2, $3); }
		;
Dec		: VarDec { $$ = create_nonterminal_node(eDec, 1, $1); }
		| VarDec ASSIGNOP Exp { $$ = create_nonterminal_node(eDec, 3, $1, $2, $3); }
		;
		
/* Expressions */
Exp	: Exp ASSIGNOP Exp	{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp AND Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp OR Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp RELOP Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp PLUS Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp MINUS Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp STAR Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp DIV Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| LP Exp RP			{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| MINUS Exp %prec NEG { $$ = create_nonterminal_node(eExp, 2, $1, $2); }
	| NOT Exp		{ $$ = create_nonterminal_node(eExp, 2, $1, $2); }
	| ID LP Args RP { $$ = create_nonterminal_node(eExp, 4, $1, $2, $3, $4); }
	| ID LP RP		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp LB Exp RB { $$ = create_nonterminal_node(eExp, 4, $1, $2, $3, $4); }
	| Exp DOT ID	{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| ID			{ $$ = create_nonterminal_node(eExp, 1, $1); }
	| INT			{ $$ = create_nonterminal_node(eExp, 1, $1); }
	| FLOAT			{ $$ = create_nonterminal_node(eExp, 1, $1); }
	;
Args: Exp COMMA Args { $$ = create_nonterminal_node(eArgs, 3, $1, $2, $3); }
	| Exp { $$ = create_nonterminal_node(eArgs, 1, $1); }
	;

%%
void yyerror(char *msg)
{
	fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}


