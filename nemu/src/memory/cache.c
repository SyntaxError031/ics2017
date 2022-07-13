#include "memory/cache.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include<memory.h>
#include<stdio.h>

extern CacheLine L1_dcache[1024];

void init_cache() {
	for(int i = 0; i < 1024; i++) {
			L1_dcache[i].valid_bit = 0;
	}
}

uint32_t cache_read(paddr_t paddr, size_t len, CacheLine *cache) {
	uint8_t block_num = paddr & 0x3f;
	uint8_t group_num = (paddr>>6) & 0x7f;
	uint32_t tag = (paddr>>13) & 0x7ffff;
	uint32_t ret = 0;
	uint32_t temp1, temp2;
	char hit = 'F';

	//printf("read: %x %d %x %x %d\n", paddr, block_num, group_num, tag, len);

	for(int i = 0; i < 8; i++) {
		if(cache[group_num*8+i].tag == tag) {
			hit = 'T';
			if(cache[group_num*8+i].valid_bit == 1) {                   //hit
				if(len + block_num <= 64 ) {     //in the same line
					memcpy(&ret, &cache[group_num*8+i].data[block_num], len);
					//printf("ret: %x\n", ret);
					return ret;	
				}
				else if(len == 2) {
					temp1 = cache_read(paddr+1, 1, cache);
					ret = (temp1<<8) | cache[group_num*8+i].data[block_num];
//					for(int j = 0; j < 8; j++) {
//						if(cache[group_num*8+8+j].tag == tag) {
//							if(cache[group_num*8+8+j].valid_bit == 1) {
//								memcpy(&temp1, &cache[group_num*8+8+j].data[0], 1);
//								ret = cache[group_num*8+i].data[block_num] | temp1>>8;
//							}
//							else {
//								temp1 = hw_mem_read(paddr+1, 1);
//								memcpy(&cache[group_num*8+8+j].data[0], &temp1, 1);
//								cache[group_num*8+8+j].valid_bit = 1;
//								cache_read(paddr, len, cache);
//							}
//						}
//					}
				}
				else {
					temp1 = cache_read(paddr+(64-block_num), block_num-60, cache);
					temp2 = cache_read(paddr, 64-block_num, cache);
					ret = (temp1<<(8*(64-block_num))) | temp2;
//					for(int j = 0; j < 8; j++) {
//						if(cache[group_num*8+8+j].tag == tag) {
//							if(cache[group_num*8+8+j].valid_bit == 1) {
//							memcpy(&temp1, &cache[group_num*8+8+j].data[0], block_num-60);
//							memcpy(&temp2, &cache[group_num*8+i].data[block_num], 64-block_num);
//							ret = temp1>>(8*(64-block_num)) + temp2;
//							}
//							else {
//								temp1 = hw_mem_read(paddr+1, block_num-60);
//								memcpy(&cache[group_num*8+8+j].data[0], &temp1, block_num-60);
//								cache[group_num*8+8+j].valid_bit = 1;
//								cache_read(paddr, len, cache);
//							}	
//						}						
//					}
				}
			}
			else {    //valid bit != 1
				hit = 'F';
//				ret = hw_mem_read(paddr, len);
//				cache_write(&cache[group_num*8+i].data[block_num], len, ret, cache);
//				cache[group_num*8+i].valid_bit = 1;
			}
		}
	}

	char is_full = 'T';	
	if(hit == 'F') {
		ret = hw_mem_read(paddr, len);
		//printf("%x\n", ret);
		for(int i = 0; i < 8; i++) {
			if(cache[group_num*8+i].valid_bit == 0) {
				is_full = 'F';
				memcpy(&cache[group_num*8+i].data[0], hw_mem+(paddr&0xffffffc0), 64);
				//printf("%x\n", cache[group_num*8+i].data[0]);
				cache[group_num*8+i].valid_bit = 1;
				cache[group_num*8+i].tag = tag;
				break;
			}
		}
		if(is_full == 'T') {            //random replacement
			memcpy(&cache[group_num*8].data[0], hw_mem+(paddr&0xffffffc0), 64);
			cache[group_num*8].valid_bit = 1;
			cache[group_num*8].tag = tag;
		}
	}

	//printf("ret: %x\n", ret);	
	return ret;
}

void cache_write(paddr_t paddr, size_t len, uint32_t data, CacheLine *cache) {
	uint8_t block_num = paddr & 0x3f;
	uint8_t group_num = (paddr>>6) & 0x7f;
	uint32_t tag = (paddr>>13) & 0x7ffff;
//	uint32_t temp1, temp2;
	hw_mem_write(paddr, len, data);
	paddr_t p = paddr - block_num;

	//printf("write: %x %d %x %x %d %x\n", paddr, block_num, group_num, tag, len, data);

	for(int i = 0; i < 8; i++) {
		if(cache[group_num*8+i].tag == tag) {
			if(cache[group_num*8+i].valid_bit == 1) {
				memcpy(cache[group_num*8+i].data, hw_mem + p, 64);
				if(len + block_num <= 64) {
					cache[group_num*8+i].valid_bit = 0;
					//memcpy(cache[group_num*8+i].data, hw_mem + p, 64);
					cache[group_num*8+i].valid_bit = 1;
				}
				else if(len == 2) {
					p = p + 0x40;
					//memcpy(&cache[group_num*8+i].data, &data, 1);
					for(int j = 0; j < 8; j++) {
						if(cache[group_num*8+8+j].tag == tag && cache[group_num*8+8+j].valid_bit == 1) {
							cache[group_num*8+8+j].valid_bit = 0;
							memcpy(cache[group_num*8+8+j].data, hw_mem + p, 64);
							//change
							cache[group_num*8+8+j].valid_bit = 1;
						}
					}
				}
				else {
 					p = p + 0x40;
					//memcpy(&cache[group_num*8+i].data[block_num], &data, 64-block_num);
					for(int j = 0; j < 8; j++) {
						if(cache[group_num*8+8+j].tag == tag && cache[group_num*8+8+j].valid_bit == 1) {
							cache[group_num*8+8+j].valid_bit = 0;
							memcpy(cache[group_num*8+8+j].data, hw_mem + p, 64);
							//change
							cache[group_num*8+8+j].valid_bit = 1;
						}
					}
				}
			}
		}
	}
}
