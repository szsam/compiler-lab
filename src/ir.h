#pragma once
#include <string>
#include <iostream>
#include <memory>

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

	Variable() = default;
	Variable(const std::string &s) : str(s) {}
	std::ostream& output(std::ostream &out) const { return out << str; }
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
};

struct Function : public InterCode
{
	std::string name;

	Function(const std::string &s) : name(s) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "FUNCTION " << name << " :";
	}
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
};

struct Minus : public BinaryOp
{
	Minus(const Variable &res, std::shared_ptr<Operand> l, std::shared_ptr<Operand> r) : 
		BinaryOp('-', res, l, r) {}
};

struct Multiply : public BinaryOp
{
	Multiply(const Variable &res, std::shared_ptr<Operand> l, std::shared_ptr<Operand> r) : 
		BinaryOp('*', res, l, r) {}
};

struct Divide : public BinaryOp
{
	Divide(const Variable &res, std::shared_ptr<Operand> l, std::shared_ptr<Operand> r) : 
		BinaryOp('/', res, l, r) {}
};

struct Goto : public InterCode
{
	int lbl;

	Goto(int l) : lbl(l) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "GOTO label" << lbl;
	}
};

// Conditional jump
struct CGoto : public InterCode
{
	std::shared_ptr<Operand> lhs, rhs;
	std::string relop;
	int lbl;

	CGoto(std::shared_ptr<Operand> x, const std::string &op, std::shared_ptr<Operand> y, int l) :
		lhs(x), rhs(y), relop(op), lbl(l) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "IF " << *lhs << " " << relop << " " << *rhs 
			<< " GOTO label" << lbl;
	}
};

struct Return : public InterCode
{
	std::shared_ptr<Operand> operand;

	Return(std::shared_ptr<Operand> x) : operand(x) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "RETURN " << *operand;
	}
};

struct Read : public InterCode
{
	Variable operand;

	Read(const Variable &var) : operand(var) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "READ " << operand;
	}
};

struct Write : public InterCode
{
	std::shared_ptr<Operand> operand;

	Write(std::shared_ptr<Operand> x) : operand(x) {}
	std::ostream& output(std::ostream &out) const override
	{
		return out << "WRITE " << *operand;
	}
};

}	// namespace ir
