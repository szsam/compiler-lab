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
	void visit(While &node);

	void visit(Plus &node)			{ visit_exp(node); }
	void visit(Minus &node)			{ visit_exp(node); }
	void visit(Multiply &node)		{ visit_exp(node); }
	void visit(Divide &node)		{ visit_exp(node); }
	void visit(Negative &node)		{ visit_exp(node); }
	void visit(And &node)			{ visit_exp(node); }
	void visit(Or &node)			{ visit_exp(node); }
	void visit(Not &node)			{ visit_exp(node); }
	void visit(Relop &node)			{ visit_exp(node); }
	void visit(Subscript &node)		{ visit_exp(node); }
	void visit(MemberAccess &node)	{ visit_exp(node); }
	void visit(Assign &node)		{ visit_exp(node); }
	void visit(FunCall &node)		{ visit_exp(node); }
	void visit(Identifier &node)	{ visit_exp(node); }
	void visit(Integer &node)		{ visit_exp(node); }
	void visit(Float &node)			{ visit_exp(node); }

	void output(std::ostream &os) const;

	// intermediate-code of the program, a list of code of functions
	std::list<std::list<std::shared_ptr<ir::InterCode>>> inter_code;

private:
	template <typename T>
	void translate_exp_arith(BinaryOp &node);

	template <typename T>
	void translate_exp_logic(T &node);

	template <typename T>
	void translate_cond_others(T &node);

	void translate_exp(Plus &node);
	void translate_exp(Minus &node);
	void translate_exp(Multiply &node);
	void translate_exp(Divide &node);
	void translate_exp(Negative &node);
	void translate_exp(And &node) { translate_exp_logic(node); }
	void translate_exp(Or &node) { translate_exp_logic(node); }
	void translate_exp(Not &node) { translate_exp_logic(node); }
	void translate_exp(Relop &node) { translate_exp_logic(node); }
	void translate_exp(Subscript &node);
	void translate_exp(MemberAccess &node) { assert(0); }
	void translate_exp(Assign &node);
	void translate_exp(FunCall &node);
	void translate_exp(Identifier &node);
	void translate_exp(Integer &node);
	void translate_exp(Float &node) { assert(0); }

	void translate_cond(Plus &node) { translate_cond_others(node); }
	void translate_cond(Minus &node) { translate_cond_others(node); }
	void translate_cond(Multiply &node) { translate_cond_others(node); }
	void translate_cond(Divide &node) { translate_cond_others(node); }
	void translate_cond(Negative &node) { translate_cond_others(node); }
	void translate_cond(And &node);
	void translate_cond(Or &node);
	void translate_cond(Not &node);
	void translate_cond(Relop &node);
	void translate_cond(Subscript &node) { translate_cond_others(node); }
	void translate_cond(MemberAccess &node) { translate_cond_others(node); }
	void translate_cond(Assign &node) { translate_cond_others(node); }
	void translate_cond(FunCall &node) { translate_cond_others(node); }
	void translate_cond(Identifier &node) { translate_cond_others(node); }
	void translate_cond(Integer &node) { translate_cond_others(node); }
	void translate_cond(Float &node) { assert(0); }

	template <typename T>
	void visit_exp(T &node) { 
		node.cond ? translate_cond(node) : translate_exp(node);
	}

	ir::Variable new_temp();
	int new_label() { return ++label_no; }

	int temp_no = 0;
	int label_no = 0;

};
