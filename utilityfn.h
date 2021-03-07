#ifndef __UTILITYFN_H__
#define __UTILITYFN_H__

void Utility_delay(unsigned short int loops);

unsigned long int Utility_hexToInt(const char* s, int count);
void Utility_intToHex(char* dst, const void* ptr, int count);
void Utility_reverse(char * str, int sz);
int Utility_intToAPadded(char * ptr, int value, int radix, int padding);

#endif // __UTILITYFN_H__
