#include "type_check_visitor.h"


// #include <algorithm>
#include <cassert>
#include <iostream>

void TypeCheckVisitor::visit(Program & node)
{
	for (auto it = node.ext_def_list.rbegin();
			it != node.ext_def_list.rend(); ++it)
	{
		(*it)->accept(*this);
	}
}

void TypeCheckVisitor::visit(GlobalVar & node)
{
	assert(0);
}

void TypeCheckVisitor::visit(FunDec & node)
{
	node.body->accept(*this);
}

void TypeCheckVisitor::visit(CompSt & node)
{
	// traverse statement list
	for (auto it_stmt = node.stmt_list.rbegin();
			it_stmt != node.stmt_list.rend(); ++it_stmt)
	{
		(*it_stmt)->accept(*this);
	}
}

void TypeCheckVisitor::visit(Return & node)
{
	node.exp->accept(*this);
	// TODO check type of return value
}

void TypeCheckVisitor::visit(Branch & node)
{
	node.cond->accept(*this);
	node.if_body->accept(*this);
	if (node.else_body)
		node.else_body->accept(*this);
}

void TypeCheckVisitor::visit(While & node)
{
	node.cond->accept(*this);
	node.body->accept(*this);
}

void TypeCheckVisitor::visit(FunCall & node)
{
	auto fun_type = std::dynamic_pointer_cast<Function>(node.sym_info.type);
	if (!fun_type) assert(0);
	node.type = fun_type->ret_type;

	for (auto it = node.args.rbegin(); it != node.args.rend(); ++it)
	{
		(*it)->accept(*this);
	}
	// TODO check whether types of arguments match with types of parameters
}

void TypeCheckVisitor::visit(Identifier & node)
{
	node.type = node.sym_info.type;
	node.is_lvalue = true;
}

void TypeCheckVisitor::visit_arith(BinaryOp & node)
{
	node.lhs->accept(*this);
	node.rhs->accept(*this);

	if (!node.lhs->type || !node.rhs->type) return;

	if (*node.lhs->type == *node.rhs->type && node.lhs->type->is_basic())
	{
		node.type = node.lhs->type;
	}
	else assert(0);
}

void TypeCheckVisitor::visit_logic(BinaryOp & node)
{
	node.lhs->accept(*this);
	node.rhs->accept(*this);

	if (!node.lhs->type || !node.rhs->type) return;

	if (*node.lhs->type == *node.rhs->type && node.lhs->type->is_integer())
	{
		node.type = node.lhs->type;
	}
	else assert(0);
}

void TypeCheckVisitor::visit(Negative & node)
{
	node.rhs->accept(*this);

	if (!node.rhs->type) return;

	if (node.rhs->type->is_basic())
	{
		node.type = node.rhs->type;
	}
	else assert(0);
}

void TypeCheckVisitor::visit(Not & node)
{
	node.rhs->accept(*this);

	if (!node.rhs->type) return;

	if (node.rhs->type->is_integer())
	{
		node.type = node.rhs->type;
	}
	else assert(0);
}

void TypeCheckVisitor::visit(Subscript & node)
{
	node.lhs->accept(*this);
	node.rhs->accept(*this);
	
	if (node.lhs->type)
	{
		auto parray = std::dynamic_pointer_cast<Array>(node.lhs->type);
		if (parray) node.type = parray->elem;
		else assert(0);
	}

	if (node.rhs->type && !node.rhs->type->is_integer())
	{
		assert(0);	
	}
	
	node.is_lvalue = true;
}

void TypeCheckVisitor::visit(Assign & node)
{
	node.lhs->accept(*this);
	node.rhs->accept(*this);

	if (!node.lhs->type || !node.rhs->type) return;

	if (*node.lhs->type != *node.rhs->type)
	{
		assert(0);
	}

	if (!node.lhs->is_lvalue) assert(0);

	node.lhs->require_lvalue = true;
	node.type = node.lhs->type;
}
