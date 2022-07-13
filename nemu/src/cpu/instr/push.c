#include "cpu/instr.h"

make_instr_func(push_r_v) {
	OPERAND r, rm;
	
	//cpu.esp -= 4;
	r.data_size = data_size;
	r.type = OPR_REG;
	r.addr = opcode & 0x7;
	operand_read(&r);

	cpu.esp -= 4;
	rm.data_size = data_size;
	rm.type = OPR_MEM;
	rm.addr = cpu.esp;
	rm.sreg = SREG_SS;

	//operand_read(&r);
	rm.val = r.val;
	operand_write(&rm);

	return 1;
}

make_instr_func(push_rm_v) {
	OPERAND rm_src, rm_dest;

	cpu.esp -= 4;
	int len = 1;
	len += modrm_rm(eip + 1, &rm_src);

	rm_src.data_size = data_size;
	rm_dest.data_size = 32;
	rm_src.type = OPR_MEM;
	rm_dest.type = OPR_MEM;
	rm_dest.addr = cpu.esp;
	rm_dest.sreg = SREG_SS;
	rm_src.sreg = SREG_SS;

	operand_read(&rm_src);
	rm_dest.val = rm_src.val;
	operand_write(&rm_dest);

	return len;
}

make_instr_func(push_i_b) {
	OPERAND rm, imm;
	cpu.esp -= 4;

	imm.type = OPR_IMM;
	imm.data_size = 8;
	imm.addr = eip + 1;
	rm.type = OPR_MEM;
	rm.addr = cpu.esp;
	rm.data_size = 32;
	rm.sreg = SREG_SS;

	operand_read(&imm);
	rm.val = sign_ext(imm.val, 8);
	operand_write(&rm);

	return 2;
}

make_instr_func(push_i_v) {
	OPERAND rm, imm;
	cpu.esp -= 4;

	imm.type = OPR_IMM;
	imm.data_size = data_size;
	imm.addr = eip + 1;
	rm.type = OPR_MEM;
	rm.data_size = 32;
	rm.addr = cpu.esp;
	rm.sreg = SREG_SS;

	operand_read(&imm);
	rm.val = sign_ext(imm.val, data_size);
	operand_write(&rm);

	return 1 + data_size / 8;
}

make_instr_func(pusha) {
	uint32_t temp = cpu.esp;
	OPERAND rm;
	rm.data_size = 32;
	rm.type = OPR_MEM;
	rm.sreg = SREG_SS;

	//push EAX
	cpu.esp -= 4;
	rm.addr = cpu.esp;
	rm.val = cpu.eax;
	operand_write(&rm);

	//push ECX
	cpu.esp -= 4;
	rm.addr = cpu.esp;
	rm.val = cpu.ecx;
	operand_write(&rm);

	//push EDX
	cpu.esp -= 4;
	rm.addr = cpu.esp;
	rm.val = cpu.edx;
	operand_write(&rm);

	//push EBX
	cpu.esp -= 4;
	rm.addr = cpu.esp;
	rm.val = cpu.ebx;
	operand_write(&rm);

	//push old ESP
	cpu.esp -= 4;
	rm.addr = cpu.esp;
	rm.val = temp;
	operand_write(&rm);

	//push EBP
	cpu.esp -= 4;
	rm.addr = cpu.esp;
	rm.val = cpu.ebp;
	operand_write(&rm);

	//push ESI
	cpu.esp -= 4;
	rm.addr = cpu.esp;
	rm.val = cpu.esi;
	operand_write(&rm);

	//push EDI
	cpu.esp -= 4;
	rm.addr = cpu.esp;
	rm.val = cpu.edi;
	operand_write(&rm);

	return 1;
}






