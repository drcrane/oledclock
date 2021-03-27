#include "clockface.h"
#include "ssd1306.h"
#include "rtc.h"

const struct point clockface_qtr[] = {{0,0},{3,0},{6,1},{10,2},{13,3},{16,4},{18,6},{21,8},{23,10},{25,13},{27,16},{28,18},{29,21},{30,25},{31,28},{31,31}};
const struct point clockface_hourlocations[] = {{0,16},{8,18},{13,24},{15,31}};

uint16_t clockface_getminutequadrant(uint16_t minute) {
	if (minute <= 15) {
		// Top Right
		return 1;
	} else
	if (minute <= 30) {
		// Bottom Right
		return 2;
	} else
	if (minute <= 44) {
		// Bottom Left
		return 4;
	} else {
		// Top Left
		return 8;
	}
}

uint16_t clockface_gethourquadrant(uint16_t hour) {
	if (hour >= 12) { hour = hour - 12; }
	if (hour <= 3) {
		return 1;
	} else
	if (hour <= 6) {
		return 2;
	} else
	if (hour <= 9) {
		return 4;
	} else {
		return 8;
	}
}

void clockface_drawtopright() {
	int idx;
	int x1, y1;
	ssd1306_fb_top_x = 32;
	ssd1306_fb_top_y = 0;
	ssd1306_clearframebuffer();
	// Draw Right Top Quarter
	// Draw from 32,0 to 64,32
	x1 = clockface_qtr[0].x;
	y1 = clockface_qtr[0].y;
	for (idx = 1; idx <= 15; idx++) {
		ssd1306_draw_line(x1, y1, clockface_qtr[idx].x, clockface_qtr[idx].y);
		x1 = clockface_qtr[idx].x;
		y1 = clockface_qtr[idx].y;
	}
	#ifdef CLOCKFACE_DRAWSECOND
	idx = rtc_ctx.second;
	if (clockface_getminutequadrant(idx) & 1) {
		ssd1306_draw_line(0, 31, clockface_qtr[idx].x, clockface_qtr[idx].y);
	}
	#endif /* CLOCKFACE_DRAWSECOND */
	idx = rtc_ctx.minute;
	if (clockface_getminutequadrant(idx) & 1) {
		ssd1306_draw_line(0, 31, clockface_qtr[idx].x, clockface_qtr[idx].y);
	}
	idx = rtc_ctx.hour;
	if (idx >= 12) { idx -= 12; }
	if (clockface_gethourquadrant(idx) & 1) {
		ssd1306_draw_line(0, 31, clockface_hourlocations[idx].x, clockface_hourlocations[idx].y);
	}
	ssd1306_writeframebuffer();
}

void clockface_drawbottomright() {
	int idx;
	int x1, y1;
	ssd1306_fb_top_x = 32;
	ssd1306_fb_top_y = 32;
	ssd1306_clearframebuffer();
	// Draw Bottom Right Quarter
	// Draw from 64,32 to 32,64
	x1 = clockface_qtr[15].x;
	y1 = 31-clockface_qtr[15].y;
	for (idx = 14; idx >= 0; idx--) {
		ssd1306_draw_line(x1, y1, clockface_qtr[idx].x, 31-clockface_qtr[idx].y);
		x1 = clockface_qtr[idx].x;
		y1 = 31-clockface_qtr[idx].y;
	}
	#ifdef CLOCKFACE_DRAWSECOND
	idx = rtc_ctx.second;
	if (clockface_getminutequadrant(idx) & 2) {
		idx = 15 - (idx - 15);
		ssd1306_draw_line(0, 0, clockface_qtr[idx].x, 31-clockface_qtr[idx].y);
	}
	#endif /* CLOCKFACE_DRAWSECOND */
	idx = rtc_ctx.minute;
	if (clockface_getminutequadrant(idx) & 2) {
		idx = 15 - (idx - 15);
		ssd1306_draw_line(0, 0, clockface_qtr[idx].x, 31-clockface_qtr[idx].y);
	}
	idx = rtc_ctx.hour;
	if (idx >= 12) { idx -= 12; }
	if (clockface_gethourquadrant(idx) & 2) {
		idx = 3 - (idx - 3);
		ssd1306_draw_line(0, 0, clockface_hourlocations[idx].x, 31-clockface_hourlocations[idx].y);
	}
	ssd1306_writeframebuffer();
}

