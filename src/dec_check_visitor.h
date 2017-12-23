#pragma once
#include "visitor.h"
#include "symbol_table.h"
#include "declaration.h"
#include "statement.h"
#include "expression.h"
#include "ir.h"

// #include <iostream>

struct DecCheckVisitor : public Visitor
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
	void visit(While &node);

	void visit(Plus &node) { visit_bop(node); }
	void visit(Minus &node) { visit_bop(node); }
	void visit(Multiply &node) { visit_bop(node); }
	void visit(Divide &node) { visit_bop(node); }
	void visit(Negative &node) { node.rhs->accept(*this); }
	void visit(And &node) { visit_bop(node); }
	void visit(Or &node) { visit_bop(node); }
	void visit(Not &node) { node.rhs->accept(*this); }
	void visit(Relop &node) { visit_bop(node); }
	void visit(Subscript &node) { assert(0); }
	void visit(MemberAccess &node) { assert(0); }
	void visit(Assign &node) { visit_bop(node); }
	void visit(FunCall &node);
	void visit(Identifier &node);
	void visit(Integer &node) {}
	void visit(Float &node) {}

private:
	SymbolTable table;
	int variable_no = 0;

	void visit_bop(BinaryOp &node);
	ir::Variable new_var() 
	{ 
		return ir::Variable("v" + std::to_string(++variable_no)); 
	}

	std::shared_ptr<Type> construct_array_type(
			std::shared_ptr<Type> base_type, const std::vector<int> &indices);
};
