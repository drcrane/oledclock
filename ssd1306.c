#include <msp430.h>
#include "ssd1306.h"
#include "ter_ssd1306.h"
#include "serial.h"

uint8_t ssd1306_frame_buffer[SSD1306_FRAMEBUFFERSZ];
int ssd1306_fb_top_x, ssd1306_fb_top_y;

void ssd1306_command_1(int cmd) {
	while (UCB0STAT & UCBBUSY);

	i2c_ctx.txbuf[0] = SSD_Command_Mode;
	i2c_ctx.txbuf[1] = cmd;
	i2c_ctx.txbuf_sz = 2;
	i2c_ctx.txbuf_pos = 0;

	UCB0CTL1 |= UCTR | UCTXSTT;
//	while (!(IFG2 & UCB0TXIFG));	
//	UCB0TXBUF = SSD_Command_Mode;
//	while (!(IFG2 & UCB0TXIFG));
//	UCB0TXBUF = cmd;
//	while (!(IFG2 & UCB0TXIFG));
//	UCB0CTL1 |= UCTXSTP;
//	while (UCB0STAT & UCBBUSY);
}

void ssd1306_command_2(int cmd, int arg) {
	while (UCB0STAT & UCBBUSY);

	i2c_ctx.txbuf[0] = SSD_Command_Mode;
	i2c_ctx.txbuf[1] = cmd;
	i2c_ctx.txbuf[2] = arg;
	i2c_ctx.txbuf_sz = 3;
	i2c_ctx.txbuf_pos = 0;
	UCB0CTL1 |= UCTR | UCTXSTT;
}

void ssd1306_command_3(int cmd, int arg1, int arg2) {
	while (UCB0STAT & UCBBUSY);
	i2c_ctx.txbuf[0] = SSD_Command_Mode;
	i2c_ctx.txbuf[1] = cmd;
	i2c_ctx.txbuf[2] = arg1;
	i2c_ctx.txbuf[3] = arg2;
	i2c_ctx.txbuf_sz = 4;
	i2c_ctx.txbuf_pos = 0;
	UCB0CTL1 |= UCTR | UCTXSTT;
}

void ssd1306_data_write(int count, char * buf) {
	uint8_t * dst_buf;
	while (UCB0STAT & UCBBUSY);
	i2c_ctx.txbuf[0] = SSD_Data_Mode;
	dst_buf = i2c_ctx.txbuf + 1;
	i2c_ctx.txbuf_sz = count + 1;
	i2c_ctx.txbuf_pos = 0;
	while (count--) {
		*dst_buf = *buf;
		dst_buf++;
		buf++;
	}
	UCB0CTL1 |= UCTR | UCTXSTT;
}

void ssd1306_writebyte(int count, int byte) {
	while (UCB0STAT & UCBBUSY);
	IE2 &= ~UCB0TXIE;
	UCB0CTL1 |= UCTR | UCTXSTT;
	while (!(IFG2 & UCB0TXIFG));
	UCB0TXBUF = SSD_Data_Mode;
	while (!(IFG2 & UCB0TXIFG));
	while (count--) {
		UCB0TXBUF = byte;
		while (!(IFG2 & UCB0TXIFG));
	}
	UCB0CTL1 |= UCTXSTP;
	while (UCB0STAT & UCBBUSY);
	IFG2 &= ~(UCB0TXIFG);
	IE2 |= UCB0TXIE;
}

void ssd1306_writestringz(int posx, int posy, char * strz) {
	char * pos;
	char c;
	ssd1306_command_1(SSD1306_SETLOWCOLUMN | (posx & 0xf));
	ssd1306_command_1(SSD1306_SETHIGHCOLUMN | ((posx >> 4) & 0xf));
	ssd1306_command_1(SSD1306_SETPAGE | ((posy >> 3) & 0xf));
	pos = strz;
	c = *pos;
	while (c != 0) {
		ssd1306_data_write(8, (char *)ter_ssd1306 + (16*(c-0x20)));
		pos ++;
		c = *pos;
	}
	ssd1306_command_1(SSD1306_SETLOWCOLUMN | (posx & 0xf));
	ssd1306_command_1(SSD1306_SETHIGHCOLUMN | ((posx >> 4) & 0xf));
	ssd1306_command_1(SSD1306_SETPAGE | (((posy >> 3) & 0xf) + 1));
	pos = strz;
	c = *pos;
	while (c != 0) {
		ssd1306_data_write(8, (char *)ter_ssd1306 + (16*(c-0x20)) + 8);
		pos ++;
		c = *pos;
	}
}

void ssd1306_clear() {
	int col;
	int row;
	row = 8;
	while (row--) {
		col = 128;
		ssd1306_command_3( 0x21, 0, 127 );
		ssd1306_command_3( 0x22, row, row );
		ssd1306_writebyte(128, 0x0);
	}
	ssd1306_command_3( 0x21, 0, 127 );
	ssd1306_command_3( 0x22, 0, 7 );
}

