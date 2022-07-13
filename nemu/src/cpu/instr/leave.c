#include "cpu/instr.h"

make_instr_func(leave_v) {
	cpu.esp = cpu.ebp;

	OPERAND rm;
	rm.type = OPR_MEM;
	rm.data_size = data_size;
	rm.addr = cpu.esp;
	rm.sreg = SREG_SS;
	operand_read(&rm);

	if(data_size == 16) {
		cpu.gpr[5]._16 = rm.val;
	}
	else if (data_size == 32) {
		cpu.ebp = rm.val;
	}

	cpu.esp += 4;
	return 1;
}

