#include "cpu/instr.h"

make_instr_func(lgdt) {

	OPERAND rm;
	rm.type = OPR_MEM;
	rm.data_size = 32;
	rm.sreg = SREG_SS;

	int len = 1;
	len += modrm_rm(eip + 1, &rm);
	operand_read(&rm);
	//printf("%x\n", rm.addr);
	uint32_t limit = laddr_read(rm.addr, 2);
	uint32_t base = laddr_read(rm.addr + 2, 4);
	//printf("%x %x\n", limit, base);
	
	cpu.gdtr.limit = limit;
	cpu.gdtr.base = base;

	//printf("%d\n", len);

	return len;
}

	


