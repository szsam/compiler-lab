#pragma once

#include "Type.h"

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
	union {
		int int_value;
		float float_value;
		char *string_value;
	}value;
	int loc;	// line number 

	// used in semantic analysis
	std::string id;
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


