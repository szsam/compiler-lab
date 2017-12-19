#include "parse_tree.h"

#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>

extern int yylineno;	// defined in lex.yy.c

ParseTreeNode *root;

/* be careful of the order here */
const char *parse_tree_node_name_table[] = {
	"INT", "FLOAT", "ID", "TYPE", "RELOP",
	"SEMI", "COMMA", "ASSIGNOP",
	"PLUS", "MINUS", "STAR", "DIV",
	"AND", "OR", "DOT", "NOT",
	"LP", "RP", "LB", "RB", "LC", "RC",
	"STRUCT", "RETURN", "IF", "ELSE", "WHILE",
    "Program", "ExtDefList", "ExtDef", "ExtDecList", 
	"Specifier", "StructSpecifier", "Tag", "OptTag", 
    "FunDec", "VarDec", "VarList", "ParamDec", 
    "CompSt", "StmtList", "Stmt", 
	"DefList", "Def", "Dec", "DecList",
	"Exp", "Args"
};

const char *get_parse_tree_node_name(GrammarSymbol node_type) {
    assert(node_type >= eFIRST_TERMINAL && node_type <= eLAST_NONTERMINAL);
    return parse_tree_node_name_table[node_type];
}

int is_nonterminal(GrammarSymbol s) {
	return s >= eFIRST_NONTERMINAL && s <= eLAST_NONTERMINAL;
}

void print_parse_tree_node(ParseTreeNode * node) {
    GrammarSymbol node_type = node->node_type;
    printf("%s", get_parse_tree_node_name(node_type));
	if (is_nonterminal(node_type)) {
		// print line number
		printf(" (%d)", node->loc);
	}
    else if(node_type == eID || node_type == eTYPE)
        printf(": %s", node->string_value.c_str());
    else if(node_type == eINT)
        printf(": %d", node->int_value);
    else if(node_type == eFLOAT)
        printf(": %f", node->float_value);
    printf("\n");
}

void print_parse_tree(ParseTreeNode *root, int indent_level) {
    /* we do not print invalid node and empty production rule node */
    if(!root || (is_nonterminal(root->node_type) && root->child == NULL))
        return;
    /* each indentation level requires 2 spaces */
    for(int i = 0; i < indent_level; i++)
        printf("  ");
    print_parse_tree_node(root);
	print_parse_tree(root->child, indent_level + 1);
	print_parse_tree(root->next, indent_level);

   //  ParseTreeNode *p = root->child;
   //  while(p) {
   //      print_parse_tree(p, indent_level + 1);
   //      p = p->next;
   //  }
}

void delete_parse_tree_node(ParseTreeNode * node) {
//    int node_type = node->node_type;
//    if(node_type == eID || node_type == eTYPE 
//        || node_type == eRELOP)
//        free(node->string_value);
    free(node);
}

void delete_parse_tree(ParseTreeNode *root) {
	if (!root) return;
//    ParseTreeNode *p = root->child;
//    while(p) {
//        delete_parse_tree_node(p);
//        p = p->next;
//    }
	delete_parse_tree(root->child);
	delete_parse_tree(root->next);
    delete_parse_tree_node(root);
}

ParseTreeNode *create_terminal_node(GrammarSymbol node_type) {
	ParseTreeNode *ret = (ParseTreeNode *)malloc(sizeof(ParseTreeNode));
	ret->node_type = node_type;
	ret->prev = ret->next = ret->child = NULL;
	ret->loc = yylineno;
	ret->has_lvalue = false;
	return ret;
}

ParseTreeNode *create_nonterminal_node(GrammarSymbol node_type, int argc, ...) {
    ParseTreeNode *head = NULL, *curr = NULL, *next = NULL;
    va_list args;

    va_start(args, argc);          
    for(int i = 0; i < argc; i++ ) {
        next = va_arg(args, ParseTreeNode *); 
        /* first one */
        if(!head) {
            head = curr = next;
            curr->prev = NULL;
        }
        else {
            curr->next = next;
            next->prev = curr;
            curr = next;
        }
        curr->next = NULL;
    }
    va_end(args);

    ParseTreeNode *ret = (ParseTreeNode *)malloc(sizeof(ParseTreeNode));
    ret->node_type = node_type;
    ret->prev = ret->next = NULL;
    ret->child = head;
	ret->has_lvalue = false;

	if (ret->child) {
		ret->loc = head->loc;
	}
	else {
		ret->loc = -1;
	}
    return ret;
}
