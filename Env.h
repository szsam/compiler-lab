#pragma once

#include <string>
#include <map>
#include <memory>

#include "Type.h"

struct Env
{
	std::map<std::string,
			 std::shared_ptr<Type> > table;
	std::shared_ptr<Env> prev;

	// methods
	Env(std::shared_ptr<Env> p) : prev(p) {}	

	bool put(std::string name, std::shared_ptr<Type> type)
	{
		return table.insert({name, type}).second;
	}

	std::shared_ptr<Type> get(std::string name)
	{
		for (std::shared_ptr<Env> p(this); p != nullptr; p = p->prev)
		{
			auto it = table.find(name);
			if (it != table.end()) return it->second;
		}
		return nullptr;
	}
};
