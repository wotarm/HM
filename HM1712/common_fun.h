#ifndef __COMMON_FUN_H__
#define __COMMON_FUN_H__
#include <stdlib.h>
#include <string.h>

unsigned char crc8l(unsigned char *ptr, unsigned char len);
void hex2str(char* hex, char* str, char len);
void hexstr2ch(char* hex, char* ch, char len);
void decstr2ch(char* dec, char* ch);
unsigned int ch2dec(char* ch);
void Delay(int iDelay);
void delayus(char t);
void strLow2Cap(char* str);
void reverse(char* s);
void hex_reverse(char* s, char len);

int pow2(char x);
#endif
