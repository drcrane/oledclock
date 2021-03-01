#include <msp430.h>
#include <stdint.h>

#include "utilityfn.h"
#include "ssd1306.h"
#include "ter_ssd1306.h"

volatile int uart_data;
volatile int txctr;

void hwuart_sendb(char b) {
	while(!(IFG2 & UCA0TXIFG));
	UCA0TXBUF = b;
}

void hwuart_sendstr(char * ptr) {
	while (*ptr != 0) {
		while(!(IFG2 & UCA0TXIFG));
		UCA0TXBUF = *ptr++;
	}
}

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;
	P1DIR |= BIT0;
	
	BCSCTL1 = CALBC1_8MHZ;
	DCOCTL = CALDCO_8MHZ;
	
	//BCSCTL1 = CALBC1_1MHZ;
	//DCOCTL = CALDCO_1MHZ;
	
	/* Configure hardware UART */
	P1SEL |= BIT1 | BIT2 ; // P1.1 = RXD, P1.2=TXD
	P1SEL2 |= BIT1 | BIT2 ; // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSSEL_2; // Use SMCLK
	// 9600 @ 1MHz
	//UCA0BR0 = 104; // Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13)
	//UCA0BR1 = 0; // Set baud rate to 9600 with 1MHz clock
	//UCA0MCTL = UCBRS_1; // Modulation UCBRSx = 1
	// 9600 @ 8MHz
	UCA0BR0 = 64;
	UCA0BR1 = 3;
	UCA0MCTL = UCBRS_2;
	UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
	IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
	
	// Initialise UCB0 for I2C
	P1SEL |= BIT6 | BIT7;
	P1SEL2 |= BIT6 | BIT7;
	UCB0CTL1 |= UCSWRST;
	UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
	UCB0CTL1 = UCSSEL_2 | UCSWRST;
	//UCB0BR0 = 12;
	UCB0BR0 = 96;
	UCB0BR1 = 0;
	/*  MCP3422:   1  1  0  1 A2 A1 A0 RW (RW Bit not in UCB0I2CSA) */
	//UCB0I2CSA = 0x68;
	/* MCP23017:   0  1  0  0 A2 A1 A0 RW (RW Bit not in UCB0I2CSA) */
	//UCB0I2CSA = 0x20;
	/* SSD1306 OLED:  0  1  1  1  1  0  SA0 RW (RW Bit not in UCB0I2CSA) */
	UCB0I2CSA = 0x3c;
	UCB0CTL1 &= ~UCSWRST;
	//IE2 |= UCB0RXIE;
	//IE2 |= UCB0TXIE;
	
	UCA0TXBUF = 'I';
	
	__eint();
	
	uart_data = 0;
	
	while (1) {
		if (uart_data == 'S') {
			UCB0CTL1 |= UCSWRST;
			UCB0I2CSA = 0x68;
			UCB0CTL1 &= ~UCSWRST;
			UCB0CTL1 |= UCTR | UCTXSTT;
			while (!(IFG2 & UCB0TXIFG));
			UCB0TXBUF = 0xAC;
			while (!(IFG2 & UCB0TXIFG));

			UCB0CTL1 |= UCTXSTP;
			IFG2 &= ~UCB0TXIFG;
			UCA0TXBUF = 's';
			uart_data = 0;
		}
		if (uart_data == 'X') {
			UCA0TXBUF = 'x';
			uart_data = 0;
		}
		if (uart_data == 'T') {
			int a, b, c, d, e;
			char buf[10];
			UCB0I2CSA = 0x68;
			UCB0CTL1 &= ~UCTR;
			UCB0CTL1 |= UCTXSTT;
			while (!(IFG2 & UCB0RXIFG));
			a = UCB0RXBUF;
			while (!(IFG2 & UCB0RXIFG));
			b = UCB0RXBUF;
			while (!(IFG2 & UCB0RXIFG));
			c = UCB0RXBUF;
			while (!(IFG2 & UCB0RXIFG));
			d = UCB0RXBUF;
			UCB0CTL1 |= UCTXSTP;
			while (!(IFG2 & UCB0RXIFG));
			e = UCB0RXBUF;

			Utility_intToHex(buf, &a, 1);
			Utility_intToHex(buf + 2, &b, 1);
			Utility_intToHex(buf + 4, &c, 1);
			Utility_intToHex(buf + 6, &d, 1);
			hwuart_sendstr(buf);
			Utility_intToHex(buf, &e, 1);
			hwuart_sendstr(buf);
			hwuart_sendstr("\r\n");
			uart_data = 0;
		}

		if (uart_data == 'I') {
			// Initilise SSD1306 (over I2C)
			UCB0I2CSA = 0x3c;
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
			ssd1306_command_3( 0x21, 0, 127 );
			ssd1306_command_3( 0x22, 0,   7 );
			ssd1306_command_1(SSD_Display_On);
			//ssd1306_command_2(SSD1306_MEMORYMODE, HORIZONTAL_MODE);
			//ssd1306_command_1(SSD_Deactivate_Scroll);
			//ssd1306_command_1(SSD_Display_On);
			UCA0TXBUF = 'i';
			uart_data = 0;
		}
		if (uart_data == 'L') {
			// Switch the screen into Off mode
			UCB0I2CSA = 0x3c;
			ssd1306_command_1(SSD_Display_Off);
			ssd1306_command_2(SSD1306_CHARGEPUMP, 0x10);
			uart_data = 0;
		}
		if (uart_data == 'C') {
			// Clear the screen
			UCB0I2CSA = 0x3c;
			ssd1306_clear();
			ssd1306_command_3( 0x21, 0, 127 );
			ssd1306_command_3( 0x22, 0, 7 );
			uart_data = 0;
		}
		if (uart_data == 'D') {
			// Write a single character
			/*
			ssd1306_command_1(SSD1306_SETLOWCOLUMN | 0);
			ssd1306_command_1(SSD1306_SETHIGHCOLUMN | 0);
			ssd1306_command_1(SSD1306_SETPAGE | 0);
			ssd1306_data_write(8, (char *)ter_ssd1306 + (16*('a'-0x20)));
			ssd1306_command_1(SSD1306_SETLOWCOLUMN | 0);
			ssd1306_command_1(SSD1306_SETPAGE | 1);
			ssd1306_data_write(8, (char *)ter_ssd1306 + (16*('a'-0x20)) + 8);
			*/

			// Write a string of characters
			ssd1306_writestringz(8, 16, "Java");

			uart_data = 0;
		}
		if (uart_data == 'R') {
			UCB0CTL1 |= UCSWRST;
			UCB0I2CSA = 0x3c;
			UCB0CTL1 &= ~UCSWRST;
			ssd1306_command_1(SSD_Inverse_Display);
			UCA0TXBUF = 'r';
			uart_data = 0;
		}
		if (uart_data == 'N') {
			ssd1306_command_1(SSD1306_Normal_Display);
			UCA0TXBUF = 'n';
			uart_data = 0;
		}
		if (uart_data == 'O') {
			ssd1306_command_2(SSD_Set_ContrastLevel, 0x0F);
			UCA0TXBUF = 'o';
			uart_data = 0;
		}
		if (uart_data == 'P') {
			ssd1306_command_2(SSD_Set_ContrastLevel, 0x01);
			UCA0TXBUF = 'p';
			uart_data = 0;
		}
		if (uart_data == 'Q') {
			ssd1306_command_2(SSD_Set_ContrastLevel, 0x7f);
			UCA0TXBUF = 'q';
			uart_data = 0;
		}
	}
}

void USCI0RX_ISR(void) __attribute__( ( interrupt( USCIAB0RX_VECTOR ) ) );
void USCI0RX_ISR(void) {
	int tmp_data;
	tmp_data = UCA0RXBUF & 0xff;
	if (tmp_data != '\n') {
		uart_data = tmp_data;
	}
	__bic_SR_register_on_exit(CPUOFF);
}

void USCI0TX_ISR(void) __attribute__ ( ( interrupt( USCIAB0TX_VECTOR ) ) );
void USCI0TX_ISR(void) {
	//i2c_data = UCB0RXBUF;
	if (txctr--) {
		UCB0TXBUF = 0x90;
	} else {
		UCB0CTL1 |= UCTXSTP;
		IFG2 &= ~UCB0TXIFG;
	}
	//__bic_SR_register_on_exit(CPUOFF);
}

