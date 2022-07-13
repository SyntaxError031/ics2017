#include "cpu/cpu.h"

void set_CF_add(uint32_t result, uint32_t src) {
	cpu.eflags.CF = result < src;
}

void set_CF_adc(uint32_t result, uint32_t src, uint32_t dest) {
	if(dest+cpu.eflags.CF < dest || dest+cpu.eflags.CF < cpu.eflags.CF)
		cpu.eflags.CF = 1;
	else {
		if(result < src || result < src+cpu.eflags.CF)
			cpu.eflags.CF = 1;
		else
			cpu.eflags.CF = 0;
	}
}

void set_CF_sub(uint32_t result, uint32_t dest) {
	cpu.eflags.CF = result > dest;
}

void set_CF_sbb(uint32_t result, uint32_t src, uint32_t dest) {
	if(dest-cpu.eflags.CF > dest)
		cpu.eflags.CF = 1;
	else {
		if(result > dest-cpu.eflags.CF)
			cpu.eflags.CF = 1;
		else
			cpu.eflags.CF = 0;
	}
}

void set_PF(uint32_t result) {
	uint32_t temp = (result << 24);
	int val = 0;
	while (temp != 0) {
		val = temp ^ val;
	       	temp >>= 1;
	}
	cpu.eflags.PF = !( val & 0x1);
}

void set_ZF(uint32_t result) {
	cpu.eflags.ZF = (result == 0);
}

void set_SF(uint32_t result) {
	cpu.eflags.SF = sign(result);
}

void set_OF_add(uint32_t result, uint32_t src, uint32_t dest) {
	if(sign(src) == sign(dest)) {
		if(sign(src) != sign(result))
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	}
	else {
		cpu.eflags.OF = 0;
	}
}

void set_OF_sub(uint32_t result, uint32_t src, uint32_t dest) {
	if(sign(dest) != sign(src)) {
		if(sign(result) == sign(src))
			cpu.eflags.OF = 1;
		else
			cpu.eflags.OF = 0;
	}
	else {
		cpu.eflags.OF = 0;
	}
}

uint32_t alu_add(uint32_t src, uint32_t dest) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	res = dest + src;

	set_CF_add(res,src);
	set_PF(res);
	set_ZF(res);
	set_SF(res);
	set_OF_add(res,src,dest);
	//assert(0);
	return res;
}

uint32_t alu_adc(uint32_t src, uint32_t dest) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	res = src + dest + cpu.eflags.CF;
	
	if(cpu.eflags.CF == 0)
		set_OF_add(res,src,dest);
	if(cpu.eflags.CF == 1)
		set_OF_add(res,src+1,dest);
	set_CF_adc(res,src,dest);
	set_PF(res);
	set_ZF(res);
	set_SF(res);
	//assert(0);
	return res;
}


uint32_t alu_sub(uint32_t src, uint32_t dest) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	res = dest - src;

	set_CF_sub(res,dest);
	set_PF(res);
	set_ZF(res);
	set_SF(res);
	set_OF_sub(res,src,dest);
	//assert(0);
	return res;
}

uint32_t alu_sbb(uint32_t src, uint32_t dest) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	res = dest - src - cpu.eflags.CF;
	
	if(cpu.eflags.CF == 0)
		set_OF_sub(res,src,dest);
	if(cpu.eflags.CF == 1)
		set_OF_sub(res,src+1,dest);
	set_CF_sbb(res,src,dest);
	set_PF(res);
	set_ZF(res);
	set_SF(res);
	//set_OF_sbb(res,src,dest);
	//assert(0);
	return res;
}


uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint64_t res = 0;
	if(data_size == 32) {
		res = (uint64_t)src * (uint64_t)dest;
		uint32_t high_32 = (res>>32) & 0xffffffff;
		if(high_32 == 0) {
			cpu.eflags.CF = 0;
			cpu.eflags.OF = 0;
		}
		else {
			cpu.eflags.CF = 1;
			cpu.eflags.OF = 1;
		}
		set_PF(res);
		cpu.eflags.SF = (res >> 63) & 0x1;
		cpu.eflags.ZF = (res == 0);
	}
	if(data_size == 16) {
		res = src * dest;
		uint16_t high_16 = (res>>16) & 0xffff;
		if(high_16 == 0) {
			cpu.eflags.CF = 0;
			cpu.eflags.OF = 0;
		}
		else {
			cpu.eflags.CF = 1;
			cpu.eflags.OF = 1;
		}
		set_PF(res);
		set_ZF(res);
		set_SF(res);
	}
	if(data_size == 8) {
		res = src * dest;
		uint8_t high_8 = (res>>8) & 0xff;
		if(high_8 == 0) {
			cpu.eflags.CF = 0;
			cpu.eflags.OF = 0;
		}
		else {
			cpu.eflags.CF = 1;
			cpu.eflags.OF = 1;
		}
		set_PF(res);
		set_ZF(res);
		set_SF(res);
	}

	//assert(0);
	return res;
}

int64_t alu_imul(int32_t src, int32_t dest, size_t data_size) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	int64_t res = 0;
	res = (int64_t)src * (int64_t)dest;
	//assert(0);
	return res;
}

uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	if(src != 0) {
		res = dest / src;
		set_PF(res);
		set_ZF(res);
		set_SF(res);
	}
	else
		assert(0);
	return res;
}

int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	int32_t res = 0;
	if(src != 0) {
		res = dest / src;
		set_PF(res);
		set_ZF(res);
		set_SF(res);
	}
	else
		assert(0);
	return res;
}

