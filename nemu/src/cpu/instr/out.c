#include "cpu/instr.h"
#include "device/port_io.h"

make_instr_func(out_a2d_b) {
	uint16_t port = cpu.edx;
	uint8_t data = cpu.gpr[0]._8[0];
	pio_write(port, 1, data);

	return 1;
}
