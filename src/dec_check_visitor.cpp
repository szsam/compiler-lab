#include "dec_check_visitor.h"


// #include <algorithm>
#include <cassert>
#include <iostream>

DecCheckVisitor::DecCheckVisitor()
{
	// Add predefined functions `int read()` and `int write(int)`
	table.enter_scope();

	table.put("read", SymbolInfo(
			std::make_shared<Function>(std::make_shared<IntegerT>()),
			ir::Variable()));

	auto fun_write = std::make_shared<Function>(std::make_shared<IntegerT>());
	fun_write->params.push_back(std::make_shared<IntegerT>());
	table.put("write", SymbolInfo(fun_write, ir::Variable()));
}

DecCheckVisitor::~DecCheckVisitor()
{
	table.exit_scope();
}

std::shared_ptr<Type> DecCheckVisitor::construct_array_type(
	std::shared_ptr<Type> base_type, const std::vector<int> &indices)
{
	if (indices.empty()) return base_type;

	std::shared_ptr<Type> subtype = base_type;
	for (auto it = indices.rbegin(); it != indices.rend(); ++it)
	{
		subtype = std::make_shared<Array>(*it, subtype);
	}
	return subtype;
}

void DecCheckVisitor::visit(Program & node)
{
	for (auto it = node.ext_def_list.rbegin();
			it != node.ext_def_list.rend(); ++it)
	{
		(*it)->accept(*this);
	}
}

void DecCheckVisitor::visit(GlobalVar & node)
{
	assert(0);
}

void DecCheckVisitor::visit(FunDec & node)
{
	node.ret_type->accept(*this);

	auto fun_type = std::make_shared<Function>(node.ret_type->type);
	SymbolInfo fun_info(fun_type, ir::Variable());
	// save function's prototype in symbol table
	assert(table.put(node.name, fun_info));

	table.enter_scope();

	for (auto it = node.params.rbegin(); it != node.params.rend(); ++it)
	{
		it->first->accept(*this);
		assert(it->second->indices.empty());
		SymbolInfo sym_info(it->first->type, new_var());
		table.put(it->second->id, sym_info);
		it->second->sym_info = sym_info;

		// save type of parameters in function's symbol table entry
		fun_type->params.push_back(it->first->type);
	}

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
			if (it_dec->initial)
				it_dec->initial->accept(*this);

			auto type = construct_array_type(base_type, it_dec->var_dec.indices);
			const auto &id = it_dec->var_dec.id;
#ifdef DEBUG
			std::cout << id << ": " << type->to_string() << std::endl;
#endif
			SymbolInfo sym_info(type, new_var());
			bool ret = table.put(id, sym_info);
			it_dec->var_dec.sym_info = sym_info;
			assert(ret);
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

void DecCheckVisitor::visit(Branch & node)
{
	node.cond->accept(*this);
	node.if_body->accept(*this);
	if (node.else_body)
		node.else_body->accept(*this);
}

void DecCheckVisitor::visit(While & node)
{
	node.cond->accept(*this);
	node.body->accept(*this);
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
	auto psym = table.get(node.name);
	assert(psym);
	node.sym_info = *psym;

	for (auto it = node.args.rbegin(); it != node.args.rend(); ++it)
	{
		(*it)->accept(*this);
	}
}
