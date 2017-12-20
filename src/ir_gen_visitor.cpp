#include "ir_gen_visitor.h"


// #include <algorithm>
#include <cassert>
#include <iostream>

ir::Variable InterCodeGenVisitor::new_temp()
{
	return ir::Variable("t" + std::to_string(++temp_no));
}

void InterCodeGenVisitor::visit(Program & node)
{
	for (auto it = node.ext_def_list.rbegin();
			it != node.ext_def_list.rend(); ++it)
	{
		(*it)->accept(*this);
		node.code.splice(node.code.end(), (*it)->code);
	}
}

void InterCodeGenVisitor::visit(GlobalVar & node)
{
	assert(0);
}

void InterCodeGenVisitor::visit(FunDec & node)
{
	node.code.push_back(std::make_shared<ir::Function>(node.name));

	for (const auto &param : node.ir_params)
		node.code.push_back(std::make_shared<ir::Param>(param));

	node.body->accept(*this);
	node.code.splice(node.code.end(), node.body->code);
}

void InterCodeGenVisitor::visit(BasicType & node)
{
	assert(0);
}

void InterCodeGenVisitor::visit(StructSpecifier & node)
{
	assert(0);
}

void InterCodeGenVisitor::visit(CompSt & node)
{
	// traverse statement list
	for (auto it_stmt = node.stmt_list.rbegin();
			it_stmt != node.stmt_list.rend(); ++it_stmt)
	{
		(*it_stmt)->accept(*this);
		node.code.splice(node.code.end(), (*it_stmt)->code);
	}
}

void InterCodeGenVisitor::visit(Return & node)
{
	auto t1 = new_temp();
	node.exp->place = t1;
	node.exp->accept(*this);
	node.code.splice(node.code.end(), node.exp->code);	
	node.code.push_back(std::make_shared<ir::Return>(
			std::make_shared<ir::Variable>(t1)));
}

void InterCodeGenVisitor::visit(Branch &node)
{
	int label1 = new_label();
	int label2 = new_label();
	int label3;

	node.cond->cond = true;
	node.cond->label_true = label1;
	node.cond->label_false = label2;
	node.cond->accept(*this);

	node.if_body->accept(*this);

	node.code.splice(node.code.end(), node.cond->code);
	node.code.push_back(std::make_shared<ir::Label>(label1));
	node.code.splice(node.code.end(), node.if_body->code);

	if (node.else_body)
	{
		label3 = new_label();
		node.else_body->accept(*this);
		node.code.push_back(std::make_shared<ir::Goto>(label3));
	}

	node.code.push_back(std::make_shared<ir::Label>(label2));

	if (node.else_body)
	{
		node.code.splice(node.code.end(), node.else_body->code);
		node.code.push_back(std::make_shared<ir::Label>(label3));
	}
}

/* translate expression */

void InterCodeGenVisitor::translate_exp(Identifier & node)
{
	if (node.place.empty()) return;
	node.code.push_back(std::make_shared<ir::Assign>(node.place, 
			std::make_shared<ir::Variable>(node.sym_info.ir_name)));
}

void InterCodeGenVisitor::translate_exp(Integer & node)
{
	if (node.place.empty()) return;
	node.code.push_back(std::make_shared<ir::Assign>(node.place, 
			std::make_shared<ir::Constant>(node.value)));
}

void InterCodeGenVisitor::translate_exp(Assign & node)
{
	auto t1 = new_temp();
	node.rhs->place = t1;
	node.rhs->accept(*this);
	node.code.splice(node.code.end(), node.rhs->code);

	auto id = std::dynamic_pointer_cast<Identifier>(node.lhs);
	if (id) 
	{
		const auto &id_name = id->sym_info.ir_name;
		node.code.push_back(std::make_shared<ir::Assign>(
				id_name, std::make_shared<ir::Variable>(t1)));
		if (!node.place.empty()) {
			node.code.push_back(std::make_shared<ir::Assign>(
					node.place, std::make_shared<ir::Variable>(id_name)));
		}
	}
	else assert(0);
}

void InterCodeGenVisitor::translate_exp(Negative & node)
{
	auto t1 = new_temp();	
	node.rhs->place = t1;
	node.rhs->accept(*this);
	node.code.splice(node.code.end(), node.rhs->code);
	if (!node.place.empty())
	{
		node.code.push_back(std::make_shared<ir::Minus>(
			node.place, 
			std::make_shared<ir::Constant>(0),
			std::make_shared<ir::Variable>(t1)));
	}
}

// translate exp for +,-,*,/
template <typename T>
void InterCodeGenVisitor::translate_exp_arith(BinaryOp & node)
{
	auto t1 = new_temp();
	auto t2 = new_temp();
	node.lhs->place = t1;
	node.rhs->place = t2;
	node.lhs->accept(*this);
	node.rhs->accept(*this);
	node.code.splice(node.code.end(), node.lhs->code);
	node.code.splice(node.code.end(), node.rhs->code);

	if (!node.place.empty())
	{
		node.code.push_back(std::make_shared<T>(
				node.place,
				std::make_shared<ir::Variable>(node.lhs->place),
				std::make_shared<ir::Variable>(node.rhs->place)));
	}
}

