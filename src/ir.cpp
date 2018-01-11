#include "ir.h"

#include <cassert>
#include <map>

namespace ir
{
	typedef std::list<std::shared_ptr<mips32_asm::Assembly>> Asm;
	using namespace std;
	using namespace mips32_asm;

	shared_ptr<Instruction> load_operand(shared_ptr<Operand> operand, Register reg)
	{
		if (auto pconst = dynamic_pointer_cast<Constant>(operand))
		{
			// li reg, operand
			return make_shared<I2RInstruction>(Instruction::LI, reg, pconst->value);	
		}
		else if (auto pvar = dynamic_pointer_cast<Variable>(operand))
		{
			// lw reg, operand
			return make_shared<MemoryInstr>(Instruction::LW, reg, Register::sp, 0);	
		}
		assert(0);	
	}

	Asm Label::emit_machine_code() const
	{
		Asm ret;
		ret.push_back(make_shared<mips32_asm::Label>(
					"L" + to_string(lbl)));
		return ret;
	}
	
	Asm Function::emit_machine_code() const
	{
		return std::list<std::shared_ptr<mips32_asm::Assembly>>();
	}
	
	Asm Assign::emit_machine_code() const
	{
		Asm ret;
		ret.push_back(load_operand(rhs, Register::t1));
		// sw $t1, x
		ret.push_back(make_shared<MemoryInstr>(
				Instruction::SW, Register::t1, Register::sp, 0));	
		return ret;
	}
	
	Asm Plus::emit_machine_code() const
	{
		Asm ret;
		// $t1 <- lhs
		ret.push_back(load_operand(lhs, Register::t1));

		if (auto pconst = dynamic_pointer_cast<Constant>(rhs))
		{
			// addiu $t3, $t1, rhs
			ret.push_back(make_shared<IInstruction>(Instruction::ADDIU,
					Register::t3, Register::t1, pconst->value));
		}
		else if (auto pvar = dynamic_pointer_cast<Variable>(rhs))
		{
			// lw $t2, rhs
			ret.push_back(make_shared<MemoryInstr>(
					Instruction::LW, Register::t2, Register::sp, 0));	
			// addu $t3, $t1, $t2
			ret.push_back(make_shared<RInstruction>(Instruction::ADDU,
					Register::t3, Register::t1, Register::t2));
		}

		// sw $t3, result
		ret.push_back(make_shared<MemoryInstr>(
				Instruction::SW, Register::t3, Register::sp, 0));	

		return ret;
	}
	
	Asm Minus::emit_machine_code() const
	{
		// very similar to Plus
		
		Asm ret;
		// $t1 <- lhs
		ret.push_back(load_operand(lhs, Register::t1));

		if (auto pconst = dynamic_pointer_cast<Constant>(rhs))
		{
			// subiu $t3, $t1, rhs
			ret.push_back(make_shared<IInstruction>(Instruction::SUBIU,
					Register::t3, Register::t1, pconst->value));
		}
		else if (auto pvar = dynamic_pointer_cast<Variable>(rhs))
		{
			// lw $t2, rhs
			ret.push_back(make_shared<MemoryInstr>(
					Instruction::LW, Register::t2, Register::sp, 0));	
			// subu $t3, $t1, $t2
			ret.push_back(make_shared<RInstruction>(Instruction::SUBU,
					Register::t3, Register::t1, Register::t2));
		}

		// sw $t3, result
		ret.push_back(make_shared<MemoryInstr>(
				Instruction::SW, Register::t3, Register::sp, 0));	

		return ret;
	}
	
	Asm Multiply::emit_machine_code() const
	{
		Asm ret;
		// $t1 <- lhs
		ret.push_back(load_operand(lhs, Register::t1));
		// $t2 <- rhs
		ret.push_back(load_operand(rhs, Register::t2));
		// mul $t3, $t1, $t2
		ret.push_back(make_shared<RInstruction>(Instruction::MUL,
				Register::t3, Register::t1, Register::t2));
		// sw $t3, result
		ret.push_back(make_shared<MemoryInstr>(
				Instruction::SW, Register::t3, Register::sp, 0));	
		return ret;
	}
	
	Asm Divide::emit_machine_code() const
	{
		Asm ret;
		// $t1 <- lhs
		ret.push_back(load_operand(lhs, Register::t1));
		// $t2 <- rhs
		ret.push_back(load_operand(rhs, Register::t2));
		// div $t1, $t2
		ret.push_back(make_shared<DoubleRegInstr>(Instruction::DIV,
				Register::t1, Register::t2));
		// mflo $t3
		ret.push_back(make_shared<SingleRegInstr>(Instruction::MFLO,
				Register::t3));
		// sw $t3, result
		ret.push_back(make_shared<MemoryInstr>(
				Instruction::SW, Register::t3, Register::sp, 0));	
		return ret;
	}
	
	Asm Goto::emit_machine_code() const
	{
		return Asm(1, make_shared<JInstruction>(Instruction::B, "L" + to_string(lbl)));
	}
	
	Asm CGoto::emit_machine_code() const
	{
		Asm ret;
		ret.push_back(load_operand(lhs, Register::t1));
		ret.push_back(load_operand(rhs, Register::t2));

		map<string, Instruction::OP> op_map = 
		{
			{"==", Instruction::BEQ}, {"!=", Instruction::BNE},
			{">", Instruction::BGT}, {"<", Instruction::BLT},
			{">=", Instruction::BGE}, {"<=", Instruction::BLE}
		};

		ret.push_back(make_shared<BranchInstr>(
					op_map[relop],
					Register::t1, Register::t2,
					"L" + to_string(lbl)));
		return ret;
	}
	
	Asm Return::emit_machine_code() const
	{
		return std::list<std::shared_ptr<mips32_asm::Assembly>>();
	}
	
	Asm Read::emit_machine_code() const
	{
		return std::list<std::shared_ptr<mips32_asm::Assembly>>();
	}
	
	Asm Write::emit_machine_code() const
	{
		return std::list<std::shared_ptr<mips32_asm::Assembly>>();
	}
	
	Asm Param::emit_machine_code() const
	{
		return std::list<std::shared_ptr<mips32_asm::Assembly>>();
	}
	
	Asm Arg::emit_machine_code() const
	{
		return std::list<std::shared_ptr<mips32_asm::Assembly>>();
	}
	
	Asm FunCall::emit_machine_code() const
	{
		return std::list<std::shared_ptr<mips32_asm::Assembly>>();
	}
	
	Asm Declare::emit_machine_code() const
	{
		return Asm();
	}
}
