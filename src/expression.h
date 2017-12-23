#pragma once

#include "declaration.h"
#include "statement.h"
#include "symbol_table.h"

struct Expression : public Statement
{
	// type of expression
	SP<Type> type;
	// the expression is required to be an lvalue
	bool require_lvalue = false;
	// the expression is an lvalue
	bool is_lvalue = false;

	// used when translated as ordinary expression
	ir::Variable place;
	// is translated as conditional expression
	bool cond;	
	// used when translated as conditional expression
	int label_true, label_false;
	// DEFINE_ACCEPT
};

struct BinaryOp : public virtual Expression
{
	SP<Expression> lhs;
	SP<Expression> rhs;

	BinaryOp(SP<Expression> l, SP<Expression> r) :
		lhs(l), rhs(r) {}
};

struct UnaryOp : public virtual Expression
{
	SP<Expression> rhs;

	UnaryOp(SP<Expression> r) : rhs(r) {}
};

struct Logic : public virtual Expression
{
};

struct Arith : public virtual Expression
{
};

struct Assign : public BinaryOp
{
	Assign(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Subscript : public BinaryOp
{
	Subscript(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Not : public UnaryOp, public Logic
{
	Not(SP<Expression> r) : UnaryOp(r) {}
	DEFINE_ACCEPT
};

struct And : public BinaryOp, public Logic
{
	And(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Or : public BinaryOp, public Logic
{
	Or(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Relop : public BinaryOp, public Logic
{
	std::string op;

	Relop(SP<Expression> l, SP<Expression> r, const std::string &s) : 
		BinaryOp(l, r) ,op(s) {}
	DEFINE_ACCEPT
};

struct Plus : public BinaryOp, public Arith
{
	Plus(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Minus : public BinaryOp, public Arith
{
	Minus(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Multiply : public BinaryOp, public Arith
{
	Multiply(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Divide : public BinaryOp, public Arith
{
	Divide(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Negative : public UnaryOp, public Arith
{
	Negative(SP<Expression> r) : UnaryOp(r) {}
	DEFINE_ACCEPT
};

struct FunCall : public Expression
{
	std::string name;
	VEC<SP<Expression>> args;

	FunCall(const std::string &n, const VEC<SP<Expression>> &a = VEC<SP<Expression>>()) :
		name(n), args(a) {}
	DEFINE_ACCEPT
};

struct MemberAccess : public Expression
{
	SP<Expression> object;
	std::string member;

	MemberAccess(SP<Expression> exp, const std::string &str) 
		: object(exp), member(str) {}
	DEFINE_ACCEPT
};

struct Identifier : public Expression
{
	std::string id;
	SymbolInfo sym_info;

	Identifier(const std::string &s) : id(s) {}
	DEFINE_ACCEPT
};

struct Integer : public Expression
{
	int value;

	Integer(int v) : value(v) {}
	DEFINE_ACCEPT
};

struct Float : public Expression
{
	float value;

	Float(float v) : value(v) {}
	DEFINE_ACCEPT
};