void clockface_drawbottomleft() {
	int idx;
	int x1, y1;
	ssd1306_fb_top_x = 0;
	ssd1306_fb_top_y = 32;
	ssd1306_clearframebuffer();
	// Draw Bottom Left Quarter
	// Draw from 32,64 to 0,32
	x1 = 32-clockface_qtr[0].x;
	y1 = 31-clockface_qtr[0].y;
	for (idx = 0; idx <= 15; idx++) {
		ssd1306_draw_line(x1, y1, 32-clockface_qtr[idx].x, 31-clockface_qtr[idx].y);
		x1 = 32-clockface_qtr[idx].x;
		y1 = 31-clockface_qtr[idx].y;
	}
	#ifdef CLOCKFACE_DRAWSECOND
	idx = rtc_ctx.second;
	if (clockface_getminutequadrant(idx) & 4) {
		idx = idx - 30;
		ssd1306_draw_line(32, 0, 32-clockface_qtr[idx].x, 31-clockface_qtr[idx].y);
	}
	#endif /* CLOCKFACE_DRAWSECOND */
	idx = rtc_ctx.minute;
	if (clockface_getminutequadrant(idx) & 4) {
		idx = idx - 30;
		ssd1306_draw_line(32, 0, 32-clockface_qtr[idx].x, 31-clockface_qtr[idx].y);
	}
	idx = rtc_ctx.hour;
	if (idx >= 12) { idx -= 12; }
	if (clockface_gethourquadrant(idx) & 4) {
		idx -= 6;
		ssd1306_draw_line(32, 0, 32-clockface_hourlocations[idx].x, 31-clockface_hourlocations[idx].y);
	}
	ssd1306_writeframebuffer();
}

void clockface_drawtopleft() {
	int idx;
	int x1, y1;
	ssd1306_fb_top_x = 0;
	ssd1306_fb_top_y = 0;
	// Draw Left Top Quarter
	// Draw From 0,32 to 32,0
	ssd1306_clearframebuffer();
	x1 = 32-clockface_qtr[15].x;
	y1 = clockface_qtr[15].y;
	for (idx = 14; idx >= 0; idx--) {
		ssd1306_draw_line(x1, y1, 32-clockface_qtr[idx].x, clockface_qtr[idx].y);
		x1 = 32-clockface_qtr[idx].x;
		y1 = clockface_qtr[idx].y;
	}
	#ifdef CLOCKFACE_DRAWSECOND
	idx = rtc_ctx.second;
	if (clockface_getminutequadrant(idx) & 8) {
		idx = 15 - (idx - 45);
		ssd1306_draw_line(31, 31, 32-clockface_qtr[idx].x, clockface_qtr[idx].y);
	}
	#endif /* CLOCKFACE_DRAWSECOND */
	idx = rtc_ctx.minute;
	if (clockface_getminutequadrant(idx) & 8) {
		idx = 15 - (idx - 45);
		ssd1306_draw_line(31, 31, 32-clockface_qtr[idx].x, clockface_qtr[idx].y);
	}
	idx = rtc_ctx.hour;
	if (idx >= 12) { idx -= 12; }
	if (clockface_gethourquadrant(idx) & 8) {
		idx = 3 - (idx - 9);
		ssd1306_draw_line(31, 31, 32-clockface_hourlocations[idx].x, clockface_hourlocations[idx].y);
	}
	ssd1306_writeframebuffer();
}

