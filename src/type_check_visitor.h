#pragma once
#include "visitor.h"
#include "declaration.h"
#include "statement.h"
#include "expression.h"

// #include <iostream>

// type check, distinguish between lvalue and rvalue
struct TypeCheckVisitor : public Visitor
{
	// DecCheckVisitor() : {}

	void visit(Program &node);

	//void visit(VarDec &node);
	//void visit(Dec &node);
	//void visit(Def &node);
	void visit(GlobalVar &node);
	void visit(FunDec &node);
	void visit(BasicType &node) { assert(0); }
	void visit(StructSpecifier &node) { assert(0); }

	void visit(CompSt &node);
	void visit(Return &node);
	void visit(Branch &node);
	void visit(While &node);

	void visit(Plus &node) { visit_arith(node); }
	void visit(Minus &node) { visit_arith(node); }
	void visit(Multiply &node) { visit_arith(node); }
	void visit(Divide &node) { visit_arith(node); }
	void visit(Negative &node);
	void visit(And &node) { visit_logic(node); }
	void visit(Or &node) { visit_logic(node); }
	void visit(Not &node);
	void visit(Relop &node) { visit_logic(node); }
	void visit(Subscript &node);
	void visit(MemberAccess &node) { assert(0); }
	void visit(Assign &node);
	void visit(FunCall &node);
	void visit(Identifier &node);
	void visit(Integer &node) { node.type = std::make_shared<IntegerT>(); }
	void visit(Float &node) { node.type = std::make_shared<FloatT>(); }

private:
	void visit_arith(BinaryOp &node);
	void visit_logic(BinaryOp &node);
};
