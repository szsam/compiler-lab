#pragma once

#include <string>
#include <unordered_map>
#include <memory>
#include <list>

#include "Type.h"
#include "ir.h"
// struct Type;
struct SymbolInfo
{
	std::shared_ptr<Type> type;
	// variable name used in intermediate code
	ir::Variable ir_name;

	SymbolInfo() = default;
	explicit SymbolInfo(std::shared_ptr<Type> t) : type(t) {}
	SymbolInfo(std::shared_ptr<Type> t, const ir::Variable &var) 
		: type(t), ir_name(var) {}
};

class SymbolTable
{
private:
	std::list< std::unordered_map<std::string, SymbolInfo> > tables;

public:
	void enter_scope() { tables.emplace_back(); }
	void exit_scope() { tables.pop_back(); }

	bool put(const std::string &name, const SymbolInfo &sym_info)
	{
		return tables.back().insert({name, sym_info}).second;
	}

	const SymbolInfo *get(const std::string &name) const
	{
		for (auto it_table = tables.crbegin(); it_table != tables.crend(); ++it_table)
		{
			auto it = it_table->find(name);
			if (it != it_table->end()) return &it->second;
		}
		return nullptr;
	}
};