void ssd1306_writeframebuffer() {
	int posx = ssd1306_fb_top_x;
	int posy = ssd1306_fb_top_y >> 3;
	int ctr = 4;
	while (ctr) {
		ssd1306_command_1(SSD1306_SETLOWCOLUMN | (posx & 0xf));
		ssd1306_command_1(SSD1306_SETHIGHCOLUMN | ((posx >> 4) & 0xf));
		ssd1306_command_1(SSD1306_SETPAGE | (posy & 0xf));
		ssd1306_data_write(32, (char *)&ssd1306_frame_buffer[(posy) * 32]);
		posy ++;
		ctr --;
	}
}

/*
 * Write a pixel in the frame buffer
 * care must be taken not to draw outside the buffer
 * bounds checking could be added?
 */
static void ssd1306_write_pixel(int x, int y) {
	int offs;
	uint8_t curr_byte;
	uint8_t byte;
	x = x - ssd1306_fb_top_x;
	y = y - ssd1306_fb_top_y;
	if (x < 0) { return; }
	if (y < 0) { return; }
	offs = x + ((y >> 3) * 32);
	byte = 0x1;
	byte = byte << (y & 0x7);
	if (offs >= 128) { return; }
	curr_byte = ssd1306_frame_buffer[offs];
	curr_byte = curr_byte | byte;
	ssd1306_frame_buffer[offs] = curr_byte;
	//printf("offs %02d %02d,%02d %x\n", offs, x, y, curr_byte);
}

static void ssd1306_draw_line_low(int x0, int y0, int x1, int y1) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int yi = 1;
	int d, x, y;
	if (dy < 0) {
		yi = -1;
		dy = -dy;
	}
	d = (2 * dy) - dx;
	y = y0;

	for (x = x0; x <= x1; x++) {
		ssd1306_write_pixel(x, y);
		if (d > 0) {
			y = y + yi;
			d = d + (2 * (dy - dx));
		} else {
			d = d + (2 * dy);
		}
	}
}

static void ssd1306_draw_line_high(int x0, int y0, int x1, int y1) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	int xi = 1;
	int d, x, y;
	if (dx < 0) {
		xi = -1;
		dx = -dx;
	}
	d = 2 * dx;
	x = x0;

	for (y = y0; y <= y1; y ++) {
		ssd1306_write_pixel(x, y);
		if (d > 0) {
			x = x + xi;
			d = d + (2 * (dx - dy));
		} else {
			d = d + (2 * dx);
		}
	}
}

void ssd1306_draw_line(int x0, int y0, int x1, int y1) {
	int dx = x1 - x0;
	int dy = y1 - y0;
	if (dx < 0) { dx = -dx; }
	if (dy < 0) { dy = -dy; }
	if (dy < dx) {
		if (x0 > x1) {
			ssd1306_draw_line_low(x1, y1, x0, y0);
		} else {
			ssd1306_draw_line_low(x0, y0, x1, y1);
		}
	} else {
		if (y0 > y1) {
			ssd1306_draw_line_high(x1, y1, x0, y0);
		} else {
			ssd1306_draw_line_high(x0, y0, x1, y1);
		}
	}
}

void ssd1306_initialise() {
	//UCB0I2CSA = 0x3c;
	ssd1306_command_1(SSD_Display_Off);
	ssd1306_command_2(SSD1306_SETDISPLAYCLOCKDIV, 0x80);
	ssd1306_command_2(SSD1306_SETMULTIPLEX, 0x3f);
	//ssd1306_command_2(SSD1306_SETDISPLAYCLOCKDIV, 0x80);
	//ssd1306_command_2(SSD1306_SETMULTIPLEX, 0x3f); // 128x64
	ssd1306_command_2(SSD1306_SETDISPLAYOFFSET, 0x00);
	ssd1306_command_1(SSD1306_SETSTARTLINE | 0x0);
	ssd1306_command_2(SSD1306_CHARGEPUMP, 0x14); // internal vcc
			
	//ssd1306_command_2(SSD1306_MEMORYMODE, PAGE_MODE);
	//ssd1306_command_2(SSD1306_MEMORYMODE, VERTICAL_MODE);
			
	//ssd1306_command_2(SSD1306_MEMORYMODE, HORIZONTAL_MODE);
	ssd1306_command_1(SSD1306_SEGREMAP | 0x1);
	ssd1306_command_1(SSD1306_COMSCANDEC);
	ssd1306_command_2(SSD1306_SETCOMPINS, 0x12);
	ssd1306_command_2(SSD_Set_ContrastLevel, 0xcf);//0x7F);
	ssd1306_command_2(SSD1306_SETPRECHARGE, 0xF1);
	ssd1306_command_2(SSD1306_SETVCOMDETECT, 0x40);
	ssd1306_command_1(SSD1306_DISPLAYALLON_RESUME);
	ssd1306_command_1(SSD1306_Normal_Display);
	//ssd1306_command_3( 0x21, 0, 127 );
	//ssd1306_command_3( 0x22, 0,   7 );
	//ssd1306_command_1(SSD_Display_On);
}

