#include <stdio.h>
#include <math.h>

/*
 * gcc -lm -o clockface clockface.c
 */

char buf[8192];

int main() {
	double angle;
	int pos;
	for (pos = 0; pos < 16; pos++) {
		angle = (90.0 - (pos * 6.0)) * (M_PI / 180.0);
		//printf("angle = %d ", 90 - (pos * 6));
		//printf("x = %0.3f ", cos(angle)*32.0);
		//printf("y = %0.3f\n", 32.0-(sin(angle)*32.0));
		sprintf(buf, "{%0.0f,%0.0f},", cos(angle)*31.0, 31.0-(sin(angle)*31.0));
		printf("%s", buf);
	}
}

