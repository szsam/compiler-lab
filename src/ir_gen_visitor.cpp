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
	// node.params;
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

void InterCodeGenVisitor::visit(Identifier & node)
{
	if (node.place.empty()) return;
	std::string var = "v" + std::to_string(node.sym_info.var_no);
	node.code.push_back(std::make_shared<ir::Assign>(node.place, 
			std::make_shared<ir::Variable>(var)));
}

void InterCodeGenVisitor::visit(Integer & node)
{
	if (node.place.empty()) return;
	node.code.push_back(std::make_shared<ir::Assign>(node.place, 
			std::make_shared<ir::Constant>(node.value)));
}

void InterCodeGenVisitor::visit(Assign & node)
{
	auto t1 = new_temp();
	node.rhs->place = t1;
	node.rhs->accept(*this);
	node.code.splice(node.code.end(), node.rhs->code);

	auto id = std::dynamic_pointer_cast<Identifier>(node.lhs);
	if (id) 
	{
		std::string idname = "v" + std::to_string(id->sym_info.var_no);
		node.code.push_back(std::make_shared<ir::Assign>(
				ir::Variable(idname), std::make_shared<ir::Variable>(t1)));
		if (!node.place.empty()) {
			node.code.push_back(std::make_shared<ir::Assign>(
					node.place, std::make_shared<ir::Variable>(idname)));
		}
	}
	else assert(0);
}

void InterCodeGenVisitor::visit(Negative & node)
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

template <typename T>
void InterCodeGenVisitor::visit_arith(Arith & node)
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

void InterCodeGenVisitor::visit(Plus & node)
{ visit_arith<ir::Plus>(node); }

void InterCodeGenVisitor::visit(Minus & node)
{ visit_arith<ir::Minus>(node); }

void InterCodeGenVisitor::visit(Multiply & node)
{ visit_arith<ir::Multiply>(node); }

void InterCodeGenVisitor::visit(Divide & node)
{ visit_arith<ir::Divide>(node); }


void InterCodeGenVisitor::visit(FunCall & node)
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
	else assert(0);
}
