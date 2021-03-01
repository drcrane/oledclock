#ifndef __UTILITYFN_H__
#define __UTILITYFN_H__

void Utility_delay(unsigned short int loops);

unsigned long int Utility_hexToInt(const char* s, int count);
void Utility_intToHex(char* dst, const void* ptr, int count);

#endif // __UTILITYFN_H__
