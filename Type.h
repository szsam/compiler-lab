#pragma once

#include <memory>
#include <vector>

struct Type
{
	enum TypeTag { BASIC, ARRAY, STRUCTURE, FUNCTION };
	TypeTag tag;

	Type(TypeTag t) : tag(t) {}
};

struct Basic : public Type
{
	enum BasicType { tINT, tFLOAT };
	BasicType basic_type;

	Basic(BasicType t) : Type(BASIC), basic_type(t) {}
};

struct Function : public Type
{
	std::shared_ptr<Type> ret_type;
	std::vector<std::shared_ptr<Type> > params;

	Function(std::shared_ptr<Type> t) : Type(FUNCTION), ret_type(t) {}
};
