#ifndef __SSD1306_H__
#define __SSD1306_H__


//#define SSD_Command_Mode	0x80 	/* Co is 1 and DC bit are 0 				 */
#define SSD_Command_Mode	0x00 	/* Co and DC bit are 0 				 */
#define SSD_Data_Mode		0x40	/* Co bit is 0 and DC bit is 1 */

#define SSD_Inverse_Display	0xA7

#define SSD_Display_Off		0xAE
#define SSD_Display_On		0xAF

#define SSD_Set_ContrastLevel	0x81

#define SSD_External_Vcc	0x01
#define SSD_Internal_Vcc	0x02

#define SSD_Activate_Scroll	0x2F
#define SSD_Deactivate_Scroll	0x2E

#define Scroll_Left		0x00
#define Scroll_Right		0x01

#define Scroll_2Frames		0x07
#define Scroll_3Frames		0x04
#define Scroll_4Frames		0x05
#define Scroll_5Frames		0x00
#define Scroll_25Frames		0x06
#define Scroll_64Frames		0x01
#define Scroll_128Frames	0x02
#define Scroll_256Frames	0x03

// Page mode on reset
#define HORIZONTAL_MODE					00
#define VERTICAL_MODE					01
#define PAGE_MODE					02

/*=========================================================================
    SSD1306 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
=========================================================================*/
#define SSD1306_DISPLAYALLON_RESUME			0xA4
#define SSD1306_DISPLAYALLON 				0xA5

#define SSD1306_Normal_Display				0xA6

#define SSD1306_SETDISPLAYOFFSET 			0xD3
#define SSD1306_SETCOMPINS 				0xDA
#define SSD1306_SETVCOMDETECT 				0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 			0xD5
#define SSD1306_SETPRECHARGE 				0xD9
#define SSD1306_SETMULTIPLEX 				0xA8
#define SSD1306_SETLOWCOLUMN 				0x00
#define SSD1306_SETHIGHCOLUMN 				0x10
#define SSD1306_SETSTARTLINE 				0x40
#define SSD1306_SETPAGE					0xB0
#define SSD1306_MEMORYMODE 				0x20
#define SSD1306_COMSCANINC 				0xC0
#define SSD1306_COMSCANDEC 				0xC8
#define SSD1306_SEGREMAP 				0xA0
#define SSD1306_CHARGEPUMP 				0x8D

// Scrolling #defines
#define SSD1306_SET_VERTICAL_SCROLL_AREA 		0xA3
#define SSD1306_RIGHT_HORIZONTAL_SCROLL 		0x26
#define SSD1306_LEFT_HORIZONTAL_SCROLL 			0x27
#define SSD1306_VERTICAL_AND_RIGHT_HORIZONTAL_SCROLL 	0x29
#define SSD1306_VERTICAL_AND_LEFT_HORIZONTAL_SCROLL	0x2A

/*=========================================================================
    SSD1308 Displays
    -----------------------------------------------------------------------
    The driver is used in multiple displays (128x64, 128x32, etc.).
=========================================================================*/
#define SSD1308_Normal_Display	0xA6

void ssd1306_command_1(int cmd);
void ssd1306_command_2(int cmd, int arg);
void ssd1306_command_3(int cmd, int arg1, int arg2);
void ssd1306_data_write(int count, char * buf);
void ssd1306_writestringz(int posx, int posy, char * strz);
void ssd1306_clear();

#endif // __SSD1306_H__

