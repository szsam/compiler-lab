#include "ir_gen_visitor.h"
#include "assembly.h"

class CodeGenerator
{
public:
	CodeGenerator(
		const std::list<std::list<std::shared_ptr<ir::InterCode>>> 
		&_inter_code) : inter_code(_inter_code) {}
	void generate_machine_code();
	void output(std::ostream &) const;

private:
	// intermediate-code
	std::list<std::list<std::shared_ptr<ir::InterCode>>> inter_code;
	// machine-code
	std::list<std::list<std::shared_ptr<mips32_asm::Assembly>>> machine_code;
};
