#pragma once
#include <string>
#include <iostream>
#include <memory>
#include <list>

#include "assembly.h"

// Intermediate Representation
namespace ir
{

// Operand hierachy
struct Operand
{
	virtual std::ostream& output(std::ostream &out) const = 0;
	virtual ~Operand() = default;
};

inline
std::ostream &operator<<(std::ostream &os, const Operand &op) { return op.output(os); }

struct Variable : public Operand
{
	std::string str;
	enum Prefix { NIL, DEREF, ADDR };
	Prefix prefix = NIL;
	const char *prefix_ch[3] = {"", "*", "&"};

	Variable() = default;
	Variable(const std::string &s, Prefix p = NIL) : str(s), prefix(p) {}
	Variable(const Variable &rhs) = default;
	Variable(const Variable &rhs, Prefix p) : str(rhs.str), prefix(p) {}

	std::ostream& output(std::ostream &out) const { return out << prefix_ch[prefix] << str; }
	bool empty() const { return str.empty(); }
};

struct Constant : public Operand
{
	int value;
	Constant(int v) : value(v) {}
	std::ostream& output(std::ostream &out) const { return out << "#" << value; }
};

// Intermediate Code Hierachy

struct InterCode
{
	virtual std::ostream& output(std::ostream &out) const = 0;
	virtual std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const = 0;
	virtual ~InterCode() = default;
};

inline
std::ostream &operator<<(std::ostream &os, const InterCode &code) { return code.output(os); }

struct Label : public InterCode
{
	int lbl;

	Label(int l) : lbl(l) {}
	std::ostream& output(std::ostream &out) const override 
	{ return out << "LABEL label" << lbl << " :"; }
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Function : public InterCode
{
	std::string name;

	Function(const std::string &s) : name(s) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "FUNCTION " << name << " :";
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Assign : public InterCode
{
	Variable lhs;
	std::shared_ptr<Operand> rhs;

	Assign(const Variable &l, std::shared_ptr<Operand> r) : lhs(l), rhs(r) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << lhs << " := " << *rhs;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct BinaryOp : public InterCode
{
	Variable result;
	std::shared_ptr<Operand> lhs;
	std::shared_ptr<Operand> rhs;
	char op;

	BinaryOp(char _op, const Variable &res, 
			std::shared_ptr<Operand> l, std::shared_ptr<Operand> r) : 
		result(res), lhs(l), rhs(r), op(_op) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << result << " := " << *lhs << " " << op << " " << *rhs;
	}
};

struct Plus : public BinaryOp
{
	Plus(const Variable &res, std::shared_ptr<Operand> l, std::shared_ptr<Operand> r) : 
		BinaryOp('+', res, l, r) {}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Minus : public BinaryOp
{
	Minus(const Variable &res, std::shared_ptr<Operand> l, std::shared_ptr<Operand> r) : 
		BinaryOp('-', res, l, r) {}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Multiply : public BinaryOp
{
	Multiply(const Variable &res, std::shared_ptr<Operand> l, std::shared_ptr<Operand> r) : 
		BinaryOp('*', res, l, r) {}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Divide : public BinaryOp
{
	Divide(const Variable &res, std::shared_ptr<Operand> l, std::shared_ptr<Operand> r) : 
		BinaryOp('/', res, l, r) {}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Goto : public InterCode
{
	int lbl;

	Goto(int l) : lbl(l) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "GOTO label" << lbl;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

// Conditional jump
struct CGoto : public InterCode
{
	std::shared_ptr<Operand> lhs, rhs;
	std::string relop;
	int lbl;

	CGoto(std::shared_ptr<Operand> x, std::shared_ptr<Operand> y, const std::string &op, int l) :
		lhs(x), rhs(y), relop(op), lbl(l) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "IF " << *lhs << " " << relop << " " << *rhs 
			<< " GOTO label" << lbl;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Return : public InterCode
{
	std::shared_ptr<Operand> operand;

	Return(std::shared_ptr<Operand> x) : operand(x) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "RETURN " << *operand;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Read : public InterCode
{
	Variable operand;

	Read(const Variable &var) : operand(var) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "READ " << operand;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Write : public InterCode
{
	std::shared_ptr<Operand> operand;

	Write(std::shared_ptr<Operand> x) : operand(x) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "WRITE " << *operand;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Param : public InterCode
{
	Variable operand;

	Param(const Variable &var) : operand(var) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "PARAM " << operand;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Arg : public InterCode
{
	std::shared_ptr<Operand> operand;

	Arg(std::shared_ptr<Operand> x) : operand(x) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "ARG " << *operand;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct FunCall : public InterCode
{
	std::string fun_name;
	Variable result;

	FunCall(const std::string str, const Variable &var) 
		: fun_name(str), result(var) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << result << " := CALL " << fun_name;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

struct Declare : public InterCode
{
	Variable operand;
	int size;

	Declare(const Variable &var, int sz) 
		: operand(var), size(sz) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "DEC " << operand << " " << size;
	}
	std::list<std::shared_ptr<mips32_asm::Assembly>> 
		emit_machine_code() const override;
};

}	// namespace ir
