#include "code_generator.h"

namespace ir
{
	const char *CodeGenerationVisitor::startup_code = 
		".data\n"
		"_prompt: .asciiz \"Enter an integer:\"\n"
		"_ret: .asciiz \"\\n\"\n"
		".globl main\n"
		".text\n"
		"read:\n"
		"	li $v0, 4\n"
		"	la $a0, _prompt\n"
		"	syscall\n"
		"	li $v0, 5\n"
		"	syscall\n"
		"	jr $ra\n"
		"\n"
		"write:\n"
		"	li $v0, 1\n"
		"	syscall\n"
		"	li $v0, 4\n"
		"	la $a0, _ret\n"
		"	syscall\n"
		"	move $v0, $0\n"
		"	jr $ra\n";

}
