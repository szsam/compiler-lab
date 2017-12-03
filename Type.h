#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <cassert>
#include <utility>

// #include "Env.h"
struct Env;

struct Type
{
	virtual bool equal(const Type &rhs) const = 0;
	virtual std::string to_string() const = 0;
};

inline bool operator==(const Type &lhs, const Type &rhs)
{
	return typeid(lhs) == typeid(rhs) && lhs.equal(rhs);
}

inline bool operator!=(const Type &lhs, const Type &rhs)
{
	return !(lhs == rhs);
}

struct Basic : public Type
{
	Basic() : Type() {}
//	bool equal(const Type &rhs) const override
//	{
//		return true;
//	}
};

struct IntegerT : public Basic
{
	bool equal(const Type &rhs) const override { return true; }
	std::string to_string() const override { return std::string("int"); }
};

struct FloatT : public Basic
{
	bool equal(const Type &rhs) const override { return true; }
	std::string to_string() const override { return std::string("float"); }
};

struct Function : public Type
{
	// return type
	std::shared_ptr<Type> ret_type;
	// types of parameters
	std::vector<std::shared_ptr<Type> > params;

	Function(std::shared_ptr<Type> t) : Type(), ret_type(t) {}
	bool equal(const Type &rhs) const override
	{
	// 	auto r = dynamic_cast<const Function&>(rhs);
	// 	return Type::equal(rhs) && *ret_type == *r.ret_type;
	// 	// TODO compare params
	    assert(0);
	 	return true;
	}

	std::string to_string() const override
	{
		assert(0);
		return std::string("not implemented");
	}

	/* check whether the given argument list mathches with 
	 * the parameter list of the function */
	bool match_args(std::vector<std::shared_ptr<Type> > args) const
	{
//		std::cout << "args: " << args[0]->to_string() << " " << args[1]->to_string() << "\n";
//		std::cout << "params: " << params[0]->to_string() << " " << params[1]->to_string() << "\n";

		return std::equal(args.begin(), args.end(), params.begin(), params.end(),
			[](std::shared_ptr<Type> a, std::shared_ptr<Type> b)->bool
			{ return *a == *b; });	
	}
};

struct Array : public Type
{
	int size;
	std::shared_ptr<Type> elem;

	Array(int s, std::shared_ptr<Type> e) : size(s), elem(e) { }

	bool equal(const Type &rhs) const override
	{
		auto r = dynamic_cast<const Array&>(rhs);
		auto p1 = base_and_dim();
		auto p2 = r.base_and_dim();

		return (p1.second == p2.second) && (*p1.first == *p2.first);
	}

	std::string to_string() const override
	{
		assert(0);
		return std::string("not implemented");
	}

	/* return base type and dimension of array */
	std::pair<std::shared_ptr<Type>, int> base_and_dim() const
	{
		int dimension = 1;
		auto p = elem;
		while (p && typeid(*p) == typeid(Array))
		{
			p = dynamic_cast<const Array &>(*p).elem;
			dimension++;
		}
		return {p, dimension};
	}
};

struct Structure : public Type
{
	std::shared_ptr<Env> fields;


	bool equal(const Type &rhs) const override
	{
		// auto r = dynamic_cast<const Array&>(rhs);
		assert(0);
		return true;
	}

	std::string to_string() const override
	{
		assert(0);
		return std::string("not implemented");
	}
};