uint32_t alu_mod(uint64_t src, uint64_t dest) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	res = dest % src;
	//assert(0);
	return res;
}

int32_t alu_imod(int64_t src, int64_t dest) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	int32_t res = 0;
	res = dest % src;
	//assert(0);
	return res;
}

uint32_t alu_and(uint32_t src, uint32_t dest) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	res = dest & src;

	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(res);
	set_SF(res);
	set_ZF(res);
	//assert(0);
	return res;
}

uint32_t alu_xor(uint32_t src, uint32_t dest) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	res = dest ^ src;

	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(res);
	set_SF(res);
	set_ZF(res);
	//assert(0);
	return res;
}

uint32_t alu_or(uint32_t src, uint32_t dest) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	res = dest | src;

	cpu.eflags.CF = 0;
	cpu.eflags.OF = 0;
	set_PF(res);
	set_SF(res);
	set_ZF(res);
	//assert(0);
	return res;
}

uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	if(data_size == 8) {
		uint8_t temp = (dest & 0x000000ff);
		uint8_t temp_res = 0;
		if(src < data_size) {
			temp_res = (temp << src);
			set_PF(temp_res);
			cpu.eflags.SF = (temp_res>>7) & 0x1;
			cpu.eflags.ZF = (temp_res == 0);
			cpu.eflags.CF = (temp >> (data_size-src)) & 0x1;
			if(src == 1 && sign(temp) != sign(temp<<1))
				cpu.eflags.OF = 1;
		}
		res = (uint32_t)temp_res + (dest & 0xffffff00);
	}	
	if (data_size == 16) {
		uint16_t temp = (dest & 0x0000ffff);
		uint16_t temp_res = 0;
		if (src < data_size) {
			temp_res = (temp << src);
			set_PF(temp_res);
			cpu.eflags.SF = (temp_res>>15) & 0x1;
			cpu.eflags.ZF = (temp_res == 0);
			cpu.eflags.CF = (temp >> (data_size - src)) & 0x1;
			if (src == 1 && sign(temp) != sign(temp << 1))
				cpu.eflags.OF = 1;
		}
		res = (uint32_t)temp_res + (dest & 0xffff0000);
	}
	if(data_size == 32) {
		if(src < data_size) {
			res = dest << src;
			cpu.eflags.CF = (dest >> (data_size-src)) & 0x1;
			if(src == 1 && sign(dest) != sign(dest<<1))
				cpu.eflags.OF = 1;
		}
		set_PF(res);
		set_SF(res);
		set_ZF(res);
	}
	
	//assert(0);
	return res;
}

uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	uint32_t res = 0;
	if(data_size == 8) {
		uint8_t temp = dest & 0x000000ff;
		uint8_t temp_res = 0;
		if(src < data_size) {
			temp_res = (temp >> src);
			cpu.eflags.CF = (temp>>(src-1)) & 0x1;
			set_PF(temp_res);
			cpu.eflags.SF = (temp_res>>7) & 0x1;
			cpu.eflags.ZF = (temp_res == 0);
		}
		res = (uint32_t)temp_res + (dest & 0xffffff00);
	}
	if(data_size == 16) {
		uint16_t temp = dest & 0x0000ffff;
		uint16_t temp_res = 0;
		if(src < data_size) {
			temp_res = (temp >> src);
			cpu.eflags.CF = (temp>>(src-1)) & 0x1;
			set_PF(temp_res);
			cpu.eflags.SF = (temp_res>>15) & 0x1;
			cpu.eflags.ZF = (temp_res == 0);
		}
		res = (uint32_t)temp_res + (dest & 0xffff0000);
	}
	if(data_size == 32) {
		if(src < data_size) {
			res = dest >> src;
			cpu.eflags.CF = (dest>>(src-1)) & 0x1;
			set_PF(res);
			set_SF(res);
			set_ZF(res);
		}
	}
	//assert(0);
	return res;
}

uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size) {
	uint32_t res = 0;
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	if(data_size == 8) {
		uint8_t temp = dest & 0x000000ff;
		uint8_t temp_res = 0;
		if(src < data_size) {
			temp_res = (char)temp >> src;
			cpu.eflags.CF = (temp >> (src-1)) & 0x1;
			set_PF(temp_res);
			cpu.eflags.SF = (temp_res>>7) & 0x1;
			cpu.eflags.ZF = (temp_res == 0);
		}
		res = (uint32_t)temp_res + (dest & 0xffffff00);
	}
	if(data_size == 16) {
		uint16_t temp = dest & 0x0000ffff;
		uint16_t temp_res = 0;
		if(src < data_size) {
			temp_res = (short)temp >> src;
			cpu.eflags.CF = (temp >> (src-1)) & 0x1;
			set_PF(temp_res);
			cpu.eflags.SF = (temp_res>>15) & 0x1;
			cpu.eflags.ZF = (temp_res == 0);
		}
		res = (uint32_t)temp_res + (dest & 0xffff0000);
	}
	if(data_size == 32) {
		if(src < data_size) {
			res = (int)dest >> src;	
			cpu.eflags.CF = (dest>>(src-1)) & 0x1;
			set_PF(res);
			set_SF(res);
			set_ZF(res);
		}
	}
	
	//assert(0);
	return res;
}

uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size) {
	//printf("\e[0;31mPlease implement me at alu.c\e[0m\n");
	//assert(0);
	return alu_shl(src,dest,data_size);
}