void InterCodeGenVisitor::translate_exp(Plus & node)
{ translate_exp_arith<ir::Plus>(node); }

void InterCodeGenVisitor::translate_exp(Minus & node)
{ translate_exp_arith<ir::Minus>(node); }

void InterCodeGenVisitor::translate_exp(Multiply & node)
{ translate_exp_arith<ir::Multiply>(node); }

void InterCodeGenVisitor::translate_exp(Divide & node)
{ translate_exp_arith<ir::Divide>(node); }


void InterCodeGenVisitor::translate_exp(FunCall & node)
{
	if (node.name == "read")
	{
		node.code.push_back(std::make_shared<ir::Read>(node.place));
	}
	else if (node.name == "write")
	{
		auto t1 = new_temp();
		node.args.front()->place = t1;
		node.args.front()->accept(*this);
		node.code.splice(node.code.end(), node.args.front()->code);
		node.code.push_back(std::make_shared<ir::Write>(
					std::make_shared<ir::Variable>(t1)));
	}
	else
	{
		std::vector<ir::Variable> arg_list;
		for (auto it = node.args.begin(); it != node.args.end(); ++it)
		{
			auto t1 = new_temp();
			(*it)->place = t1;
			(*it)->accept(*this);
			node.code.splice(node.code.end(), (*it)->code);
			arg_list.push_back(t1);
		}	
		for (const auto &arg : arg_list)
		{
			node.code.push_back(std::make_shared<ir::Arg>(
					std::make_shared<ir::Variable>(arg)));
		}
		node.code.push_back(std::make_shared<ir::FunCall>(
					node.name, node.place));
	}
}

template <typename T>
void InterCodeGenVisitor::translate_exp_logic(T &node)
{
	int label1 = new_label();
	int label2 = new_label();

	node.label_true = label1;
	node.label_false = label2;

	translate_cond(node);	// code1
	
	if (!node.place.empty()) 
	{
		// code0
		node.code.push_front(std::make_shared<ir::Assign>(node.place, 
				std::make_shared<ir::Constant>(0)));
	}

	node.code.push_back(std::make_shared<ir::Label>(label1));

	if (!node.place.empty()) 
	{
		node.code.push_back(std::make_shared<ir::Assign>(node.place, 
				std::make_shared<ir::Constant>(1)));
	}

	node.code.push_back(std::make_shared<ir::Label>(label2));
}

/* translate condition */
void InterCodeGenVisitor::translate_cond(Relop &node)
{
	auto t1 = new_temp();
	auto t2 = new_temp();

	node.lhs->place = t1;
	node.lhs->accept(*this);
	node.rhs->place = t2;
	node.rhs->accept(*this);

	node.code.splice(node.code.end(), node.lhs->code);
	node.code.splice(node.code.end(), node.rhs->code);
	node.code.push_back(std::make_shared<ir::CGoto>(
				std::make_shared<ir::Variable>(t1),
				std::make_shared<ir::Variable>(t2),
				node.op,
				node.label_true));
	node.code.push_back(std::make_shared<ir::Goto>(node.label_false));
}

void InterCodeGenVisitor::translate_cond(Not &node) 
{
	node.rhs->cond = true;
	node.rhs->label_true = node.label_false;
	node.rhs->label_false = node.label_true;

	node.rhs->accept(*this);
	node.code.splice(node.code.end(), node.rhs->code);
}

void InterCodeGenVisitor::translate_cond(And &node) 
{
	int label1 = new_label();
	node.lhs->cond = true;
	node.lhs->label_true = label1;
	node.lhs->label_false = node.label_false;
	node.lhs->accept(*this);

	node.rhs->cond = true;
	node.rhs->label_true = node.label_true;
	node.rhs->label_false = node.label_false;
	node.rhs->accept(*this);

	node.code.splice(node.code.end(), node.lhs->code);
	node.code.push_back(std::make_shared<ir::Label>(label1));
	node.code.splice(node.code.end(), node.rhs->code);
}

void InterCodeGenVisitor::translate_cond(Or &node) 
{
	int label1 = new_label();
	node.lhs->cond = true;
	node.lhs->label_true = node.label_true;
	node.lhs->label_false = label1;
	node.lhs->accept(*this);

	node.rhs->cond = true;
	node.rhs->label_true = node.label_true;
	node.rhs->label_false = node.label_false;
	node.rhs->accept(*this);

	node.code.splice(node.code.end(), node.lhs->code);
	node.code.push_back(std::make_shared<ir::Label>(label1));
	node.code.splice(node.code.end(), node.rhs->code);
}

template <typename T>
void InterCodeGenVisitor::translate_cond_others(T &node)
{
	auto t1 = new_temp();
	node.place = t1;
	translate_exp(node);

	node.code.push_back(std::make_shared<ir::CGoto>(
				std::make_shared<ir::Variable>(t1),
				std::make_shared<ir::Constant>(0),
				"!=",
				node.label_true));
	node.code.push_back(std::make_shared<ir::Goto>(node.label_false));
}
