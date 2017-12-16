#pragma once

struct Program; 
struct VarDec; struct Dec; struct Def; struct GlobalVar;
struct FunDec; struct BasicType; struct StructSpecifier;
struct CompSt; struct Return; struct Branch; struct While;
struct Plus; struct Minus; struct Multiply; struct Divide; struct Negative;
struct And; struct Or; struct Not;
struct Relop; struct Subscript; struct MemberAccess; struct Assign;
struct FunCall; struct Identifier; struct Integer; struct Float;

struct Visitor
{
	// virtual void visit(ASTNode &node) = 0;

	virtual void visit(Program &node) = 0;

	virtual void visit(VarDec &node) = 0;
	virtual void visit(Dec &node) = 0;
	virtual void visit(Def &node) = 0;
	virtual void visit(GlobalVar &node) = 0;
	virtual void visit(FunDec &node) = 0;
	virtual void visit(BasicType &node) = 0;
	virtual void visit(StructSpecifier &node) = 0;

	virtual void visit(CompSt &node) = 0;
	virtual void visit(Return &node) = 0;
	virtual void visit(Branch &node) = 0;
	virtual void visit(While &node) = 0;

	virtual void visit(Plus &node) = 0;
	virtual void visit(Minus &node) = 0;
	virtual void visit(Multiply &node) = 0;
	virtual void visit(Divide &node) = 0;
	virtual void visit(Negative &node) = 0;
	virtual void visit(And &node) = 0;
	virtual void visit(Or &node) = 0;
	virtual void visit(Not &node) = 0;
	virtual void visit(Relop &node) = 0;
	virtual void visit(Subscript &node) = 0;
	virtual void visit(MemberAccess &node) = 0;
	virtual void visit(Assign &node) = 0;
	virtual void visit(FunCall &node) = 0;
	virtual void visit(Identifier &node) = 0;
	virtual void visit(Integer &node) = 0;
	virtual void visit(Float &node) = 0;

};