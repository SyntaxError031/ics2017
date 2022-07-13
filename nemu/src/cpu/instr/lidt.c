#include "cpu/instr.h"

make_instr_func(lidt) {
	OPERAND rm;
	rm.type = OPR_MEM;
	rm.data_size = 32;
	rm.sreg = SREG_SS;

	int len = 1;
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	uint32_t limit = laddr_read(rm.addr, 2);
	uint32_t base = laddr_read(rm.addr+2, 4);
	//printf("%x %x\n", limit, base);

	cpu.idtr.limit = limit;
	cpu.idtr.base = base;

	return len;
}
