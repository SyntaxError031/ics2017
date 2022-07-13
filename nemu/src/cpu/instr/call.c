#include "cpu/instr.h"

make_instr_func(call_rel_v) {
	OPERAND rm1, rm2;
	cpu.esp -= 4;

	rm1.data_size = data_size;
	rm1.type = OPR_MEM;
	rm1.addr = cpu.esp;
	rm1.sreg = SREG_SS;
	
	rm2.data_size = data_size;
	rm2.type = OPR_MEM;
	rm2.addr = cpu.eip + 1;
	rm2.sreg = SREG_SS;
	operand_read(&rm2);
	//printf("%x\n", rm2.val);
	
	if(data_size == 16) {		
		rm1.val = (cpu.eip+1+data_size/8) & 0x0000ffff;
		operand_write(&rm1);
		cpu.eip = alu_and(0x0000ffff,cpu.eip+rm2.val);
	}
	else if(data_size == 32) {
		rm1.val = cpu.eip+1+data_size/8;
		operand_write(&rm1);
		cpu.eip = cpu.eip + rm2.val;
	}

	return 1 + data_size / 8;
}

make_instr_func(call_rm_near_v) {
	OPERAND rm1, rm2;
	cpu.esp -= 4;

	rm1.data_size = data_size;
	rm1.type = OPR_MEM;
	rm1.sreg = SREG_SS;
	rm2.data_size = 32;
	rm2.type = OPR_MEM;
	rm2.addr = cpu.esp;
	
	int len = 1;
	len += modrm_rm(eip + 1, &rm1);
	rm2.sreg = SREG_SS;
	operand_read(&rm1);
	//printf("%x\n",rm1.val);

	if(data_size == 16) {
		rm2.val = sign_ext(((cpu.eip+len) & 0x0000ffff),data_size);
		operand_write(&rm2);
		cpu.eip = rm1.val & 0x0000ffff;
	}

	else if(data_size == 32) {
		rm2.val = cpu.eip + len;
		//printf("%x\n",rm2.val);
		operand_write(&rm2);
		cpu.eip = rm1.val;
	}

	return 0;
}
