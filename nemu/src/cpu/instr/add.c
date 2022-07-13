#include "cpu/instr.h"

static void instr_execute_2op() {
	operand_read(&opr_src);
	operand_read(&opr_dest);
	opr_dest.val = alu_add(opr_src.val,opr_dest.val);
	operand_write(&opr_dest);
}

make_instr_impl_2op(add, i, rm, v)
make_instr_impl_2op(add, r, rm, v)
make_instr_impl_2op(add, r, rm, b)
make_instr_impl_2op(add, rm, r, v)


make_instr_func(add_si2rm_v) {
	OPERAND rm, imm;
	rm.data_size = data_size;
	imm.data_size = 8;

	int len = 1;
	len += modrm_rm(eip + 1, &rm);

	imm.type = OPR_IMM;
	imm.addr = eip + len;
	rm.sreg = SREG_SS;

	operand_read(&imm);
	operand_read(&rm);
	rm.val = alu_add(sign_ext(imm.val,8), rm.val);
	operand_write(&rm);
	
	return len + 1;
}

make_instr_func(add_i2a_v) {
	OPERAND imm;
	imm.type = OPR_IMM;
	imm.addr = eip + 1;
	imm.data_size = data_size;

	operand_read(&imm);
	if(data_size == 16) {
		uint16_t res = imm.val + (uint16_t)cpu.eax;
		cpu.eax = ((cpu.eax>>16)<<16) + res;
	}
	else if(data_size == 32) {
		cpu.eax = alu_add(imm.val, cpu.eax);
	}

	return 1 + data_size / 8;
}
