#include <msp430.h>
#include <stdint.h>
#include <stddef.h>
#include "rtc.h"

struct rtc_context rtc_ctx;

void rtc_initialise() {
	rtc_ctx.year = 2020;
	rtc_ctx.month = 1;
	rtc_ctx.day = 1;
	rtc_ctx.hour = 23;
	rtc_ctx.minute = 50;
	rtc_ctx.second = 0;
}

void Timer0_A0(void) __attribute__((interrupt(TIMER0_A0_VECTOR)));
void Timer0_A0(void) {
	rtc_ctx.second ++;
	if (rtc_ctx.second == 60) {
		rtc_ctx.minute ++;
		rtc_ctx.second = 0;
		if (rtc_ctx.minute == 60) {
			rtc_ctx.hour ++;
			rtc_ctx.minute = 0;
			if (rtc_ctx.hour == 24) {
				rtc_ctx.hour = 0;
			}
		}
	}
}

