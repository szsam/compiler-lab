#include "code_generator.h"

#include <map>

namespace ir
{
	using namespace std;
	using namespace mips32_asm;

	void CodeGenerationVisitor::output(std::ostream &os) const
	{
		os << startup_code << std::endl;

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
			// initialization
			var_info.clear();
			local_var_offset = 0;
			max_num_of_args = 0;
			is_leaf = true;
			
			// the first inter-code 
			auto pfun = dynamic_pointer_cast<ir::Function>(fun_inter_code.front());
			assert(pfun);

			// save information of parameters in var_info
			int param_offset = 0;
			for (const auto &param : pfun->params)
			{
				// var_info[param] = VarInfo(VarInfo::PARAM, param_offset);
				var_info.insert({param, VarInfo(VarInfo::PARAM, param_offset)});
				param_offset += 4;
			}

			epilogue_label = pfun->name + "_epilogue";

			// generate code for function body
			std::list<std::shared_ptr<mips32_asm::Assembly>> fun_machine_code;
			for (const auto &p_code : fun_inter_code)
			{
				p_code->accept(*this);
				fun_machine_code.splice(fun_machine_code.end(),
						p_code->assembly);
			}

//			for (const auto &var : var_info)
//			{
//				cout << var.first << " " << var.second.offset << "\n";
//			}

			// calculate frame size
			int space_for_args = is_leaf ? 0 : max(16u, 4 * max_num_of_args);
			int frame_size = space_for_args + local_var_offset + 8;

			// fill in offset of local variables in load/store instructions
			for (auto &var : var_info)
			{
				var.second.back_patch(frame_size, space_for_args);
			}
			// label of function
			auto temp_list = AsmList(1, make_shared<mips32_asm::Label>(pfun->name));
			// prologue	
			temp_list.splice(temp_list.end(), prologue(frame_size));
			// store first four arguments (if any) in caller's stack
			for (vector<Variable>::size_type i = 0;
					i < pfun->params.size() && i < 4; ++i)
			{
				Register::Reg reg;
				switch (i)
				{
					case 0: reg = Register::a0; break;
					case 1: reg = Register::a1; break;
					case 2: reg = Register::a2; break;
					case 3: reg = Register::a3; break;
				}
				temp_list.push_back(make_shared<MemoryInstr>(
					Instruction::SW, reg, Register::fp, 
					frame_size + 4 * i));	
			}	
			// insert temp_list before fun_machine_code
			fun_machine_code.splice(fun_machine_code.begin(), temp_list);
			// epilogue
			fun_machine_code.splice(fun_machine_code.end(), epilogue(frame_size));

			// save the generated machine code 
			machine_code.push_back(fun_machine_code);
		}
	
	}

	CodeGenerationVisitor::AsmList CodeGenerationVisitor::load_operand(
			const Variable &var, Register reg)
	{
		// lw reg, operand
		switch (var.prefix)
		{
			case Variable::NIL:
				// reg <- var
				return { access_variable(var, Instruction::LW, reg) };
			case Variable::DEREF:
				// reg <- var
				// lw reg, 0(reg)
				return { access_variable(var, Instruction::LW, reg),
						 make_shared<MemoryInstr>(Instruction::LW, reg, reg, 0) };
			case Variable::ADDR:
				return { access_variable(var, Instruction::ADDIU, reg) };
		}
	}

	CodeGenerationVisitor::AsmList CodeGenerationVisitor::load_operand(
			shared_ptr<Operand> operand, Register reg)
	{
		if (auto pconst = dynamic_pointer_cast<Constant>(operand))
		{
			// li reg, operand
			return { make_shared<I2RInstruction>(Instruction::LI, reg, pconst->value)};
		}
		else if (auto pvar = dynamic_pointer_cast<Variable>(operand))
		{
			return load_operand(*pvar, reg);
		}
		assert(0);	
	}

	// access parameter, local variable, or temporary
	// OP should be LW or SW or ADDIU
	std::shared_ptr<mips32_asm::Instruction>
	CodeGenerationVisitor::access_variable(
					const Variable &var, 
					mips32_asm::Instruction::OP op, 
					mips32_asm::Register reg)
	{
		shared_ptr<Instruction> instr;

		if (op == Instruction::LW || op == Instruction::SW)
			instr = make_shared<MemoryInstr>(op, reg, Register::fp, 0);
		else // addiu
			instr = make_shared<IInstruction>(op, reg, Register::fp, 0);

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
			if (auto p_minstr = dynamic_pointer_cast<MemoryInstr>(pinstr))
				p_minstr->offset = base + offset;
			else if (auto p_iinstr = dynamic_pointer_cast<IInstruction>(pinstr))
				p_iinstr->imm = base + offset;
		}
	}

	CodeGenerationVisitor::AsmList CodeGenerationVisitor::prologue(int frame_size)
	{
		AsmList asm_list;
		// addiu $sp, $sp, -frame_size
		asm_list.push_back(make_shared<IInstruction>(
				Instruction::ADDIU, Register::sp, Register::sp, -frame_size));

		if (!is_leaf) 
		{
			// sw $ra, frame_size-4($sp)
			asm_list.push_back(make_shared<MemoryInstr>(
					Instruction::SW, Register::ra, Register::sp, frame_size - 4));
		}

		// sw $fp, frame_size-8($sp)
		asm_list.push_back(make_shared<MemoryInstr>(
				Instruction::SW, Register::fp, Register::sp, frame_size - 8));
		// move $fp, $sp
		asm_list.push_back(make_shared<DoubleRegInstr>(
					Instruction::MOVE, Register::fp, Register::sp));

		return asm_list;
	}

	CodeGenerationVisitor::AsmList CodeGenerationVisitor::epilogue(int frame_size)
	{
		AsmList asm_list;

		asm_list.push_back(make_shared<mips32_asm::Label>(epilogue_label));	
		// move $sp, $fp
		asm_list.push_back(make_shared<DoubleRegInstr>(
					Instruction::MOVE, Register::sp, Register::fp));

		if (!is_leaf) 
		{	// $lw $sp, frame_size-4($sp)
			asm_list.push_back(make_shared<MemoryInstr>(
					Instruction::LW, Register::ra, Register::sp, frame_size - 4));
		}

		// lw $fp, frame_size-8($sp)
		asm_list.push_back(make_shared<MemoryInstr>(
				Instruction::LW, Register::fp, Register::sp, frame_size - 8));
		// addiu $sp, $sp, frame_size
		asm_list.push_back(make_shared<IInstruction>(
				Instruction::ADDIU, Register::sp, Register::sp, frame_size));
		// jr $ra
		asm_list.push_back(make_shared<SingleRegInstr>(Instruction::JR, Register::ra));

		return asm_list;
	}

	void CodeGenerationVisitor::visit(Label &code)
	{
		code.assembly.push_back(make_shared<mips32_asm::Label>(
					"L" + to_string(code.lbl)));
	}
	
	void CodeGenerationVisitor::visit(Function &code)
	{
	//	code.assembly.push_back(make_shared<mips32_asm::Label>(code.name));
	//	int param_offset = 0;
	//	for (const auto &param : code.params)
	//	{
	//		// var_info[param] = VarInfo(VarInfo::PARAM, param_offset);
	//		var_info.insert({param, VarInfo(VarInfo::PARAM, param_offset)});
	//		param_offset += 4;
	//		// store first four arguments (if any) in caller's stack
	//	}
	//	epilogue_label = code.name + "_epilogue";
	}
	
	void CodeGenerationVisitor::visit(Assign &code)
	{
		// load y into $t1
		code.assembly.splice(code.assembly.end(), load_operand(code.rhs, Register::t1));

		if (code.lhs.prefix == Variable::NIL)
		{
			// store $t1 into x
			code.assembly.push_back(
					access_variable(code.lhs, Instruction::SW, Register::t1));	
		}
		else if (code.lhs.prefix == Variable::DEREF)
		{
			// load the value of x into $t2
			code.lhs.prefix = Variable::NIL;
			code.assembly.splice(code.assembly.end(), load_operand(code.lhs, Register::t2));
			// sw $t1, 0($t2)
			code.assembly.push_back(make_shared<MemoryInstr>(
						Instruction::SW, Register::t1, Register::t2, 0));
		}
		else assert(0);
	}
	
	// generate code for Plus and Minus
	void CodeGenerationVisitor::visit_bop(BinaryOp &code,
			Instruction::OP rinstr, Instruction::OP iinstr)
	{
		// $t1 <- lhs
		code.assembly.splice(code.assembly.end(), 
				load_operand(code.lhs, Register::t1));

		if (auto pconst = dynamic_pointer_cast<Constant>(code.rhs))
		{
			// addiu/subiu $t3, $t1, rhs
			code.assembly.push_back(make_shared<IInstruction>(iinstr,
					Register::t3, Register::t1, pconst->value));
		}
		else if (auto pvar = dynamic_pointer_cast<Variable>(code.rhs))
		{
			// lw $t2, rhs
			code.assembly.push_back(access_variable(
					*pvar, Instruction::LW, Register::t2));	
			// addu/subu $t3, $t1, $t2
			code.assembly.push_back(make_shared<RInstruction>(rinstr,
					Register::t3, Register::t1, Register::t2));
		}

		// sw $t3, result
		code.assembly.push_back(access_variable(
				code.result, Instruction::SW, Register::t3));	
	}
	
	void CodeGenerationVisitor::visit(Plus &code)
	{
		visit_bop(code, Instruction::ADDU, Instruction::ADDIU);
	}

	void CodeGenerationVisitor::visit(Minus &code)
	{
		visit_bop(code, Instruction::SUBU, Instruction::SUBIU);
	}
	
	void CodeGenerationVisitor::visit(Multiply &code)
	{
		// $t1 <- lhs
		code.assembly.splice(code.assembly.end(), 
			load_operand(code.lhs, Register::t1));
		// $t2 <- rhs
		code.assembly.splice(code.assembly.end(), 
			load_operand(code.rhs, Register::t2));
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
		code.assembly.splice(code.assembly.end(), 
			load_operand(code.lhs, Register::t1));
		// $t2 <- rhs
		code.assembly.splice(code.assembly.end(), 
			load_operand(code.rhs, Register::t2));
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
		code.assembly.splice(code.assembly.end(), 
			load_operand(code.lhs, Register::t1));
		code.assembly.splice(code.assembly.end(), 
			load_operand(code.rhs, Register::t2));

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
		code.assembly.splice(code.assembly.end(), 
			load_operand(code.operand, Register::v0));
		code.assembly.push_back(make_shared<JInstruction>(Instruction::B, epilogue_label));
	}
	
	void CodeGenerationVisitor::visit(Read &code)
	{
		is_leaf = false;
		// jal read
		code.assembly.push_back(make_shared<JInstruction>(
					Instruction::JAL, "read"));
		// operand <- v0
		code.assembly.push_back(
				access_variable(code.operand, Instruction::SW, Register::v0));
	}
	
	void CodeGenerationVisitor::visit(Write &code)
	{
		is_leaf = false;
		max_num_of_args = max(max_num_of_args, 1u);
		// a0 <- operand
		code.assembly.splice(code.assembly.end(), 
			load_operand(code.operand, Register::a0));
		// jal write
		code.assembly.push_back(make_shared<JInstruction>(
					Instruction::JAL, "write"));
	}
	
	void CodeGenerationVisitor::visit(Param &code)
	{
	}
	
	void CodeGenerationVisitor::visit(Arg &code)
	{
	}
	
	void CodeGenerationVisitor::visit(FunCall &code)
	{
		is_leaf = false;
		max_num_of_args = max(max_num_of_args, code.args.size());

		for (vector<shared_ptr<Operand>>::size_type i = 0;
				i < code.args.size(); ++i)
		{
			if (i < 4)
			{
				// $a_i <- args[i]
				code.assembly.splice(code.assembly.end(), 
					load_operand(code.args[i], 
						Register::Reg((int)Register::a0 + i)));
			}
			else
			{
				// $t0 <- args[i]
				code.assembly.splice(code.assembly.end(), 
					load_operand(code.args[i], Register::t0));
				// sw $t0, 4*i($fp)
				code.assembly.push_back(make_shared<MemoryInstr>(
							Instruction::SW, Register::t0, Register::fp, 4 * i));
			}
		}

		// jal fun
		code.assembly.push_back(make_shared<JInstruction>(
					Instruction::JAL, code.fun_name));
		// result <- v0
		code.assembly.push_back(
				access_variable(code.result, Instruction::SW, Register::v0));

	}
	
	void CodeGenerationVisitor::visit(Declare &code)
	{
		var_info.insert({code.operand, VarInfo(VarInfo::LOCAL, local_var_offset)});
		local_var_offset += code.size;
	}
}
