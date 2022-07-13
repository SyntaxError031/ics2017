#include "cpu/instr.h"

make_instr_func(xchg_esp) {
	uint32_t temp = cpu.esp;
	cpu.esp = cpu.eax;
	cpu.eax = temp;

	return 1;
}
