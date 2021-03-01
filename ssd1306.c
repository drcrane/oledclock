#include <msp430.h>
#include "ssd1306.h"
#include "ter_ssd1306.h"

void ssd1306_command_1(int cmd) {
	UCB0CTL1 |= UCTR | UCTXSTT;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0TXBUF = SSD_Command_Mode;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0TXBUF = cmd;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0CTL1 |= UCTXSTP;
	while (UCB0STAT & UCBBUSY);
}

void ssd1306_command_2(int cmd, int arg) {
	UCB0CTL1 |= UCTR | UCTXSTT;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0TXBUF = SSD_Command_Mode;
	while (!(IFG2 & UCB0TXIFG)) {  };
	
	UCB0TXBUF = cmd;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0TXBUF = arg;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0CTL1 |= UCTXSTP;
	while (UCB0STAT & UCBBUSY);
}

void ssd1306_command_3(int cmd, int arg1, int arg2) {
	UCB0CTL1 |= UCTR | UCTXSTT;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0TXBUF = SSD_Command_Mode;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0TXBUF = cmd;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0TXBUF = arg1;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0TXBUF = arg2;
	while (!(IFG2 & UCB0TXIFG));
	
	UCB0CTL1 |= UCTXSTP;
	while (UCB0STAT & UCBBUSY);
}

void ssd1306_data_write(int count, char * buf) {
	int i;
	i = 0;
	UCB0CTL1 |= UCTR | UCTXSTT;
	while (!(IFG2 & UCB0TXIFG));
	UCB0TXBUF = SSD_Data_Mode;
	while (!(IFG2 & UCB0TXIFG));
	while (count--) {
		UCB0TXBUF = buf[i];
		while (!(IFG2 & UCB0TXIFG));
		i++;
	}
	UCB0CTL1 |= UCTXSTP;
	while (UCB0STAT & UCBBUSY);
}

void ssd1306_writebyte(int count, int byte) {
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
}



