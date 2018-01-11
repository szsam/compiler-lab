#pragma once

#include <string>
#include <iostream>

// MIPS32 assembly language
namespace mips32_asm
{

	struct Assembly
	{
		virtual ~Assembly() = default;
		virtual std::ostream& output(std::ostream &os) const = 0;
	};
	
	inline
	std::ostream &operator<<(std::ostream &os, const Assembly &instr) 
	{ return instr.output(os); }
	
	struct Label : public Assembly
	{
		Label(const std::string &lbl) :label(lbl) {}
		std::ostream& output(std::ostream &os) const
		{
			return os << label << ":";
		}
	
		std::string label;
	};
	
	struct Register
	{
	 	static const char *regs[32];
		enum Reg { zero, at, v0, v1, a0, a1, a2, a3,
				   t0, t1, t2, t3, t4, t5, t6, t7,
	 	  	   	   s0, s1, s2, s3, s4, s5, s6, s7,
	 	  	   	   t8, t9, k0, k1, gp, sp, fp, ra
		}reg;
	
		Register(Reg r) : reg(r) {}
		const char *str() const { return regs[reg]; }
	};
	
	struct Instruction : public Assembly
	{
		enum OP { ADDU, ADDIU, SUBU, SUBIU, MUL, DIV, MFLO,
				  LW, SW, J, JR, JAL, 
				  BEQ, BNE, BGT, BLT, BGE, BLE,
				  MOVE, LI, LA, B
		};
	
		Instruction(OP _op) : op(_op) {}
		std::ostream& output(std::ostream &os) const 
		{ return os << "\t" << op_str[op] << " "; }
		// virtual ~Instruction() = default;
	
		OP op;
		static const char *op_str[];
	};
	
	struct RInstruction : public Instruction
	{
		// OP rd, rs, rt
		// e.g. addu subu mul
		RInstruction(OP _op, Register _rd, Register _rs, Register _rt):
			Instruction(_op), rd(_rd), rs(_rs), rt(_rt) {}
		std::ostream& output(std::ostream &os) const 
		{ 
			Instruction::output(os); 
			return os << "$" << rd.str() << ", $" << rs.str() << ", $" << rt.str();
		}
	
		Register rd, rs, rt;
	};
	
	struct IInstruction : public Instruction
	{
		// OP rt, rs, IMM
		// e.g. addiu subiu
		IInstruction(OP _op, Register _rt, Register _rs, int _imm):
			Instruction(_op), rs(_rs), rt(_rt), imm(_imm) {}
		std::ostream& output(std::ostream &os) const 
		{ 
			Instruction::output(os); 
			return os << "$" << rt.str() << ", $" << rs.str() << ", " << imm;
		}
	
		Register rs, rt;
		int imm;
	};
	
	struct JInstruction : public Instruction
	{
		// OP label
		JInstruction(OP _op, const std::string &lbl): Instruction(_op), label(lbl) {}
		std::ostream& output(std::ostream &os) const 
		{ 
			Instruction::output(os); 
			return os << label;
		}
	
		std::string label;
	};
	
	struct BranchInstr : public Instruction
	{
		// banch instructions
		// beq bne bgt blt bge ble
		// OP rs, rt, label
		BranchInstr(OP _op, Register _rs, Register _rt, const std::string &lbl):
			Instruction(_op), rs(_rs), rt(_rt), label(lbl) {}
		std::ostream& output(std::ostream &os) const 
		{ 
			Instruction::output(os); 
			return os << "$" << rs.str() << ", $" << rt.str() << ", " << label;
		}
	
		Register rs, rt;
		std::string label;
	};
	
	struct MemoryInstr : public Instruction
	{
		// memory instructions
		// lw sw
		// OP rt, offset(base)
		MemoryInstr(OP _op, Register _rt, Register _base, int _offset):
			Instruction(_op), rt(_rt), base(_base), offset(_offset) {}
		std::ostream& output(std::ostream &os) const 
		{ 
			Instruction::output(os); 
			return os << "$" << rt.str() << ", " 
				<< offset << "($" << base.str() << ")";
		}
	
		Register rt, base;
		int offset;
	};
	
	struct SingleRegInstr : public Instruction
	{
		// OP r
		// jr mflo
		SingleRegInstr(OP _op, Register _r):
			Instruction(_op), r(_r) {}
		std::ostream& output(std::ostream &os) const 
		{ 
			Instruction::output(os); 
			return os << "$" << r.str();
		}
	
		Register r;
	};
	
	struct DoubleRegInstr : public Instruction
	{
		// OP r1, r2
		// div move
		DoubleRegInstr(OP _op, Register _r1, Register _r2):
			Instruction(_op), r1(_r1), r2(_r2) {}
		std::ostream& output(std::ostream &os) const 
		{ 
			Instruction::output(os); 
			return os << "$" << r1.str() << ", $" << r2.str();
		}
	
		Register r1, r2;
	};
	
	struct I2RInstruction : public Instruction
	{
		// OP r, IMM
		// li
		I2RInstruction(OP _op, Register _r, int _imm):
			Instruction(_op), r(_r), imm(_imm) {}
		std::ostream& output(std::ostream &os) const 
		{ 
			Instruction::output(os); 
			return os << "$" << r.str() << ", " << imm;
		}
	
		Register r;
		int imm;
	};
	
	struct LoadAddrInstr : public Instruction
	{
		// OP r, label
		// la
		LoadAddrInstr(OP _op, Register _r, const std::string &lbl):
			Instruction(_op), r(_r), label(lbl) {}
		std::ostream& output(std::ostream &os) const 
		{ 
			Instruction::output(os); 
			return os << "$" << r.str() << ", " << label;
		}
	
		Register r;
		std::string label;
	};

}
