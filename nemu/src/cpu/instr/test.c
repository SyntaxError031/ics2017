#include "cpu/instr.h"

static void instr_execute_2op() {
	operand_read(&opr_src);
	operand_read(&opr_dest);
	alu_and(opr_src.val, opr_dest.val);
}

make_instr_impl_2op(test, r, rm, v)
make_instr_impl_2op(test, r, rm, b)
make_instr_impl_2op(test, i, rm, v)
make_instr_impl_2op(test, i, rm, b)

make_instr_func(test_i2a_v) {
	OPERAND imm;
	imm.type = OPR_IMM;
	imm.data_size = data_size;
	imm.addr = eip + 1;

	operand_read(&imm);
	alu_and(imm.val, cpu.eax);

	return 1 + data_size / 8;
}

make_instr_func(test_i2a_b) {
	OPERAND imm;
	imm.type = OPR_IMM;
	imm.data_size = 8;
	imm.addr = eip + 1;

	operand_read(&imm);
	alu_and(imm.val, cpu.eax);

	return 2;
}

