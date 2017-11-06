%{
#include "syntax.tab.h"

int yycolumn = 1;
#define YY_USER_ACTION \
	yylloc.first_line = yylloc.last_line = yylineno; \
	yylloc.first_column = yycolumn; \
	yylloc.last_column = yycolumn + yyleng - 1; \
	yycolumn += yyleng;
%}
%option yylineno

digit [0-9]
letter [_a-zA-Z]
delim [ \n\t]
ws {delim}+

INT			0|[1-9][0-9]*
FLOAT		{INT}\.{digit}+


%%
\n { yycolumn = 1; }
[ \t]+ { /* no action and no return */}

{INT} { printf("INT\n"); return INT; }
{FLOAT} { printf("FLOAT\n"); return FLOAT; }

struct	{ printf("STRUCT\n"); return STRUCT; }
return	{ printf("RETURN\n"); return RETURN; }
if		{ printf("IF	\n"); return IF;	 }
else    { printf("ELSE	\n"); return ELSE;	 }
while   { printf("WHILE	\n"); return WHILE;	 }

int|float { printf("TYPE\n"); return TYPE; }
{letter}({letter}|{digit})*	{ printf("ID\n"); return ID; }

; { printf("SEMI\n"); return SEMI; }
, { printf("COMMA\n"); return COMMA; }
= { printf("ASSIGNOP\n"); return ASSIGNOP; }

>|<|>=|<=|==|!=	{ printf("RELOP\n"); }

"+" { printf("PLUS\n"); return PLUS; }
-   { printf("MINUS\n");return MINUS; }
"*" { printf("STAR\n"); return STAR; }
"/" { printf("DIV\n");  return DIV; }
&&  { printf("AND\n");  return AND; }
"||" { printf("OR\n");   return OR; }
"." { printf("DOT\n");  return DOT; }
!   { printf("NOT\n");  return NOT; }

\( { printf("LP\n"); return LP; }
\) { printf("RP\n"); return RP; }
\[ { printf("LB\n"); return LB; }
\] { printf("RB\n"); return RB; }
\{ { printf("LC\n"); return LC; }
\} { printf("RC\n"); return RC; }

. {
	printf("Error type A at Line %d: Mysterious characters \'%s\'\n",
	yylineno, yytext);
}

%%
/* {user subroutines} */

