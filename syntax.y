%{
#include <stdio.h>
int yylex();
void yyerror();
extern int yylineno;

%}

/* declared tokens */
%token INT FLOAT ID
%token SEMI COMMA ASSIGNOP RELOP
%token PLUS MINUS STAR DIV
%token AND OR DOT NOT
%token TYPE
%token LP RP LB RB LC RC
%token STRUCT RETURN IF ELSE WHILE

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
Program 	: ExtDefList
			;
ExtDefList	: ExtDef ExtDefList
			| 
			;
ExtDef		: Specifier ExtDecList SEMI
			| Specifier SEMI
			| Specifier FunDec CompSt
			;
ExtDecList	: VarDec
			| VarDec COMMA ExtDecList
			;
			
/* Specifiers */
Specifier	: TYPE
			| StructSpecifier
			;
StructSpecifier : STRUCT OptTag LC DefList RC
				| STRUCT Tag
				;
OptTag	: ID
		| 
		;
Tag	: ID
	;
	
/* Declarators */
VarDec	: ID
		| VarDec LB INT RB
		;
FunDec	: ID LP VarList RP
		| ID LP RP
		;
VarList	: ParamDec COMMA VarList
		| ParamDec
		;
ParamDec	: Specifier VarDec
			;

/* Statements */
CompSt	: LC DefList StmtList RC { printf("CompSt -> { DefList StmtList }, line %d\n", @$.first_line); }
		;
StmtList: Stmt StmtList
		| 
		;
Stmt	: Exp SEMI { printf("stmt -> exp semi, line %d\n", @$.first_line); }
		| CompSt
		| RETURN Exp SEMI
		| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE
		| IF LP Exp RP Stmt ELSE Stmt
		| WHILE LP Exp RP Stmt
		| error SEMI
		;
		
/* Local Definitions */
DefList	: Def DefList
		|
		;
Def		: Specifier DecList SEMI
		;
DecList	: Dec
		| Dec COMMA DecList
		;
Dec		: VarDec
		| VarDec ASSIGNOP Exp
		;
		
/* Expressions */
Exp	: Exp ASSIGNOP Exp
	| Exp AND Exp
	| Exp OR Exp
	| Exp RELOP Exp
	| Exp PLUS Exp
	| Exp MINUS Exp
	| Exp STAR Exp
	| Exp DIV Exp
	| LP Exp RP
	| MINUS Exp %prec NEG
	| NOT Exp
	| ID LP Args RP
	| ID LP RP
	| Exp LB Exp RB
	| Exp DOT ID
	| ID
	| INT
	| FLOAT
	;
Args: Exp COMMA Args
	| Exp
	;

%%
void yyerror(char *msg)
{
	fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}
