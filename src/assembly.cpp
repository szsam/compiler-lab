#include "assembly.h"

namespace mips32_asm
{
	const char *Register::regs[32] = {
	   "zero", "at", "v0", "v1", "a0", "a1", "a2", "a3",
	   "t0", "t1", "t2", "t3", "t4", "t5", "t6", "t7",
	   "s0", "s1", "s2", "s3", "s4", "s5", "s6", "s7",
	   "t8", "t9", "k0", "k1", "gp", "sp", "fp", "ra"
	 };
	
	const char *Instruction::op_str[] = {
		"addu", "addiu", "subu", "subiu", "mul", "div", "mflo",
		"lw", "sw", "j", "jr", "jal", 
		"beq", "bne", "bgt", "blt", "bge", "ble",
		"move", "li", "la", "b"
	};
}

// int main()
// {
// 	Label lbl("label1");
// 	std::cout << lbl << std::endl;
// 	
// 	RInstruction instr1(Instruction::ADDU, Register::t1, Register::t2, Register::t3);
// 	std::cout << instr1 << std::endl;
// 
// 	IInstruction instr2(Instruction::ADDIU, Register::t1, Register::t2, 28);
// 	std::cout << instr2 << std::endl;
// 
// 	BranchInstr instr3(Instruction::BEQ, Register::t1, Register::t2, "label1");
// 	std::cout << instr3 << std::endl;
// 
// 	MemoryInstr instr4(Instruction::LW, Register::t1, Register::t2, 4);
// 	std::cout << instr4 << std::endl;
// 
// 	JInstruction instr5(Instruction::JAL, "func");
// 	std::cout << instr5 << std::endl;
// 	
// 	SingleRegInstr instr6(Instruction::JR, Register::ra);
// 	std::cout << instr6 << std::endl;
// 	
// 	DoubleRegInstr instr7(Instruction::MOVE, Register::t1, Register::t2);
// 	std::cout << instr7 << std::endl;
// 	
// 	I2RInstruction instr8(Instruction::LI, Register::t1, 2);
// 	std::cout << instr8 << std::endl;
// }
