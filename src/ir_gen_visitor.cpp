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

//	for (const auto &param : node.ir_params)
//		node.code.push_back(std::make_shared<ir::Param>(param));
	for (auto it = node.params.rbegin(); it != node.params.rend(); ++it)
	{
		node.code.push_back(std::make_shared<ir::Param>(
					it->second->sym_info.ir_name));
	}

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
	// traverse def_list
	for (auto it_def = node.def_list.rbegin(); 
			it_def != node.def_list.rend(); ++it_def)
	{
		for (auto it_dec = it_def->dec_list.rbegin();
					it_dec != it_def->dec_list.rend(); ++it_dec)
		{
			if (it_dec->initial)
			{
				auto t1 = new_temp();
				it_dec->initial->place = t1;
				it_dec->initial->accept(*this);
				node.code.splice(node.code.end(), it_dec->initial->code);
				node.code.push_back(std::make_shared<ir::Assign>(
						it_dec->var_dec.sym_info.ir_name,	
						std::make_shared<ir::Variable>(t1)));
			}

			// array or structure
			auto type = it_dec->var_dec.sym_info.type;
			if (!type->is_basic())
			{
				auto t1 = new_temp();
				node.code.push_back(std::make_shared<ir::Declare>(
						t1,	type->width));
				node.code.push_back(std::make_shared<ir::Assign>(
						it_dec->var_dec.sym_info.ir_name,
						std::make_shared<ir::Variable>(t1, ir::Variable::ADDR)));
			}

		}

	}

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

void InterCodeGenVisitor::visit(While &node)
{
	int label1 = new_label();
	int label2 = new_label();
	int label3 = new_label();

	node.cond->cond = true;
	node.cond->label_true = label2;
	node.cond->label_false = label3;
	node.cond->accept(*this);

	node.body->accept(*this);

	node.code.push_back(std::make_shared<ir::Label>(label1));
	node.code.splice(node.code.end(), node.cond->code);
	node.code.push_back(std::make_shared<ir::Label>(label2));
	node.code.splice(node.code.end(), node.body->code);
	node.code.push_back(std::make_shared<ir::Goto>(label1));
	node.code.push_back(std::make_shared<ir::Label>(label3));
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
	assert(node.type->is_integer());

	auto t1 = new_temp();
	node.rhs->place = t1;
	node.rhs->accept(*this);
	node.code.splice(node.code.end(), node.rhs->code);

	if (auto id = std::dynamic_pointer_cast<Identifier>(node.lhs)) 
	{
		const auto &id_name = id->sym_info.ir_name;
		node.code.push_back(std::make_shared<ir::Assign>(
				id_name, std::make_shared<ir::Variable>(t1)));
		if (!node.place.empty()) {
			node.code.push_back(std::make_shared<ir::Assign>(
					node.place, std::make_shared<ir::Variable>(id_name)));
		}
	}
	else if (auto subscript = std::dynamic_pointer_cast<Subscript>(node.lhs))
	{
		auto addr = new_temp();
		node.lhs->place = addr;
		node.lhs->accept(*this);
		node.code.splice(node.code.end(), node.lhs->code);

		auto star_addr = ir::Variable(addr, ir::Variable::DEREF);
		node.code.push_back(std::make_shared<ir::Assign>(
				star_addr, std::make_shared<ir::Variable>(t1)));
		if (!node.place.empty()) {
			node.code.push_back(std::make_shared<ir::Assign>(
					node.place, std::make_shared<ir::Variable>(star_addr)));
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
		if (node.place.empty()) node.place = new_temp();
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

void InterCodeGenVisitor::translate_exp(Subscript &node)
{
	// node.lhs->type must be an Array, after type checking
	auto array_type = std::static_pointer_cast<Array>(node.lhs->type);
	auto elem_width = std::make_shared<ir::Constant>(array_type->elem->width);

	auto base_addr = new_temp();
	node.lhs->place = base_addr;
	node.lhs->accept(*this);
	node.code.splice(node.code.end(), node.lhs->code);

	auto index = new_temp();
	node.rhs->place = index;
	node.rhs->accept(*this);
	node.code.splice(node.code.end(), node.rhs->code);
	
	node.code.push_back(std::make_shared<ir::Multiply>(node.place, elem_width, 
				std::make_shared<ir::Variable>(index)));
	node.code.push_back(std::make_shared<ir::Plus>(node.place, 
				std::make_shared<ir::Variable>(node.place), 
				std::make_shared<ir::Variable>(base_addr)));


	if (!node.require_lvalue && node.type->is_basic())
	{
		auto star_place = ir::Variable(node.place, ir::Variable::DEREF);
		node.code.push_back(std::make_shared<ir::Assign>(node.place, 
					std::make_shared<ir::Variable>(star_place)));
	}

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
