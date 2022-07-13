#include "cpu/instr.h"

make_instr_func(ret_near) {
	OPERAND imm;
	imm.type = OPR_IMM;
	imm.data_size = data_size;
	imm.addr = cpu.esp;

	operand_read(&imm);
	cpu.eip = imm.val;
	cpu.esp += 4;

	return 0;
}

make_instr_func(ret_near_i) {
	OPERAND imm1, imm2;

	imm1.type = OPR_IMM;
	imm1.data_size = data_size;
	imm1.addr = cpu.esp;
	//imm1.sreg = SREG_SS; 

	imm2.type = OPR_IMM;
	imm2.data_size = 16;
	imm2.addr = eip + 1;

	operand_read(&imm1);
	operand_read(&imm2);
	cpu.eip = imm1.val;
	cpu.esp += 4;
	cpu.esp += imm2.val;

	return 0;
}

make_instr_func(iret) {
	OPERAND rm;
	rm.type = OPR_MEM;
	rm.data_size = 32;
	rm.sreg = SREG_SS;

	//pop EIP
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.eip = rm.val;
	cpu.esp += 4;

	//pop CS
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.cs.val = rm.val;
	cpu.esp += 4;

	//pop EFLAGS
	rm.addr = cpu.esp;
	operand_read(&rm);
	cpu.eflags.val = rm.val;
	cpu.esp += 4;

	return 0;
}

