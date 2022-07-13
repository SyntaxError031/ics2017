#include "cpu/cpu.h"
#include "memory/memory.h"
#include "assert.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg) {
	/* TODO: perform segment translation from virtual address to linear address
	 * by reading the invisible part of the segment register 'sreg'
	 */
	uint32_t base = cpu.segReg[sreg].base;
	return base + offset;
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg) {
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */
	uint32_t index = cpu.segReg[sreg].index;
	//printf("%x %x\n", index, cpu.gdtr.base);
	SegDesc seg_desc;
	//seg_desc.val[0] = *(int *)(cpu.gdtr.base + 8*index);
	seg_desc.val[0] = laddr_read(cpu.gdtr.base + 8*index, 4);
	seg_desc.val[1] = laddr_read(cpu.gdtr.base+8*index+4, 4);
	//printf("%x %x\n", seg_desc.val[0], seg_desc.val[1]);

	cpu.segReg[sreg].base = seg_desc.base_15_0 + (seg_desc.base_23_16<<16) + (seg_desc.base_31_24<<24);
	assert(cpu.segReg[sreg].base == 0);

	cpu.segReg[sreg].limit = seg_desc.limit_15_0 + (seg_desc.limit_19_16<<16);
	assert(cpu.segReg[sreg].limit == 0xfffff);
	assert(seg_desc.granularity == 1);

	cpu.segReg[sreg].type = seg_desc.type;
	cpu.segReg[sreg].privilege_level = seg_desc.privilege_level;
	cpu.segReg[sreg].soft_use = seg_desc.soft_use;

}
