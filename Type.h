#pragma once

#include <memory>

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
