#include "cpu/instr.h"

static void instr_execute_2op() {
	operand_read(&opr_src);
	operand_read(&opr_dest);
	alu_sub(opr_src.val, opr_dest.val);
}

make_instr_impl_2op(cmp, r, rm, v)
make_instr_impl_2op(cmp, i , rm, v)
make_instr_impl_2op(cmp, rm, r, v)
make_instr_impl_2op(cmp, i, rm, b)
make_instr_impl_2op(cmp, rm, r, b)
make_instr_impl_2op(cmp, i, a, v)

make_instr_func(cmp_si2rm_v) {
	OPERAND rm, imm;
	rm.data_size = data_size;
	rm.type = OPR_MEM;
	rm.sreg = SREG_SS;

	int len = 1;
	len += modrm_rm(eip+1, &rm);

	imm.type = OPR_IMM;
	imm.addr = eip + len;
	imm.data_size = 8;

	operand_read(&imm);
	operand_read(&rm);
	alu_sub(sign_ext(imm.val,8),sign_ext(rm.val,data_size));
	
	return len + 1;
}

make_instr_func(cmp_i2a_b) {
	OPERAND imm;
	imm.type = OPR_IMM;
	imm.data_size = 8;
	imm.addr = eip + 1;

	operand_read(&imm);
	alu_sub(imm.val, cpu.gpr[0]._8[0]);

	return 2;
}


