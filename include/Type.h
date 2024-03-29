#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <cassert>
#include <utility>
#include <algorithm>

// #include "Env.h"
// struct Env;

struct Type
{
	Type(int w) : width(w) {}
	virtual bool equal(const Type &rhs) const = 0;
	virtual std::string to_string() const = 0;
	virtual bool is_basic() const { return false; };
	virtual bool is_integer() const { return false; };
	virtual bool is_array() const { return false; };

	// number of bytes needed for objects of this type
	int width;
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
	Basic() : Type(4) {}
	bool is_basic() const override { return true; }
	bool equal(const Type &rhs) const override { return true; }
};

struct IntegerT : public Basic
{
	// bool equal(const Type &rhs) const override { return true; }
	std::string to_string() const override { return std::string("int"); }
	bool is_integer() const override { return true; }
};

struct FloatT : public Basic
{
	// bool equal(const Type &rhs) const override { return true; }
	std::string to_string() const override { return std::string("float"); }
};

struct Function : public Type
{
	// return type
	std::shared_ptr<Type> ret_type;
	// types of parameters
	std::vector<std::shared_ptr<Type> > params;

	Function(std::shared_ptr<Type> t) : Type(0), ret_type(t) {}

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

	Array(int s, std::shared_ptr<Type> e) : Type(s * e->width), size(s), elem(e) { }

	bool is_array() const override { return true; };

	bool equal(const Type &rhs) const override
	{
		auto r = dynamic_cast<const Array&>(rhs);
		auto p1 = base_and_dim();
		auto p2 = r.base_and_dim();

		return (p1.second == p2.second) && (*p1.first == *p2.first);
	}

	std::string to_string() const override
	{
		return "array(" + elem->to_string() + ", " + std::to_string(size) + ")";
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

struct StructField
{
	std::string name;
	std::shared_ptr<Type> type;

	StructField(std::string n, std::shared_ptr<Type> t) : name(n), type(t) {}
	bool operator==(const StructField &rhs) const { return *type == *rhs.type; }
};

class Structure : public Type
{
public:
	Structure() = default;

	bool equal(const Type &rhs) const override
	{
		auto r = dynamic_cast<const Structure&>(rhs);
		return std::equal(fields.begin(), fields.end(), r.fields.begin(), r.fields.end());
	}

	std::string to_string() const override
	{
		assert(0);
		return std::string("not implemented");
	}

	bool add_field(std::string n, std::shared_ptr<Type> t)
	{
		if (get_field_type(n)) return false;
		else { 
			fields.emplace_back(n, t);
			return true;
		}
	}

	std::shared_ptr<Type> get_field_type(std::string name) const
	{
		auto it = std::find_if(fields.begin(), fields.end(), 
			[name](const StructField &f) { return f.name == name; });
		return (it == fields.end()) ? nullptr : it->type;
	}

private:
	std::vector<StructField> fields;
};
