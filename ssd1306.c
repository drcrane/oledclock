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
	ssd1306_command_3( 0x21, 0, 127 );
	ssd1306_command_3( 0x22, 0, 7 );
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

