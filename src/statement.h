#pragma once

#include "declaration.h"

struct Statement : public ASTNode
{

};

struct CompSt : public Statement
{
	VEC<Def> def_list;
	VEC<SP<Statement>> stmt_list;

	CompSt(VEC<Def> &d, VEC<SP<Statement>> s) : def_list(d), stmt_list(s) {}
	DEFINE_ACCEPT
};

struct Return : public Statement
{
	SP<Expression> exp;

	Return(SP<Expression> e) : exp(e) {}
	DEFINE_ACCEPT
};

struct Branch : public Statement
{
	SP<Expression> cond;
	SP<Statement> if_body;
	SP<Statement> else_body;

	Branch(SP<Expression> c, SP<Statement> if_, SP<Statement> else_ = nullptr) :
		cond(c), if_body(if_), else_body(else_) {}
	DEFINE_ACCEPT
};

struct While : public Statement
{
	SP<Expression> cond;
	SP<Statement> body;

	While(SP<Expression> c, SP<Statement> b) :
		cond(c), body(b) {}
	DEFINE_ACCEPT
};
