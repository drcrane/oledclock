#ifndef CLOCKFACE_H
#define CLOCKFACE_H

#include <stdint.h>

#define CLOCKFACE_DRAWSECOND
#define CLOCKFACE_POINTS 16

struct point {
	uint8_t x;
	uint8_t y;
};

extern const struct point clockface_qtr[];

uint16_t clockface_getminutequadrant(uint16_t minute);
uint16_t clockface_gethourquadrant(uint16_t hour);

void clockface_drawtopright();
void clockface_drawbottomright();
void clockface_drawbottomleft();
void clockface_drawtopleft();

#endif /* CLOCKFACE_H */

