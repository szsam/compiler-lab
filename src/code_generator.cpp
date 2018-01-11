#include "code_generator.h"

void CodeGenerator::output(std::ostream &os) const
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

void CodeGenerator::generate_machine_code()
{
	for (const auto &fun_code : inter_code)
	{
		// generate machine code for a function
		std::list<std::shared_ptr<mips32_asm::Assembly>> machine_code_fun;
		for (const auto &p_code : fun_code)
		{
			machine_code_fun.splice(machine_code_fun.end(),
					p_code->emit_machine_code());
		}
		machine_code.push_back(machine_code_fun);
	}

}
