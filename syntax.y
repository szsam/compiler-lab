%{
#include <stdio.h>
#include <memory>
#include <string.h>
#include <iostream>
#include <vector>

#include "parse_tree.h"
#include "Type.h"
#include "Env.h"


/* defined in lex.yy.c */
int yylex();
extern int yylineno;

/* defined later in this file */
void yyerror(const char *);

/* set if an error (including lexical err and syntax err) has occured during parse */
int error_occurred = 0;

std::shared_ptr<Type> saved_specifier;
std::shared_ptr<Env> cur_env;
std::shared_ptr<Function> cur_func;
/* save types of arguments when parsing function call */
std::vector<std::shared_ptr<Type>> args;

std::shared_ptr<Type> check_arith_op(std::shared_ptr<Type> lhs, std::shared_ptr<Type> rhs);

void semantic_error(int type, int lineno, std::string msg);

void check_function_call(ParseTreeNode *pnode);

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
Program 	: { cur_env = std::make_shared<Env>(cur_env); }
		      ExtDefList { $$ = create_nonterminal_node(eProgram, 1, $2); root = $$; }
			;
ExtDefList	: ExtDef ExtDefList { $$ = create_nonterminal_node(eExtDefList, 2, $1, $2); }
			| { $$ = create_nonterminal_node(eExtDefList, 0); }
			;
ExtDef		: Specifier ExtDecList SEMI { $$ = create_nonterminal_node(eExtDef, 3, $1, $2, $3); }
			| Specifier SEMI { $$ = create_nonterminal_node(eExtDef, 2, $1, $2); }
			| Specifier FunDec CompSt { $$ = create_nonterminal_node(eExtDef, 3, $1, $2, $3); }
			;
ExtDecList	: VarDec { $$ = create_nonterminal_node(eExtDefList, 1, $1);
		               cur_env->put($1->id, $1->type);
					 }
			| VarDec COMMA ExtDecList { $$ = create_nonterminal_node(eExtDefList, 3, $1, $2, $3); }
			;
			
/* Specifiers */
Specifier	: TYPE { $$ = create_nonterminal_node(eSpecifier, 1, $1); 
				     Basic::BasicType t = !strcmp($1->value.string_value, "int") ? Basic::tINT : Basic::tFLOAT;
				     saved_specifier = std::make_shared<Basic>(t);
				   }
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
VarDec	: ID { $$ = create_nonterminal_node(eVarDec, 1, $1);
		       $$->id = $1->value.string_value;
			   $$->type = saved_specifier;
			 }
		| VarDec LB INT RB { $$ = create_nonterminal_node(eVarDec, 4, $1, $2, $3, $4); }
		;
FunDec	: ID LP { const char *id = $1->value.string_value;
				  cur_func = std::make_shared<Function>(saved_specifier);

		  		  if(!cur_env->put(id, cur_func))
				  {
				     std::cerr << "Error type 4 at Line " << yylineno 
						<< ": Redefined function '" << id << "'.\n";
				  }
				}
		  VarList RP { $$ = create_nonterminal_node(eFunDec, 4, $1, $2, $4, $5); 
					 }
		| ID LP RP { $$ = create_nonterminal_node(eFunDec, 3, $1, $2, $3); 
					 const char *id = $1->value.string_value;
					 cur_func = std::make_shared<Function>(saved_specifier);
					 if(!cur_env->put(id, cur_func))
					 {
						std::cerr << "Error type 4 at Line " << yylineno 
							<< ": Redefined function '" << id << "'.\n";
					 }
				   }
		;
VarList	: ParamDec COMMA VarList { $$ = create_nonterminal_node(eVarList, 3, $1, $2, $3); 
								   if ($1->type) cur_func->params.push_back($1->type);
								 }
		| ParamDec { $$ = create_nonterminal_node(eVarList, 1, $1); 
					 if ($1->type) cur_func->params.push_back($1->type);
				   }
		;
ParamDec	: Specifier VarDec { $$ = create_nonterminal_node(eParamDec, 2, $1, $2); 
								 if(!cur_env->put($2->id, $2->type))
				  				 {
				  				     std::cerr << "Error type 3 at Line " << yylineno 
				  				  	<< ": Redefined parameter '" << $2->id << "'.\n";
				  				 }
								 else
								 {
									 $$->id = $2->id;
									 $$->type = $2->type;
								 }
							   }
			;
/* Statements */
CompSt	: LC DefList StmtList RC { $$ = create_nonterminal_node(eCompSt, 4, $1, $2, $3, $4); }
	    | error RC { $$ = create_nonterminal_node(eCompSt, 1, $2); }
		;
StmtList: Stmt StmtList { $$ = create_nonterminal_node(eStmtList, 2, $1, $2); }
		| { $$ = create_nonterminal_node(eStmtList, 0); }
		;
Stmt	: Exp SEMI { $$ = create_nonterminal_node(eStmt, 2, $1, $2); }
		| CompSt { $$ = create_nonterminal_node(eStmt, 1, $1); }
		| RETURN Exp SEMI { $$ = create_nonterminal_node(eStmt, 3, $1, $2, $3); 
							if ($2->type && cur_func->ret_type && 
									*$2->type != *cur_func->ret_type)
							{
								semantic_error(8, yylineno, "Type mismatched for return");
							}
						  }
		| IF LP Exp RP Stmt %prec LOWER_THAN_ELSE { $$ = create_nonterminal_node(eStmt, 
													5, $1, $2, $3, $4, $5); }
		| IF LP Exp RP Stmt ELSE Stmt { $$ = create_nonterminal_node(eStmt, 7, $1, $2, $3, $4, $5, $6, $7); }
		| WHILE LP Exp RP Stmt { $$ = create_nonterminal_node(eStmt, 5, $1, $2, $3, $4, $5); }
		| error SEMI { $$ = create_nonterminal_node(eStmt, 1, $2); }
		;
		
/* Local Definitions */
DefList	: Def DefList { $$ = create_nonterminal_node(eDefList, 2, $1, $2); }
		| { $$ = create_nonterminal_node(eDefList, 0); }
		;
Def		: Specifier DecList SEMI { $$ = create_nonterminal_node(eDef, 3, $1, $2, $3); }
		| error SEMI { $$ = create_nonterminal_node(eDef, 1, $2); }
		;
DecList	: Dec { $$ = create_nonterminal_node(eDecList, 1, $1); }
		| Dec COMMA DecList { $$ = create_nonterminal_node(eDecList, 3, $1, $2, $3); }
		;
Dec		: VarDec { $$ = create_nonterminal_node(eDec, 1, $1); 
				   if(!cur_env->put($1->id, $1->type))
				   {
				       std::cerr << "Error type 3 at Line " << yylineno 
						<< ": Redefined variable '" << $1->id << "'.\n";
				   }
				 }
		| VarDec ASSIGNOP Exp { $$ = create_nonterminal_node(eDec, 3, $1, $2, $3); 
							    if(!cur_env->put($1->id, $1->type))
							    {
								   std::cerr << "Error type 3 at Line " << yylineno 
									<< ": Redefined variable '" << $1->id << "'.\n";
							    }
							  }
		;
		
/* Expressions */
Exp	: Exp ASSIGNOP Exp	{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); 
						  if ($1->type && $3->type && *$1->type != *$3->type)
						  {
							std::cerr << "Error type 5 at Line " << yylineno 
								<< ": Type mismatched for assignment.\n";
						  }
						  if (!$1->has_lvalue)
						  {
							  std:: cerr << "Error type 6 at Line " << yylineno 
								<< ": Lvalue required as left operand of assignment\n";
						  }
						}
	| Exp AND Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp OR Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp RELOP Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp PLUS Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); 
						  $$->type = check_arith_op($1->type, $3->type);
						  if (!$$->type)
						  {
							semantic_error(7, yylineno, "Type mismatched for operands");
						  }
						}
	| Exp MINUS Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp STAR Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| Exp DIV Exp		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| LP Exp RP			{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); }
	| MINUS Exp %prec NEG { $$ = create_nonterminal_node(eExp, 2, $1, $2); }
	| NOT Exp		{ $$ = create_nonterminal_node(eExp, 2, $1, $2); }
	| ID LP Args RP { $$ = create_nonterminal_node(eExp, 4, $1, $2, $3, $4); 
					  check_function_call($1);
				    }
	| ID LP RP		{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3);
					  check_function_call($1);
				    }
	| Exp LB Exp RB { $$ = create_nonterminal_node(eExp, 4, $1, $2, $3, $4); 
					  $$->has_lvalue = true;
					}
	| Exp DOT ID	{ $$ = create_nonterminal_node(eExp, 3, $1, $2, $3); 
					  $$->has_lvalue = true;
					}
	| ID			{ $$ = create_nonterminal_node(eExp, 1, $1); 
					  const char *id = $1->value.string_value;
					  if (auto idtype = cur_env->get(id))
					  {
						  $$->type = idtype;
						  $$->has_lvalue = true;
					  }
					  else
					  {
						fprintf(stderr, "Error type 1 at Line %d: Undefined variable '%s'.\n", yylineno, id);
					  }
					}
	| INT			{ $$ = create_nonterminal_node(eExp, 1, $1); 
					  $$->type = std::make_shared<Basic>(Basic::tINT);
					}
	| FLOAT			{ $$ = create_nonterminal_node(eExp, 1, $1);
					  $$->type = std::make_shared<Basic>(Basic::tFLOAT);
					}
	;
