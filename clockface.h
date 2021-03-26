#ifndef CLOCKFACE_H
#define CLOCKFACE_H

#include <stdint.h>

#define CLOCKFACE_POINTS 16

struct point {
	uint8_t x;
	uint8_t y;
};

extern const struct point clockface_qtr[];

#endif /* CLOCKFACE_H */

