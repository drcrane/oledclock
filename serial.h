#ifndef SERIAL_H
#define SERIAL_H

#include <stdint.h>
#include <stddef.h>

struct i2c_context {
	uint8_t txbuf[16];
	size_t txbuf_pos;
	size_t txbuf_sz;
};

#define UART_LINEBUFSZ 32
#define UART_HASRECEIVED 1

struct uart_context {
	uint8_t * rxlinebuf;
	uint16_t flags;
	uint16_t rxlinebuf_pos;
	uint8_t rxlinebuf_a[UART_LINEBUFSZ];
	uint8_t rxlinebuf_b[UART_LINEBUFSZ];
};

extern struct i2c_context i2c_ctx;

extern struct uart_context uart_ctx;

void uart_init();
uint8_t * uart_getlinebuf();

#endif /* SERIAL_H */

