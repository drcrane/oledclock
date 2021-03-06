#include <msp430.h>
#include <stdint.h>

#include "utilityfn.h"
#include "ssd1306.h"
#include "ter_ssd1306.h"
#include "rtc.h"
#include "timerfn.h"
#include "serial.h"
#include "clockface.h"

char txbuf[28];

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

void write_time();

void oled_displaytime() {
	write_time();
	UCB0I2CSA = 0x3c;
	ssd1306_writestringz(64, 0, txbuf);
	//P2OUT ^= BIT0;
	int toupdate;
	toupdate = clockface_getminutequadrant(rtc_ctx.second);
	if (rtc_ctx.second == 16) {
		toupdate |= 1;
	} else
	if (rtc_ctx.second == 31) {
		toupdate |= 2;
	} else
	if (rtc_ctx.second == 45) {
		toupdate |= 4;
	} else
	if (rtc_ctx.second == 0) {
		toupdate |= 8;
	}
	if (toupdate & 1) {
		clockface_drawtopright();
	}
	if (toupdate & 2) {
		clockface_drawbottomright();
	}
	if (toupdate & 4) {
		clockface_drawbottomleft();
	}
	if (toupdate & 8) {
		clockface_drawtopleft();
	}
	timer_callback(1, oled_displaytime);
}

void oled_drawline() {
	UCB0I2CSA = 0x3c;

	clockface_drawtopright();
	clockface_drawbottomright();
	clockface_drawbottomleft();
	clockface_drawtopleft();

	timer_callback(1, oled_displaytime);
}

void oled_initialise() {
	UCB0I2CSA = 0x3c;
	ssd1306_initialise();
	ssd1306_clear();
	ssd1306_command_1(SSD_Display_On);
	//timer_callback(1, oled_displaytime);
	timer_callback(1, oled_drawline);
}

void write_time() {
	char * buf = txbuf;
	buf += Utility_intToAPadded(buf, rtc_ctx.hour, 10, 2);
	*buf = ':';
	buf++;
	buf += Utility_intToAPadded(buf, rtc_ctx.minute, 10, 2);
	*buf = ':';
	buf++;
	buf += Utility_intToAPadded(buf, rtc_ctx.second, 10, 2);
}

void led_toggle() {
	P2OUT ^= BIT0;
	timer_callback(10, led_toggle);
}

