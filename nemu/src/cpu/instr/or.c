#include "cpu/instr.h"

static void instr_execute_2op() {
	operand_read(&opr_src);
	operand_read(&opr_dest);
	opr_dest.val = alu_or(opr_src.val, opr_dest.val);
	operand_write(&opr_dest);
}

make_instr_impl_2op(or, r, rm, v)
make_instr_impl_2op(or, rm, r, b)
make_instr_impl_2op(or, i, rm, b)
make_instr_impl_2op(or, i, rm, v)

make_instr_func(or_si2rm_v) {
	OPERAND imm, rm;

	imm.data_size = 8;
	rm.data_size = data_size;
	rm.type = OPR_MEM;
	rm.sreg = SREG_SS;

	int len = 1;
	len += modrm_rm(eip + 1, &rm);

	imm.type = OPR_IMM;
	imm.addr = eip + len;

	operand_read(&imm);
	operand_read(&rm);
	rm.val = alu_or(sign_ext(imm.val,8), rm.val);
	operand_write(&rm);
	
	return len + 1;
}
