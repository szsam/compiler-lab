#include "assembly.h"
#include "ir_visitor.h"
#include "ir_gen_visitor.h"


namespace ir
{
	class CodeGenerationVisitor : public IRVisitor
	{
	public:
		CodeGenerationVisitor(
			const std::list<std::list<std::shared_ptr<InterCode>>> 
			&_inter_code) : inter_code(_inter_code) {}
		void generate_machine_code();
		void output(std::ostream &) const;
	
		void visit(Label &);
		void visit(Function &);

		void visit(Assign &);

		void visit(Plus &);
		void visit(Minus &);
		void visit(Multiply &);
		void visit(Divide &);

		void visit(Goto &);
		void visit(CGoto &);
		void visit(Return &);

		void visit(Read &);
		void visit(Write &);

		void visit(Param &);
		void visit(Arg &);
		void visit(FunCall &);

		void visit(Declare &);
	private:
		// intermediate-code
		std::list<std::list<std::shared_ptr<InterCode>>> inter_code;
		// machine-code
		std::list<std::list<std::shared_ptr<mips32_asm::Assembly>>> machine_code;
	};
}
