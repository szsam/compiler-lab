%{
/* parse_tree.h should precede syntax.tab.h  
 * because of our definition of macro YYSTYPE 
 */
#include "parse_tree.h"
#include "syntax.tab.h"

#include <stdlib.h>

extern int error_occurred;

// int yycolumn = 1;
// #define YY_USER_ACTION \
// 	yylloc.first_line = yylloc.last_line = yylineno; \
// 	yylloc.first_column = yycolumn; \
// 	yylloc.last_column = yycolumn + yyleng - 1; \
// 	yycolumn += yyleng;
%}
%option yylineno

digit [0-9]
digits [0-9]+
letter [_a-zA-Z]
delim [ \n\t]
ws {delim}+

decimal 0|[1-9][0-9]*
oct 0[0-7]+
hex 0[xX][0-9a-fA-F]+

decimal_float {digits}\.{digits}

significand [0-9]*\.?[0-9]+|[0-9]+\.
exponential_sym [eE]
optional_sign [+-]?
scientific_notation {significand}{exponential_sym}{optional_sign}{digits}

%%
\n { /* yycolumn = 1; */ }
[ \t]+ { /* no action and no return */}

{decimal} { 
	yylval = create_terminal_node(eINT);
	yylval->value.int_value = atoi(yytext);
	return INT; 
}

{oct} { 
	yylval = create_terminal_node(eINT);
	yylval->value.int_value = strtol(yytext, NULL, 8);
	return INT; 
}

{hex} { 
	yylval = create_terminal_node(eINT);
	yylval->value.int_value = strtol(yytext, NULL, 16);
	return INT; 
}

{decimal_float} { 
	yylval = create_terminal_node(eFLOAT);
	yylval->value.float_value = strtof(yytext, NULL);
	return FLOAT; 
}

{scientific_notation} { 
	yylval = create_terminal_node(eFLOAT);
	yylval->value.float_value = strtof(yytext, NULL);
	return FLOAT; 
}

struct	{ yylval = create_terminal_node(eSTRUCT); return STRUCT; }
return	{ yylval = create_terminal_node(eRETURN); return RETURN; }
if		{ yylval = create_terminal_node(eIF);	return IF;	 }
else    { yylval = create_terminal_node(eELSE); return ELSE;	 }
while   { yylval = create_terminal_node(eWHILE); return WHILE;	 }

int|float { 
	yylval = create_terminal_node(eTYPE);
	yylval->value.string_value = strdup(yytext);
	return TYPE; 
}

{letter}({letter}|{digit})*	{ 
	yylval = create_terminal_node(eID);
	yylval->value.string_value = strdup(yytext);
	return ID;
}

; { yylval = create_terminal_node(eSEMI); return SEMI; }
, { yylval = create_terminal_node(eCOMMA); return COMMA; }
= { yylval = create_terminal_node(eASSIGNOP); return ASSIGNOP; }

>|<|>=|<=|==|!=	{ 
	yylval = create_terminal_node(eRELOP);
	yylval->value.string_value = strdup(yytext);
	return RELOP;	
}

"+" { yylval = create_terminal_node(ePLUS); return PLUS; }
-   { yylval = create_terminal_node(eMINUS);return MINUS; }
"*" { yylval = create_terminal_node(eSTAR); return STAR; }
"/" { yylval = create_terminal_node(eDIV);  return DIV; }
&&  { yylval = create_terminal_node(eAND);  return AND; }
"||" { yylval = create_terminal_node(eOR);  return OR; }
"." { yylval = create_terminal_node(eDOT);  return DOT; }
!   { yylval = create_terminal_node(eNOT);  return NOT; }

\( { yylval = create_terminal_node(eLP); return LP; }
\) { yylval = create_terminal_node(eRP); return RP; }
\[ { yylval = create_terminal_node(eLB); return LB; }
\] { yylval = create_terminal_node(eRB); return RB; }
\{ { yylval = create_terminal_node(eLC); return LC; }
\} { yylval = create_terminal_node(eRC); return RC; }

. {
	error_occurred = 1;
	fprintf(stderr, "Error type A at Line %d: Mysterious characters \'%s\'\n",
	yylineno, yytext);
}

%%
/* {user subroutines} */

