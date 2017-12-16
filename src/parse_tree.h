#pragma once

#include "Type.h"
#include "expression.h"

typedef enum GrammarSymbol {
	eFIRST_TERMINAL,
	eINT = eFIRST_TERMINAL, eFLOAT, eID, eTYPE, eRELOP,
	eSEMI, eCOMMA, eASSIGNOP,
	ePLUS, eMINUS, eSTAR, eDIV,
	eAND, eOR, eDOT, eNOT,
	eLP, eRP, eLB, eRB, eLC, eRC,
	eSTRUCT, eRETURN, eIF, eELSE, eWHILE,
	eLAST_TERMINAL = eWHILE,
	eFIRST_NONTERMINAL, 
    eProgram = eFIRST_NONTERMINAL, eExtDefList, eExtDef, eExtDecList, 
	eSpecifier, eStructSpecifier, eTag, eOptTag, 
    eFunDec, eVarDec, eVarList, eParamDec, 
    eCompSt, eStmtList, eStmt, 
	eDefList, eDef, eDec, eDecList,
	eExp, eArgs,
	eLAST_NONTERMINAL = eArgs
}GrammarSymbol;

typedef struct ParseTreeNode {
	enum GrammarSymbol node_type;
	// union {
		int int_value;
		float float_value;
		std::string string_value;
		
		SP<ExtDef> ext_def;
		PVP<ExtDef> ext_def_list;
		SP<VarDec> var_dec;
		SP<VEC<VarDec>> var_dec_list;
		Param param_dec;
		SP<VEC<Param>> var_list;
		SP<FunDec> fun_dec;

		SP<Specifier> specifier;
		SP<StructSpecifier> struct_specifier;

		SP<Statement> stmt;
		SP<CompSt> comp_st;
		PVP<Statement> stmt_list;

		SP<Dec> dec;
		SP<VEC<Dec>> dec_list;
		SP<Def> def;
		SP<VEC<Def>> def_list;

		SP<Expression> exp;
		SP<FunCall> fun_call;
		PVP<Expression> args;
	// };
	int loc;	// line number 

	// used in semantic analysis
	// std::string id;
	std::shared_ptr<Type> type;
	bool has_lvalue;

	struct ParseTreeNode *prev, *next, *child;
}ParseTreeNode;

#define YYSTYPE ParseTreeNode *


const char *get_parse_tree_node_name(GrammarSymbol node_type);
void print_parse_tree_node(ParseTreeNode *);
void delete_parse_tree(ParseTreeNode *);
void delete_parse_tree_node(ParseTreeNode *);
void print_parse_tree(ParseTreeNode *root, int indent_level);

extern ParseTreeNode *root;

ParseTreeNode *create_terminal_node(GrammarSymbol node_type);
ParseTreeNode *create_nonterminal_node(GrammarSymbol node_type, int argc, ...);


