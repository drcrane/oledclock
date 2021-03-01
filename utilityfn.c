#include "utilityfn.h"
#include <msp430.h>

void Utility_delay(unsigned short int loops) {
	while (loops--) {
		__nop();
	}
}

/**
 * convert the hex value supplied in s into a 32bit integer, terminating on count or \0
 */
unsigned long int Utility_hexToInt(const char* s, int count) {
	unsigned long int ret = 0;
	char c;
	while ((c = *s++) != '\0' && count--) {
		int n = 0;
		if('0' <= c && c <= '9') { n = c-'0'; }
		else if('a' <= c && c <= 'f') { n = 10 + c - 'a'; }
		else if('A' <= c && c <= 'F') { n = 10 + c - 'A'; }
		ret = n + (ret << 4);
	}
	return ret;
}

/**
 * translate count bytes from ptr into ascii hex and store them in the dst array.
 */
void Utility_intToHex(char* dst, const void* ptr, int count) {
	int i;
	const unsigned char* cptr = (const unsigned char*)ptr;
	const char hexchars[] = "0123456789ABCDEF";
	unsigned int j = (count << 1);
	dst[j] = 0;
	for (i = 0; i < count; i++) {
		dst[--j] = hexchars[((cptr[i]) & 0xf)];
		dst[--j] = hexchars[((cptr[i] >> 4) & 0xf)];
	}
}

