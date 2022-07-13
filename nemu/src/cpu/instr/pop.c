#include "cpu/instr.h"

make_instr_func(pop_2r_v) {
	OPERAND r, rm;

	r.type = OPR_REG;
	r.data_size = data_size;
	r.addr = opcode & 0x7;

	rm.type = OPR_MEM;
	rm.data_size = data_size;
	rm.addr = cpu.esp;
	rm.sreg = SREG_SS;

	operand_read(&rm);
	r.val = rm.val;
	operand_write(&r);
	cpu.esp += 4;
	
	return 1;
}

make_instr_func(popa) {
	OPERAND rm;
	rm.type = OPR_MEM;
	rm.data_size = 32;
	rm.sreg = SREG_SS;

	//pop EDI
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.edi = rm.val;
	cpu.esp += 4;

	//pop ESI
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.esi = rm.val;
	cpu.esp += 4;

	//pop EBP
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.ebp = rm.val;
	cpu.esp += 4;

	//skip ESP
	cpu.esp += 4;

	//pop EBX
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.ebx = rm.val;
	cpu.esp += 4;

	//pop EDX
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.edx = rm.val;
	cpu.esp += 4;

	//pop ECX
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.ecx = rm.val;
	cpu.esp += 4;

	//pop EAX
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.eax = rm.val;
	cpu.esp += 4;

	return 1;
}


	



