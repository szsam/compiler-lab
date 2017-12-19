#pragma once

#include "declaration.h"
#include "statement.h"
#include "symbol_table.h"

struct Expression : public Statement
{
	// SP<Type> type;
	// used when translated as ordinary expression
	ir::Variable place;
	// is translated as conditional expression
	bool cond;	
	// used when translated as conditional expression
	int label_true, label_false;
	// DEFINE_ACCEPT
};

struct BinaryOp : public Expression
{
	SP<Expression> lhs;
	SP<Expression> rhs;

	BinaryOp(SP<Expression> l, SP<Expression> r) :
		lhs(l), rhs(r) {}
};

struct UnaryOp : public Expression
{
	SP<Expression> rhs;

	UnaryOp(SP<Expression> r) : rhs(r) {}
};

struct FunCall : public Expression
{
	std::string name;
	VEC<SP<Expression>> args;

	FunCall(const std::string &n, const VEC<SP<Expression>> &a = VEC<SP<Expression>>()) :
		name(n), args(a) {}
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

struct Assign : public BinaryOp
{
	Assign(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Logic : public BinaryOp
{
	Logic(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
};

struct Arith : public BinaryOp
{
	Arith(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
};

struct Relop : public BinaryOp
{
	std::string op;

	Relop(SP<Expression> l, SP<Expression> r, const std::string &s) : 
		BinaryOp(l, r) ,op(s) {}
	DEFINE_ACCEPT
};

struct Subscript : public BinaryOp
{
	Subscript(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct MemberAccess : public BinaryOp
{
	MemberAccess(SP<Expression> l, SP<Expression> r) : BinaryOp(l, r) {}
	DEFINE_ACCEPT
};

struct Negative : public UnaryOp
{
	Negative(SP<Expression> r) : UnaryOp(r) {}
	DEFINE_ACCEPT
};

struct Not : public UnaryOp
{
	Not(SP<Expression> r) : UnaryOp(r) {}
	DEFINE_ACCEPT
};

struct And : public Logic
{
	And(SP<Expression> l, SP<Expression> r) : Logic(l, r) {}
	DEFINE_ACCEPT
};

struct Or : public Logic
{
	Or(SP<Expression> l, SP<Expression> r) : Logic(l, r) {}
	DEFINE_ACCEPT
};

struct Plus : public Arith
{
	Plus(SP<Expression> l, SP<Expression> r) : Arith(l, r) {}
	DEFINE_ACCEPT
};

struct Minus : public Arith
{
	Minus(SP<Expression> l, SP<Expression> r) : Arith(l, r) {}
	DEFINE_ACCEPT
};

struct Multiply : public Arith
{
	Multiply(SP<Expression> l, SP<Expression> r) : Arith(l, r) {}
	DEFINE_ACCEPT
};

struct Divide : public Arith
{
	Divide(SP<Expression> l, SP<Expression> r) : Arith(l, r) {}
	DEFINE_ACCEPT
};