Args: Exp COMMA Args { $$ = create_nonterminal_node(eArgs, 3, $1, $2, $3); 
					   args.push_back($1->type);
					 }
	| Exp { $$ = create_nonterminal_node(eArgs, 1, $1); 
			args.clear();
			args.push_back($1->type);	
		  }
	;

%%
void yyerror(const char *msg)
{
	error_occurred = 1;
	fprintf(stderr, "Error type B at Line %d: %s\n", yylineno, msg);
}

/* Types of operands of arithmetic operator should be identical and int/float */
std::shared_ptr<Type> check_arith_op(std::shared_ptr<Type> lhs, std::shared_ptr<Type> rhs)
{
	if (lhs && rhs && (typeid(*lhs) == typeid(*rhs)) && (typeid(*lhs) == typeid(Basic)))
	{
		auto l = dynamic_cast<Basic &>(*lhs);
		auto r = dynamic_cast<Basic &>(*rhs);
		return (l.basic_type == r.basic_type) ? lhs : nullptr;
	}
	else return nullptr;	
}

void semantic_error(int type, int lineno, std::string msg)
{
	std::cerr << "Error type " << type << " at Line " << lineno << ": " << msg << ".\n";
}

void check_function_call(ParseTreeNode *pnode)
{
	const char *id = pnode->value.string_value;
  	auto idtype = cur_env->get(id);
  	if (!idtype)
  	{
  	  std::cerr << "Error type 2 at Line " << yylineno
  	  	<< ": Undefined function '" << id << "'.\n";
  	}
  	else if (typeid(*idtype) != typeid(Function))
  	{
  	  semantic_error(11, yylineno, std::string("'") + id + "' is not a function");
  	}
  	else
  	{
  	    auto fun = dynamic_cast<Function &>(*idtype);
  	    if (!fun.match_args(args))
  	    {
  	  	  semantic_error(9, yylineno, "Argument(s) of function call to '" 
  	  		+ std::string(id) + "' mismatch with its parameter(s)");
  	    }

  	}

}