int main(void) {
	WDTCTL = WDTPW + WDTHOLD;
	P1DIR |= BIT0;
	
	/* Setup Button for Interrupt */
	P1REN |= BIT5;
	P1OUT |= BIT5;
	P1IFG &= ~(BIT5);
	P1IES |= BIT5;
	P1IE |= BIT5;

	P2DIR |= BIT0;
	P2OUT &= ~(BIT0);

	//BCSCTL1 = CALBC1_8MHZ;
	//DCOCTL = CALDCO_8MHZ;
	
	BCSCTL1 = CALBC1_1MHZ;
	DCOCTL = CALDCO_1MHZ;
	
	/* Configure hardware UART */
	P1SEL |= BIT1 | BIT2 ; // P1.1 = RXD, P1.2=TXD
	P1SEL2 |= BIT1 | BIT2 ; // P1.1 = RXD, P1.2=TXD
	UCA0CTL1 |= UCSWRST;
	UCA0CTL1 |= UCSSEL_2; // Use SMCLK
	// 9600 @ 1MHz
	UCA0BR0 = 104; // Set baud rate to 9600 with 1MHz clock (Data Sheet 15.3.13)
	UCA0BR1 = 0; // Set baud rate to 9600 with 1MHz clock
	UCA0MCTL = UCBRS_1; // Modulation UCBRSx = 1
	// 9600 @ 8MHz
	//UCA0BR0 = 64;
	//UCA0BR1 = 3;
	//UCA0MCTL = UCBRS_2;
	UCA0CTL1 &= ~UCSWRST; // Initialize USCI state machine
	IE2 |= UCA0RXIE; // Enable USCI_A0 RX interrupt
	
	// Configure XTAL for 32.768kHz
	P2DIR |= BIT7; // TODO: BIT 6 = 0 bit 0,1,2,3,4,5 -> 1 (OUT)
	BCSCTL3 |= XCAP_3;
	
	// Timer A 0 is used for the RTC
	TA0CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;
	//TA0CTL = TASSEL_1 | ID_2 | MC_1 | TACLR;
	TA0CCR0 = 32767;
	TA0CCTL0 = CCIE;

	// Timer A 1 is used for callbacks
	TA1CTL = TASSEL_1 | ID_0 | MC_1 | TACLR;
	TA1CCR0 = 32767;
	//TA1CCR0 = 16383;
	TA1CCTL0 = CCIE;

	// Initialise UCB0 for I2C
	P1SEL |= BIT6 | BIT7;
	P1SEL2 |= BIT6 | BIT7;
	UCB0CTL1 |= UCSWRST;
	UCB0CTL0 = UCMST | UCMODE_3 | UCSYNC;
	UCB0CTL1 = UCSSEL_2 | UCSWRST;
	UCB0BR0 = 12;
	//UCB0BR0 = 96;
	UCB0BR1 = 0;
	/*  MCP3422:   1  1  0  1 A2 A1 A0 RW (RW Bit not in UCB0I2CSA) */
	//UCB0I2CSA = 0x68;
	/* MCP23017:   0  1  0  0 A2 A1 A0 RW (RW Bit not in UCB0I2CSA) */
	//UCB0I2CSA = 0x20;
	/* SSD1306 OLED:  0  1  1  1  1  0  SA0 RW (RW Bit not in UCB0I2CSA) */
	UCB0I2CSA = 0x3c;
	UCB0CTL1 &= ~UCSWRST;
	//IE2 |= UCB0RXIE;
	IE2 |= UCB0TXIE;
	
	rtc_initialise();
	timer_initialise();
	uart_init();

	//__eint();
	__bis_SR_register(GIE);

	/* To enter LPM3 one should disable the serial interfaces        *
	 * Serial interfaces can be disabled once their work is complete *
	 * also, the GPIO interrupts should be enabled for buttons       */
	//__bis_SR_register(LMP3_bits);
	//__bis_SR_register(GIE | CPUOFF | SCG0 | SCG1);
	
	timer_callback(10, led_toggle);
	timer_callback(1, oled_initialise);

	while (1) {
#if 0
		if (uart_data == '1') {
			UCB0CTL1 |= UCSWRST;
			UCB0I2CSA = 0x68;
			UCB0CTL1 &= ~UCSWRST;
			UCB0CTL1 |= UCTR | UCTXSTT;
			while (!(IFG2 & UCB0TXIFG));
			/* 1 = Initiate Conversion, 0 1 = Channel 2, 0 = One Shot Conversion, 1 1 = 18 Bits (3.75 sps), 0 0 = x1 Gain */
			UCB0TXBUF = 0xAC;
			while (!(IFG2 & UCB0TXIFG));

			UCB0CTL1 |= UCTXSTP;
			IFG2 &= ~UCB0TXIFG;
			UCA0TXBUF = 's';
			uart_data = 0;
		}
		if (uart_data == '2') {
			UCA0TXBUF = 'x';
			uart_data = 0;
		}
		if (uart_data == '3') {
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
		if (uart_data == '4') {
			UCB0I2CSA = 0x3c;
			ssd1306_initialise();
			ssd1306_clear();
			ssd1306_command_1(SSD_Display_On);
			//ssd1306_command_2(SSD1306_MEMORYMODE, HORIZONTAL_MODE);
			//ssd1306_command_1(SSD_Deactivate_Scroll);
			//ssd1306_command_1(SSD_Display_On);
			UCA0TXBUF = 'i';
			uart_data = 0;
		}
		if (uart_data == '5') {
			// Switch the screen into Off mode
			UCB0I2CSA = 0x3c;
			ssd1306_command_1(SSD_Display_Off);
			ssd1306_command_2(SSD1306_CHARGEPUMP, 0x10);
			uart_data = 0;
		}
		if (uart_data == '6') {
			// Turn the screen on
			//UCB0I2CSA = 0x3c;
			ssd1306_command_2(SSD1306_CHARGEPUMP, 0x14);
			ssd1306_command_1(SSD_Display_On);
			//timer_callback(2, toggle_led);
			uart_data = 0;
		}
		if (uart_data == '7') {
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
			write_time();
			ssd1306_writestringz(8, 32, txbuf);

			uart_data = 0;
		}
		if (uart_data == '8') {
			UCB0CTL1 |= UCSWRST;
			UCB0I2CSA = 0x3c;
			UCB0CTL1 &= ~UCSWRST;
			ssd1306_command_1(SSD_Inverse_Display);
			UCA0TXBUF = 'r';
			uart_data = 0;
		}
		if (uart_data == '9') {
			ssd1306_command_1(SSD1306_Normal_Display);
			UCA0TXBUF = 'n';
			uart_data = 0;
		}
		if (uart_data == '0') {
			ssd1306_command_2(SSD_Set_ContrastLevel, 0x0F);
			UCA0TXBUF = 'o';
			uart_data = 0;
		}
#endif /* 0 */
		/*
		if (uart_data == '') {
			ssd1306_command_2(SSD_Set_ContrastLevel, 0x01);
			UCA0TXBUF = 'p';
			uart_data = 0;
		}
		if (uart_data == 'Q') {
			ssd1306_command_2(SSD_Set_ContrastLevel, 0x7f);
			UCA0TXBUF = 'q';
			uart_data = 0;
		}
		*/
		if (uart_ctx.flags & UART_HASRECEIVED) {
			int c;
			c = *(uart_getlinebuf());
			if (c == 'U') {
				if (!timer_is_present(oled_displaytime)) {
					timer_callback(1, oled_displaytime);
				}
			}
			if (c == 'u') {
				timer_is_present_remove(oled_displaytime);
			}
			if (c == 'o') {
				ssd1306_command_1(SSD_Display_Off);
				ssd1306_command_2(SSD1306_CHARGEPUMP, 0x10);
			}
			if (c == 'O') {
				ssd1306_command_2(SSD1306_CHARGEPUMP, 0x14);
				ssd1306_command_1(SSD_Display_On);
			}
			if (c == 'l') {
				P2OUT &= ~BIT0;
			}
			if (c == 'L') {
				P2OUT |= BIT0;
			}
			if (c == 'T') {
				P2OUT ^= BIT0;
			}
			if (c == 'F') {
				if (!timer_is_present(led_toggle)) {
					timer_callback(10, led_toggle);
				}
			}
			if (c == 'f') {
				timer_is_present_remove(led_toggle);
			}
			//if (c == 'G') {
			//	frame_buffer[frame_buffer_idx] = 'f';
			//	frame_buffer_idx ++;
			//}
			uart_ctx.flags &= ~(UART_HASRECEIVED);
		}
		timer_docallbacks();
		__bic_SR_register(GIE);
		// Check for pending flags whilst no more can arrive!
		if ((uart_ctx.flags & UART_HASRECEIVED) == 0) {
			__bis_SR_register(GIE | CPUOFF | SCG0 | SCG1);
		}
		__bis_SR_register(GIE);
	}
}

void Port1_ISR(void) __attribute__( ( interrupt( PORT1_VECTOR ) ) );
void Port1_ISR(void) {
	P1IFG &= ~(BIT5);
	__bic_SR_register_on_exit(CPUOFF);
}

void USCI0RX_ISR(void) __attribute__( ( interrupt( USCIAB0RX_VECTOR ) ) );
void USCI0RX_ISR(void) {
	if (IFG2 & UCA0RXIFG) {
		// Kick off the CPU on a CR (\r) or (\n)
		int hwuart_byte;
		hwuart_byte = UCA0RXBUF;
		if (uart_ctx.rxlinebuf_pos < UART_LINEBUFSZ) {
			if (hwuart_byte == '\r' || hwuart_byte == '\n') {
				if (uart_ctx.rxlinebuf_pos != 0) {
					uart_ctx.rxlinebuf[uart_ctx.rxlinebuf_pos] = '\0';
					if (uart_ctx.rxlinebuf == uart_ctx.rxlinebuf_a) {
						uart_ctx.rxlinebuf = uart_ctx.rxlinebuf_b;
					} else {
						uart_ctx.rxlinebuf = uart_ctx.rxlinebuf_a;
					}
					uart_ctx.rxlinebuf_pos = 0;
					uart_ctx.flags |= UART_HASRECEIVED;
					__bic_SR_register_on_exit(CPUOFF);
				}
			} else {
				uart_ctx.rxlinebuf[uart_ctx.rxlinebuf_pos] = hwuart_byte;
				uart_ctx.rxlinebuf_pos ++;
			}
		} else {
			uart_ctx.rxlinebuf_pos = 0;
		}
	}
}

void USCI0TX_ISR(void) __attribute__ ( ( interrupt( USCIAB0TX_VECTOR ) ) );
void USCI0TX_ISR(void) {
	if (IFG2 & UCA0TXIFG) {
		/*
		if (txctr--) {
			UCB0TXBUF = 0x90;
		} else {
			UCB0CTL1 |= UCTXSTP;
			IFG2 &= ~UCB0TXIFG;
		}
		*/
	}
	if (IFG2 & UCB0TXIFG) {
		if (i2c_ctx.txbuf_pos < i2c_ctx.txbuf_sz) {
			UCB0TXBUF = i2c_ctx.txbuf[i2c_ctx.txbuf_pos];
			i2c_ctx.txbuf_pos ++;
		} else
		if (i2c_ctx.txbuf_pos == i2c_ctx.txbuf_sz) {
			UCB0CTL1 |= UCTXSTP;
			IFG2 &= ~UCB0TXIFG;
			i2c_ctx.txbuf_pos ++;
		} else {
			P2OUT |= BIT0;
		}
	}
	if (IFG2 & UCB0RXIFG) {
	}
}

