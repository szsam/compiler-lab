#include "dec_check_visitor.h"


// #include <algorithm>
#include <cassert>
#include <iostream>

void DecCheckVisitor::visit(Program & node)
{
	table.enter_scope();
	for (auto it = node.ext_def_list.rbegin();
			it != node.ext_def_list.rend(); ++it)
	{
		(*it)->accept(*this);
	}
	table.exit_scope();
}

void DecCheckVisitor::visit(GlobalVar & node)
{
	assert(0);
}

void DecCheckVisitor::visit(FunDec & node)
{
	table.enter_scope();
	node.body->accept(*this);
	table.exit_scope();
}

void DecCheckVisitor::visit(BasicType & node)
{
	if (node.str == "int")
		node.type = std::make_shared<IntegerT>();
	else
		node.type = std::make_shared<FloatT>();
}

void DecCheckVisitor::visit(StructSpecifier & node)
{
	assert(0);
}

void DecCheckVisitor::visit(CompSt & node)
{
	// traverse def_list
	for (auto it_def = node.def_list.rbegin(); 
			it_def != node.def_list.rend(); ++it_def)
	{
		it_def->specifier->accept(*this);
		auto base_type = it_def->specifier->type;

		for (auto it_dec = it_def->dec_list.rbegin();
					it_dec != it_def->dec_list.rend(); ++it_dec)
		{
			// it_dec->initial
			if (it_dec->var_dec.indices.empty())
			{
				const auto &id = it_dec->var_dec.id;
				bool ret = table.put(id, base_type);
				assert(ret);
			}
			else assert(0);
		}
	}

	// traverse statement list
	for (auto it_stmt = node.stmt_list.rbegin();
			it_stmt != node.stmt_list.rend(); ++it_stmt)
	{
		(*it_stmt)->accept(*this);
	}
}

void DecCheckVisitor::visit(Return & node)
{
	node.exp->accept(*this);
}



void DecCheckVisitor::visit_bop(BinaryOp & node)
{
	node.lhs->accept(*this);
	node.rhs->accept(*this);
}

void DecCheckVisitor::visit(Identifier & node)
{
	auto psym = table.get(node.id);
	if (psym) {
		node.sym_info = *psym;
		// std::cout << node.id << ": v" << psym->var_no << std::endl;
	}
	else assert(0);
}

void DecCheckVisitor::visit(FunCall & node)
{
	for (auto it = node.args.rbegin(); it != node.args.rend(); ++it)
	{
		(*it)->accept(*this);
	}
}
