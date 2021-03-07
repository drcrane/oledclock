#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>

struct i2c_context {
	uint8_t txbuf[16];
	size_t txbuf_pos;
	size_t txbuf_sz;
};

extern struct i2c_context i2c_ctx;

#endif /* SERIAL_H */

