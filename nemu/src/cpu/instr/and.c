#include "cpu/instr.h"

static void instr_execute_2op() {
	operand_read(&opr_src);
	operand_read(&opr_dest);
	opr_dest.val = alu_and(opr_src.val, opr_dest.val);
	operand_write(&opr_dest);
}

make_instr_impl_2op(and, rm, r, b)
make_instr_impl_2op(and, r, rm, v)
make_instr_impl_2op(and, i, rm, v)
make_instr_impl_2op(and, i, rm, b)
make_instr_impl_2op(and, i, a, v)

make_instr_func(and_si2rm_v) {
	OPERAND imm, rm;
	rm.data_size = data_size;
	imm.data_size = 8;

	int len = 1;
	len += modrm_rm(eip + 1, &rm);

	imm.type = OPR_IMM;
	imm.addr = eip + len;
	rm.sreg = SREG_SS;

	operand_read(&imm);
	operand_read(&rm);
	rm.val = alu_and(sign_ext(imm.val,8), rm.val);
	operand_write(&rm);

	return len + 1;
}

