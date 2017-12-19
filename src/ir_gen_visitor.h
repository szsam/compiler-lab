#pragma once
#include "visitor.h"
// #include "symbol_table.h"
#include "declaration.h"
#include "statement.h"
#include "expression.h"

// #include <iostream>

// Intermediate Code Generation
struct InterCodeGenVisitor : public Visitor
{
	// DecCheckVisitor() : {}

	void visit(Program &node);

	//void visit(VarDec &node);
	//void visit(Dec &node);
	//void visit(Def &node);
	void visit(GlobalVar &node);
	void visit(FunDec &node);
	void visit(BasicType &node);
	void visit(StructSpecifier &node);

	void visit(CompSt &node);
	void visit(Return &node);
	void visit(Branch &node);
	void visit(While &node) { assert(0); }

	void visit(Plus &node);
	void visit(Minus &node);
	void visit(Multiply &node);
	void visit(Divide &node);
	void visit(Negative &node);
	void visit(And &node) { assert(0); }
	void visit(Or &node) { assert(0); }
	void visit(Not &node) { assert(0); }
	void visit(Relop &node) { translate_cond(node); }
	void visit(Subscript &node) { assert(0); }
	void visit(MemberAccess &node) { assert(0); }
	void visit(Assign &node);
	void visit(FunCall &node);
	void visit(Identifier &node);
	void visit(Integer &node);
	void visit(Float &node) { assert(0); }

private:
	template <typename T>
	void visit_arith(Arith &node);

	ir::Variable new_temp();
	int new_label() { return ++label_no; }

	void translate_cond(Relop &node);
//	void visit(Expression &node) { 
//		node.cond ? translate_cond(node) : translate_exp(node);
//	}

	int temp_no = 0;
	int label_no = 0;
};
