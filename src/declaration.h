#pragma once
#include <vector>
#include <list>
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
#include "Type.h"
#include "ir.h"
#include "symbol_table.h"

#undef DEFINE_ACCEPT
#define DEFINE_ACCEPT void accept(Visitor &visitor) override { visitor.visit(*this); }

struct ASTNode
{
	int row;
	std::list<std::shared_ptr<ir::InterCode>> code;

	virtual void accept(Visitor &visitor) = 0;
	virtual ~ASTNode() = default;
};

struct ExtDef : public ASTNode
{

};

struct Specifier : public ExtDef
{
	SP<Type> type;
};

struct Program : public ASTNode
{
	VEC<SP<ExtDef>> ext_def_list;

	Program(const VEC<SP<ExtDef>> &v) : ext_def_list(v) {}
	DEFINE_ACCEPT
};

struct Expression;
struct VarDec
{
	std::string id;
	VEC<int> indices;
	SymbolInfo sym_info;

	VarDec(const std::string &s) : id(s) {}
};

struct Dec
{
	VarDec var_dec;
	SP<Expression> initial;

	Dec(VarDec &v, SP<Expression> i = nullptr) : var_dec(v), initial(i) {}
};

struct Def
{
	SP<Specifier> specifier;
	VEC<Dec> dec_list;

	Def(SP<Specifier> s, VEC<Dec> &d) : specifier(s), dec_list(d) {}
};

struct GlobalVar : public ExtDef
{
	SP<Specifier> specifier;
	VEC<VarDec> var_dec_list;

	GlobalVar(SP<Specifier> s, const VEC<VarDec> &v) : 
		specifier(s), var_dec_list(v) {}
	DEFINE_ACCEPT
};

typedef std::pair<SP<Specifier>, SP<VarDec>> Param;

struct CompSt;
struct FunDec : public ExtDef
{
	std::string name;
	VEC<Param>  params;
	SP<Specifier> ret_type;
	SP<CompSt> body;
	// VEC<ir::Variable> ir_params;  // parameters' name in IR

	FunDec(const std::string &n, const VEC<Param> &p = VEC<Param>()) :
		name(n), params(p) {}
	DEFINE_ACCEPT
};

struct BasicType : public Specifier
{
	std::string str;

	BasicType(const std::string &s) : str(s) {}
	DEFINE_ACCEPT
};

struct StructSpecifier : public Specifier
{
	std::string name;
	VEC<Def> def_list;

	StructSpecifier(const std::string &n, const VEC<Def> &d = std::vector<Def>()) 
		: name(n), def_list(d) {}
	DEFINE_ACCEPT
};
