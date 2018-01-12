#include "code_generator.h"

#include <map>

namespace ir
{
	typedef std::list<std::shared_ptr<mips32_asm::Assembly>> Asm;
	using namespace std;
	using namespace mips32_asm;

	void CodeGenerationVisitor::output(std::ostream &os) const
	{
		for (const auto &fun_code : machine_code)
		{
			for (const auto &p_code : fun_code)
			{
				os << *p_code << std::endl;
			}
			os << std::endl;
		}
	}
	
	void CodeGenerationVisitor::generate_machine_code()
	{
		for (const auto &fun_inter_code : inter_code)
		{
			// generate machine code for a function
			var_info.clear();
			local_var_offset = 0;
			max_num_of_args = 0;
			is_leaf = true;
			
			std::list<std::shared_ptr<mips32_asm::Assembly>> fun_machine_code;
			for (const auto &p_code : fun_inter_code)
			{
				p_code->accept(*this);
				fun_machine_code.splice(fun_machine_code.end(),
						p_code->assembly);
			}

			for (const auto &var : var_info)
			{
				cout << var.first << " " << var.second.offset << "\n";
			}

			int space_for_args = is_leaf ? 0 : max(16, 4 * max_num_of_args);
			int frame_size = space_for_args + local_var_offset + 8;

			for (auto &var : var_info)
			{
				var.second.back_patch(frame_size, space_for_args);
			}

			machine_code.push_back(fun_machine_code);
		}
	
	}

	shared_ptr<Instruction> CodeGenerationVisitor::load_operand(
			shared_ptr<Operand> operand, Register reg)
	{
		if (auto pconst = dynamic_pointer_cast<Constant>(operand))
		{
			// li reg, operand
			return make_shared<I2RInstruction>(Instruction::LI, reg, pconst->value);	
		}
		else if (auto pvar = dynamic_pointer_cast<Variable>(operand))
		{
			// lw reg, operand
			return access_variable(*pvar, Instruction::LW, reg); 
		}
		assert(0);	
	}

	// access parameter, local variable, or temporary
	// OP should be LW or SW
	std::shared_ptr<mips32_asm::MemoryInstr>
	CodeGenerationVisitor::access_variable(
					const Variable &var, 
					mips32_asm::Instruction::OP op, 
					mips32_asm::Register reg)
	{
		auto instr = make_shared<MemoryInstr>(op, reg, Register::fp, 0);
		auto ret = var_info.insert({var, VarInfo(VarInfo::LOCAL, local_var_offset)});

		// new variable
		if (ret.second) local_var_offset += 4;

		ret.first->second.add_instr(instr);

		return instr;
	}

	void CodeGenerationVisitor::VarInfo::back_patch(int frame_size, int args_size)
	{
		int base = (type == PARAM ? frame_size : args_size);
		for (auto &pinstr : instructions)
		{
			pinstr->offset = base + offset;
		}
	}

	void CodeGenerationVisitor::visit(Label &code)
	{
		code.assembly.push_back(make_shared<mips32_asm::Label>(
					"L" + to_string(code.lbl)));
	}
	
	void CodeGenerationVisitor::visit(Function &code)
	{
		code.assembly.push_back(make_shared<mips32_asm::Label>(code.name));
		int param_offset = 0;
		for (const auto &param : code.params)
		{
			// var_info[param] = VarInfo(VarInfo::PARAM, param_offset);
			var_info.insert({param, VarInfo(VarInfo::PARAM, param_offset)});
			param_offset += 4;
		}
	}
	
	void CodeGenerationVisitor::visit(Assign &code)
	{
		// x := y
		// load y into $t1
		code.assembly.push_back(load_operand(code.rhs, Register::t1));
		// sw $t1, x
		code.assembly.push_back(access_variable(code.lhs, Instruction::SW, Register::t1));	
	}
	
	void CodeGenerationVisitor::visit(Plus &code)
	{
		// $t1 <- lhs
		code.assembly.push_back(load_operand(code.lhs, Register::t1));

		if (auto pconst = dynamic_pointer_cast<Constant>(code.rhs))
		{
			// addiu $t3, $t1, rhs
			code.assembly.push_back(make_shared<IInstruction>(Instruction::ADDIU,
					Register::t3, Register::t1, pconst->value));
		}
		else if (auto pvar = dynamic_pointer_cast<Variable>(code.rhs))
		{
			// lw $t2, rhs
			code.assembly.push_back(access_variable(
					*pvar, Instruction::LW, Register::t2));	
			// addu $t3, $t1, $t2
			code.assembly.push_back(make_shared<RInstruction>(Instruction::ADDU,
					Register::t3, Register::t1, Register::t2));
		}

		// sw $t3, result
		code.assembly.push_back(access_variable(
				code.result, Instruction::SW, Register::t3));	
	}
	
	void CodeGenerationVisitor::visit(Minus &code)
	{
		// very similar to Plus
	}
	
	void CodeGenerationVisitor::visit(Multiply &code)
	{
		// $t1 <- lhs
		code.assembly.push_back(load_operand(code.lhs, Register::t1));
		// $t2 <- rhs
		code.assembly.push_back(load_operand(code.rhs, Register::t2));
		// mul $t3, $t1, $t2
		code.assembly.push_back(make_shared<RInstruction>(Instruction::MUL,
				Register::t3, Register::t1, Register::t2));
		// sw $t3, result
		code.assembly.push_back(access_variable(
				code.result, Instruction::SW, Register::t3));	
	}
	
	void CodeGenerationVisitor::visit(Divide &code)
	{
		// $t1 <- lhs
		code.assembly.push_back(load_operand(code.lhs, Register::t1));
		// $t2 <- rhs
		code.assembly.push_back(load_operand(code.rhs, Register::t2));
		// div $t1, $t2
		code.assembly.push_back(make_shared<DoubleRegInstr>(Instruction::DIV,
				Register::t1, Register::t2));
		// mflo $t3
		code.assembly.push_back(make_shared<SingleRegInstr>(Instruction::MFLO,
				Register::t3));
		// sw $t3, result
		code.assembly.push_back(access_variable(
				code.result, Instruction::SW, Register::t3));	
	}
	
	void CodeGenerationVisitor::visit(Goto &code)
	{
		code.assembly.push_back(make_shared<JInstruction>(
					Instruction::B, "L" + to_string(code.lbl)));
	}
	
	void CodeGenerationVisitor::visit(CGoto &code)
	{
		code.assembly.push_back(load_operand(code.lhs, Register::t1));
		code.assembly.push_back(load_operand(code.rhs, Register::t2));

		const map<string, Instruction::OP> op_map = 
		{
			{"==", Instruction::BEQ}, {"!=", Instruction::BNE},
			{">", Instruction::BGT}, {"<", Instruction::BLT},
			{">=", Instruction::BGE}, {"<=", Instruction::BLE}
		};

		code.assembly.push_back(make_shared<BranchInstr>(
					op_map.at(code.relop),
					Register::t1, Register::t2,
					"L" + to_string(code.lbl)));
	}
	
	void CodeGenerationVisitor::visit(Return &code)
	{
	}
	
	void CodeGenerationVisitor::visit(Read &code)
	{
	}
	
	void CodeGenerationVisitor::visit(Write &code)
	{
	}
	
	void CodeGenerationVisitor::visit(Param &code)
	{
	}
	
	void CodeGenerationVisitor::visit(Arg &code)
	{
	}
	
	void CodeGenerationVisitor::visit(FunCall &code)
	{
	}
	
	void CodeGenerationVisitor::visit(Declare &code)
	{
	}
}
