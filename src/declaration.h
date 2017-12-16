#pragma once
#include <vector>
#include <memory>
#include <string>
#include <utility>

template <typename T>
using SP = std::shared_ptr<T>;

template <typename T>
using VEC = std::vector<T>;

template <typename T>
using PVP = SP<VEC<SP<T>>>;


#include "visitor.h"

#define DEFINE_ACCEPT void accept(Visitor &visitor) override { visitor.visit(*this); }

struct ASTNode
{
	int row;

	virtual void accept(Visitor &visitor) = 0;
	virtual ~ASTNode() = default;
};

struct ExtDef : public ASTNode
{

};

struct Specifier : public ExtDef
{

};

struct Program : public ASTNode
{
	PVP<ExtDef> ext_def_list;

	Program(PVP<ExtDef> p) : ext_def_list(p) {}
	DEFINE_ACCEPT
};

struct Expression;
struct VarDec : public ASTNode
{
	std::string id;
	VEC<int> indices;

	VarDec(std::string s) : id(s) {}
	DEFINE_ACCEPT
};

struct Dec : public ASTNode
{
	VarDec var_dec;
	SP<Expression> initial;

	Dec(VarDec &v, SP<Expression> i = nullptr) : var_dec(v), initial(i) {}
	DEFINE_ACCEPT
};

struct Def : public ASTNode
{
	SP<Specifier> specifier;
	VEC<Dec> dec_list;

	Def(SP<Specifier> s, VEC<Dec> &d) : specifier(s), dec_list(d) {}
	DEFINE_ACCEPT
};

struct GlobalVar : public ExtDef
{
	SP<Specifier> specifier;
	PVP<VarDec> var_dec_list;

	GlobalVar(SP<Specifier> s, PVP<VarDec> v) : specifier(s), var_dec_list(v) {}
	DEFINE_ACCEPT
};

typedef std::pair<SP<Specifier>, SP<VarDec>> Param;

struct CompSt;
struct FunDec : public ExtDef
{
	std::string name;
	SP<VEC<Param>>  params;
	SP<Specifier> ret_type;
	SP<CompSt> body;

	FunDec(std::string n, SP<VEC<Param>> p = nullptr) :
		name(n), params(p) {}
	DEFINE_ACCEPT
};

struct BasicType : public Specifier
{
	std::string type;

	BasicType(std::string t) : type(t) {}
	DEFINE_ACCEPT
};

struct StructSpecifier : public Specifier
{
	std::string name;
	VEC<Def> def_list;

	StructSpecifier(std::string n, const VEC<Def> &d = std::vector<Def>()) : name(n), def_list(d) {}
	DEFINE_ACCEPT
};
