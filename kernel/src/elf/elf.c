#include "common.h"
#include "memory.h"
#include "string.h"

#include <elf.h>

#ifdef HAS_DEVICE_IDE
#define ELF_OFFSET_IN_DISK 0
#endif

#define STACK_SIZE (1 << 20)

void ide_read(uint8_t *, uint32_t, uint32_t);
void ide_write(uint8_t *, uint32_t, uint32_t);
void create_video_mapping();
uint32_t get_ucr3();

uint32_t loader() {	
	Elf32_Ehdr *elf;
	Elf32_Phdr *ph, *eph;

#ifdef HAS_DEVICE_IDE
	uint8_t buf[4096];
	ide_read(buf, ELF_OFFSET_IN_DISK, 4096);
	elf = (void*)buf;
	Log("ELF loading from hard disk.");
#else
	elf = (void *)0x0;
	Log("ELF loading from ram disk.");
#endif

	/* Load each program segment */
	ph = (void *)elf + elf->e_phoff;
	eph = ph + elf->e_phnum;
	for(; ph < eph; ph ++) {
		if(ph->p_type == PT_LOAD) {

			//panic("Please implement the loader");
			uint32_t addr;
#ifdef IA32_PAGE
			addr = mm_malloc(ph->p_vaddr, ph->p_memsz);
#else
			addr = ph->p_vaddr;
#endif
			/* TODO: copy the segment from the ELF file to its proper memory area */
			//Log("%x %x\n", ph->p_vaddr, addr);
			
			//char *src = (void *)ph->p_offset;
			//char *dest = (void *)ph->p_vaddr;
			uint8_t *dest = (void *)addr;
			
#ifdef HAS_DEVICE_IDE	
			//uint8_t buffer;
			ide_read(dest, ph->p_offset, ph->p_filesz);
			//ide_write(buf, addr, ph->p_filesz);
			//for(int i = 0; i < ph->p_filesz; i++) {
				//*(dest+i) = buffer[i];
			//}
#else
			uint8_t *src = (void *)ph->p_offset;
			//char *dest = (void *)addr;
			for(int i = 0; i < ph->p_filesz; i++) {
				*(dest+i) = *(src+i);
			}
#endif
			
			/* TODO: zeror the memory area [vaddr + file_sz, vaddr + mem_sz) */
			
			for(int i = ph->p_filesz; i < ph->p_memsz; i++) {
				//char *res = (char *)ph->p_vaddr+i;
				char *res = (char *)addr+i;
				*res = 0;
			}

#ifdef IA32_PAGE
			/* Record the program break for future use */
			extern uint32_t brk;
			uint32_t new_brk = ph->p_vaddr + ph->p_memsz - 1;
			if(brk < new_brk) { brk = new_brk; }
#endif
		}
	}

	volatile uint32_t entry = elf->e_entry;

#ifdef IA32_PAGE
	mm_malloc(KOFFSET - STACK_SIZE, STACK_SIZE);
#ifdef HAS_DEVICE_VGA
	create_video_mapping();
#endif
	write_cr3(get_ucr3());
#endif
	return entry;
}
