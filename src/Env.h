#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <list>

#include "Type.h"
// struct Type;
struct Symbol
{
	std::shared_ptr<Type> type;
	// variable number, used in intermediate code
	int var_no;

	Symbol() : type(), var_no(-1) {}
	Symbol(std::shared_ptr<Type> t) : type(t), var_no(-1) {}
	Symbol(std::shared_ptr<Type> t, int no) : type(t), var_no(no) {}
};

class SymbolTable
{
private:
	std::list< std::unordered_map<std::string, Symbol> > tables;
	int variable_no = 0;

	// methods

public:
	void enter_scope() { tables.emplace_back(); }
	void exit_scope() { tables.pop_back(); }

	bool put(const std::string &name, std::shared_ptr<Type> type)
	{
		Symbol sym(type);
		if (type->is_basic()) 
		{
			++variable_no;
			sym.var_no = variable_no;
		}
		return tables.back().insert({name, sym}).second;
	}

	const Symbol *get(const std::string &name) const
	{
		for (auto it_table = tables.crbegin(); it_table != tables.crend(); ++it_table)
		{
			auto it = it_table->find(name);
			if (it != it_table->end()) return &it->second;
		}
		return nullptr;
	}
};
