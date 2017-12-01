#pragma once

#include <memory>
#include <vector>

struct Type
{
	enum TypeTag { BASIC, ARRAY, STRUCTURE, FUNCTION };
	TypeTag tag;

	Type(TypeTag t) : tag(t) {}
	virtual bool equal(const Type &rhs) const
	{
		return tag == rhs.tag;
	}
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
	virtual bool equal(const Type &rhs) const override
	{
		auto r = dynamic_cast<const Basic&>(rhs);
		return Type::equal(rhs) && basic_type == r.basic_type;
	}
};

struct Function : public Type
{
	std::shared_ptr<Type> ret_type;
	std::vector<std::shared_ptr<Type> > params;

	Function(std::shared_ptr<Type> t) : Type(FUNCTION), ret_type(t) {}
	virtual bool equal(const Type &rhs) const override
	{
		auto r = dynamic_cast<const Function&>(rhs);
		return Type::equal(rhs) && *ret_type == *r.ret_type;
		// TODO compare params
	}
};

