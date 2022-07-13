#include "nemu.h"

typedef struct{
	char valid_bit;
	uint32_t tag;
	uint8_t data[64];
}CacheLine;

void init_cache();
uint32_t cache_read(paddr_t paddr, size_t len, CacheLine *cache);
void cache_write(paddr_t paddr, size_t len, uint32_t data, CacheLine *cache);
CacheLine L1_dcache[1024];
