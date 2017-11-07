#include "parse_tree.h"
#include <stdarg.h>
#include <assert.h>
#include <stdlib.h>
#include <stdio.h>


/* be careful of the order here */
const char *syntax_node_name_table[] = {
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

const char *get_syntax_node_name(GrammarSymbol node_type) {
    assert(node_type >= eFIRST_TERMINAL && node_type <= eLAST_NONTERMINAL);
    return syntax_node_name_table[node_type];
}

void print_syntax_node(struct syntax_node * node) {
    GrammarSymbol node_type = node->node_type;
    printf("%s", get_syntax_node_name(node_type));
    if(node_type == eID || node_type == eTYPE)
        printf(": %s", node->value.string_value);
    else if(node_type == eINT)
        printf(": %d", node->value.int_value);
    else if(node_type == eFLOAT)
        printf(": %f", node->value.float_value);
}

void delete_syntax_node(struct syntax_node * node) {
    int node_type = node->node_type;
    if(node_type == eID || node_type == eTYPE 
        || node_type == eRELOP)
        free(node->value.string_value);
    free(node);
}

int is_nonterminal(GrammarSymbol s) {
	return s >= eFIRST_NONTERMINAL && s <= eLAST_NONTERMINAL;
}

void print_syntax_tree(struct syntax_node *root, int indent_level) {
    /* we do not print invalid node and empty production rule node */
    if(!root || (is_nonterminal(root->node_type) && root->child == NULL))
        return;
    /* each indentation level == 2 space */
    for(int i = 0; i < indent_level; i++)
        printf("  ");
    print_syntax_node(root);
    printf("\n");
    struct syntax_node *p = root->child;
    while(p) {
        print_syntax_tree(p, indent_level + 1);
        p = p->next;
    }
}

void delete_syntax_tree(struct syntax_node *root) {
    struct syntax_node *p = root->child;
    delete_syntax_node(root);
    while(p) {
        delete_syntax_node(p);
        p = p->next;
    }
}

struct syntax_node *create_terminal_node(GrammarSymbol node_type) {
	struct syntax_node *ret = (struct syntax_node *)malloc(sizeof(struct syntax_node));
	ret->node_type = node_type;
	// ret->is_empty = 0;
	ret->prev = ret->next = ret->child = NULL;
	return ret;
}

struct syntax_node *create_nonterminal_node(GrammarSymbol node_type, int argc, ...) {
    struct syntax_node *head = NULL, *curr = NULL, *next = NULL;
    va_list args;

    va_start(args, argc);          
    for(int i = 0; i < argc; i++ ) {
        next = va_arg(args, struct syntax_node *); 
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

    struct syntax_node *ret = (struct syntax_node *)malloc(sizeof(struct syntax_node));
    ret->node_type = node_type;
    ret->prev = ret->next = NULL;
    ret->child = head;
    // ret->is_empty = 1;
    if(argc > 0) {
        // ret->is_empty = 0;
        // ret->line_no = head->line_no;
    }
    return ret;
}
