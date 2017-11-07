#pragma once

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

struct syntax_node {
	enum GrammarSymbol node_type;
	// int is_empty;
	union {
		int int_value;
		float float_value;
		char *string_value;
	}value;
	int loc;	// line number 
	struct syntax_node *prev, *next, *child;
};

#define YYSTYPE struct syntax_node *

// const struct syntax_node EPSILON;

const char *get_syntax_node_name(GrammarSymbol node_type);
void print_syntax_node(struct syntax_node * node);
void delete_syntax_tree(struct syntax_node *root);
void print_syntax_tree(struct syntax_node *root, int indent_level);

struct syntax_node *root;

struct syntax_node *create_terminal_node(GrammarSymbol node_type);
struct syntax_node *create_nonterminal_node(GrammarSymbol node_type, int argc, ...);


