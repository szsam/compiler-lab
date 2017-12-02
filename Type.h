#pragma once

#include <memory>
#include <vector>
#include <iostream>

struct Type
{
	enum TypeTag { BASIC, ARRAY, STRUCTURE, FUNCTION };
	TypeTag tag;

	Type(TypeTag t) : tag(t) {}
	virtual bool equal(const Type &rhs) const
	{
		return tag == rhs.tag;
	}
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
	enum BasicType { tINT, tFLOAT };
	BasicType basic_type;

	Basic(BasicType t) : Type(BASIC), basic_type(t) {}
	bool equal(const Type &rhs) const override
	{
		auto r = dynamic_cast<const Basic&>(rhs);
		return Type::equal(rhs) && basic_type == r.basic_type;
	}
	std::string to_string() const override
	{
		return basic_type == tINT ? std::string("int") : std::string("float");
	}
};

struct Function : public Type
{
	// return type
	std::shared_ptr<Type> ret_type;
	// types of parameters
	std::vector<std::shared_ptr<Type> > params;

	Function(std::shared_ptr<Type> t) : Type(FUNCTION), ret_type(t) {}
	bool equal(const Type &rhs) const override
	{
		auto r = dynamic_cast<const Function&>(rhs);
		return Type::equal(rhs) && *ret_type == *r.ret_type;
		// TODO compare params
	}

	std::string to_string() const override
	{
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

