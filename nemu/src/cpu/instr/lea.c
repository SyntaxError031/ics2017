#include "cpu/instr.h"

/*
static void instr_execute_2op() {
	operand_read(&opr_src);
	opr_dest.val = opr_src.addr;
	operand_write(&opr_dest);
}

make_instr_impl_2op(lea, rm, r, v)
*/

make_instr_func(lea_rm2r_v) {
	OPERAND r, rm;

	r.data_size = data_size;
	rm.data_size = data_size;
	rm.type = OPR_MEM;
	r.type = OPR_REG;

	int len = 1;
	len += modrm_r_rm(eip + 1, &r, &rm);
	rm.sreg = SREG_SS;

	//operand_read(&rm);
	r.val = rm.addr;
	//printf("%x %x\n", rm.addr, r.val);
	operand_write(&r);

	return len;
}

