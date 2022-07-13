#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"


void raise_intr(uint8_t intr_no) {
#ifdef IA32_INTR
	//printf("Please implement raise_intr()");
	//assert(0);

	//Trigger an exception/interrupt with 'intr_no'
	//'intr_no' is the index to the IDT
	//Push EFLAGS, CS, and EIP
	OPERAND rm_eflags, rm_cs, rm_eip;

	cpu.esp -= 4;
	rm_eflags.data_size = 32;
	rm_eflags.type = OPR_MEM;
	rm_eflags.addr = cpu.esp;
	rm_eflags.sreg = SREG_SS;
	rm_eflags.val = cpu.eflags.val;
	operand_write(&rm_eflags);

	cpu.esp -= 4;
	rm_cs.data_size = 32;
	rm_cs.type = OPR_MEM;
	rm_cs.addr = cpu.esp;
	rm_cs.sreg = SREG_SS;
	rm_cs.val = sign_ext(cpu.cs.val, 16);
	operand_write(&rm_cs);

	cpu.esp -= 4;
	rm_eip.data_size = 32;
	rm_eip.type = OPR_MEM;
	rm_eip.addr = cpu.esp;
	rm_eip.sreg = SREG_SS;
	rm_eip.val = cpu.eip;
	operand_write(&rm_eip);

	//Find the IDT entry using 'intr_no'
	GateDesc gate_desc;
	gate_desc.val[0] = laddr_read(cpu.idtr.base + 8*intr_no, 4);
	gate_desc.val[1] = laddr_read(cpu.idtr.base + 8*intr_no + 4, 4);
	
	//Clear IF if it is an interrupt
	if(gate_desc.type == 0xe)
		cpu.eflags.IF = 0;

	//Set EIP to the entry of the interrupt handle
	cpu.cs.val = gate_desc.selector;
	uint32_t addr = gate_desc.offset_15_0 + (gate_desc.offset_31_16<<16);
	cpu.eip = addr;

#endif
}

void raise_sw_intr(uint8_t intr_no) {
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
