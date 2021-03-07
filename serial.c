#include <msp430.h>
#include "serial.h"

struct i2c_context i2c_ctx;
struct uart_context uart_ctx;

void uart_init() {
	uart_ctx.rxlinebuf = uart_ctx.rxlinebuf_a;
	uart_ctx.rxlinebuf_pos = 0;
	uart_ctx.flags = 0;
}

uint8_t * uart_getlinebuf() {
	if (uart_ctx.rxlinebuf == uart_ctx.rxlinebuf_b) {
		return uart_ctx.rxlinebuf_a;
	}
	return uart_ctx.rxlinebuf_b;
}

