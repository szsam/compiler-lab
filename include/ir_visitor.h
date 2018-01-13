#pragma once

namespace ir
{
	struct Label; struct Function; struct Assign; 
	struct Plus; struct Minus; struct Multiply; struct Divide;
	struct Goto; struct CGoto; struct Return; 
	struct Read; struct Write; 
	struct Param; struct Arg; struct FunCall;
	struct Declare;

	
	// visitor of intermediate representation
	struct IRVisitor
	{
		virtual void visit(Label &) = 0;
		virtual void visit(Function &) = 0;

		virtual void visit(Assign &) = 0;

		virtual void visit(Plus &) = 0;
		virtual void visit(Minus &) = 0;
		virtual void visit(Multiply &) = 0;
		virtual void visit(Divide &) = 0;

		virtual void visit(Goto &) = 0;
		virtual void visit(CGoto &) = 0;
		virtual void visit(Return &) = 0;

		virtual void visit(Read &) = 0;
		virtual void visit(Write &) = 0;

		virtual void visit(Param &) = 0;
		virtual void visit(Arg &) = 0;
		virtual void visit(FunCall &) = 0;

		virtual void visit(Declare &) = 0;
	};
}
