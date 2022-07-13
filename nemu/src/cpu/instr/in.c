#include "cpu/instr.h"
#include "device/port_io.h"

make_instr_func(in_d2a_b) {
	uint16_t port = cpu.edx;
	uint32_t data = pio_read(port, 1);
	cpu.gpr[0]._8[0] = data;
	
	return 1;
}

make_instr_func(in_d2a_v) {
	uint16_t port = cpu.edx;
	uint32_t data = pio_read(port, 4);
	cpu.eax = data;

	return 1;
}
