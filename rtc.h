#ifndef RTC_H
#define RTC_H

#include <stdint.h>

struct rtc_context {
	int16_t year;
	int16_t month;
	int16_t day;
	int16_t hour;
	int16_t minute;
	int16_t second;
};

extern struct rtc_context rtc_ctx;

void rtc_initialise();

#endif /* RTC_H */

