#include "assembly.h"
#include "ir_visitor.h"
#include "ir_gen_visitor.h"

#include <map>

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
		typedef std::list<std::shared_ptr<mips32_asm::Assembly>> AsmList;
		std::list<AsmList> machine_code;

		struct VarInfo
		{
		public:
			enum Type { PARAM, LOCAL };
			VarInfo(Type t, int off) : type(t), offset(off) {}
			void add_instr(std::shared_ptr<mips32_asm::MemoryInstr> instr)
			{ instructions.push_back(instr); } 
			void back_patch(int frame_size, int args_size);

		// private:
		public:
			Type type;
			// offset to the first parameter, or first local variable
			int offset;
			// instructions that use this variable
			std::vector<std::shared_ptr<mips32_asm::MemoryInstr>> instructions;

		};
		std::map<Variable, VarInfo> var_info;
		// current offset of local variable (including temporaries)
		// i.e. offset to the first local variable (in bytes)
		int local_var_offset;
		//  maximum number of arguments of called functions 
		int max_num_of_args;
		// a function is leaf if it does not itself call any function 
		bool is_leaf;
		std::string epilogue_label;

		// load operand(variable/constant) into register
		std::shared_ptr<mips32_asm::Instruction>
		   	load_operand(std::shared_ptr<Operand> operand, mips32_asm::Register reg);

		// access parameter, local variable, or temporary
		std::shared_ptr<mips32_asm::MemoryInstr>
		access_variable(const Variable &, mips32_asm::Instruction::OP, mips32_asm::Register);

		AsmList prologue(int frame_size);

		AsmList epilogue(int frame_size);
	};
}
