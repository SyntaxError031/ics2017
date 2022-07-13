#include "cpu/cpu.h"
#include "memory/memory.h"
#include "assert.h"

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr) {
#ifndef TLB_ENABLED
	//printf("\nPlease implement page_translate()\n");
	//assert(0);
	uint32_t dir = (laddr>>22) & 0x3ff;
	uint32_t page = (laddr>>12) & 0x3ff;
	uint32_t offset = laddr & 0x00000fff;

	uint32_t pd_addr = (cpu.cr3.pdbr<<12) + (dir<<2);    //page director address
	PDE pde;
	pde.val = paddr_read(pd_addr, 4);
	assert(pde.present == 1);

	uint32_t pt_addr = (pde.page_frame<<12) + (page<<2);    //page table address
	PTE pte;
	pte.val = paddr_read(pt_addr, 4);
	assert(pte.present == 1);
	
	return (pte.page_frame<<12)+offset;
	
#else	
	return tlb_read(laddr) | (laddr & PAGE_MASK);;
#endif
}
