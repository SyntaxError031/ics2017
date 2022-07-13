#include "cpu/instr.h"

make_instr_func(jmp_near) {
        OPERAND rel;
        rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

	int offset = sign_ext(rel.val, data_size);
	print_asm_1("jmp", "", 1 + data_size / 8, &rel);

	cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_short_) {
	OPERAND rel;
	rel.type = OPR_IMM;
	rel.sreg = SREG_CS;
	rel.data_size = 8;
	rel.addr = eip + 1;

	operand_read(&rel);
	cpu.eip += sign_ext(rel.val, 8);

	return 2;
}

make_instr_func(jmp_near_in) {
	OPERAND rm;
	rm.type = OPR_MEM;
	rm.data_size = data_size;
	rm.sreg = SREG_SS;
	
	int len = 1;
	len += modrm_rm(eip + 1, &rm);
	rm.sreg = SREG_SS;
	operand_read(&rm);
	cpu.eip = rm.val;

	//printf("%x %x\n", rm.val, rm.addr);
	return 0;
}

make_instr_func(jmp_long) {
	OPERAND imm1, imm2;
	imm1.type = OPR_IMM;
	imm1.data_size = 32;
	imm1.addr = eip + 1;
	imm2.type = OPR_IMM;
	imm2.data_size = 16;
	imm2.addr = eip + 5;

	operand_read(&imm1);
	//printf("%x\n", imm1.val);
	operand_read(&imm2);
	//printf("%x\n", imm2.val);

	cpu.cs.val = imm2.val;
	//printf("%x\n", cpu.cs.val);
	load_sreg(1);
	cpu.eip = imm1.val;
	//printf("%x\n",cpu.eip);

	return 0;
}

	

