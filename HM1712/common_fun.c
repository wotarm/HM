#include "common_fun.h"
void delayus(char t)
{
	for(;t<=0;t--)
	{
	}
}
 unsigned char crc8l(unsigned char *ptr, unsigned char len)
{                              //ptr data pointer£¬len is the data length
	unsigned  char j, crc = 0;
	while(len--)   
	{           //loop by the data number
		for(j = 0x80; j != 0; j >>= 1)   
		{ //from high to low position
			if((crc & 0x80) != 0) 
			{
				crc <<= 1;  
				crc ^= 0x81; 
			}
			else  
				crc <<= 1;
			if((*ptr & j) != 0 )  
				crc ^= 0x81;//process the data
		}
		ptr++;                   //next data
	}
	return(crc);
}
//-------------------------------------------
//name:		hexstr2ch
//function:	convert a hex string to a real hex value string
//para:		hex - the source hex string, ch - destination string, store the hex value, len - the value length 
//-------------------------------------------
 void hexstr2ch(char* hex, char* ch, char len)
{
	unsigned char idx;
	unsigned char odd =0;
	odd = strlen(hex)%2;
	for(idx = 0; idx < len; idx++)
	{
		if(odd==1 && idx ==0)
		{
			if(hex[idx * 2]>= '0' && hex[idx * 2] <= '9')
			{
				ch[idx] = (hex[idx * 2] - '0') ;
			}
			else if(hex[idx * 2] >='A' && hex[idx * 2] <= 'F')
			{
				ch[idx] = (hex[idx * 2] - 'A'+10) ;
			}
			else if(hex[idx * 2] >='a' && hex[idx * 2] <= 'f')
			{
				ch[idx] = (hex[idx * 2] - 'a'+10) ;
			}
			continue;
		}
		if(hex[idx * 2 - odd]>= '0' && hex[idx * 2 -odd] <= '9')
		{
			ch[idx] = (hex[idx * 2- odd] - '0') * 16;
		}
		else if(hex[idx * 2 -odd] >='A' && hex[idx * 2 -odd] <= 'F')
		{
			ch[idx] = (hex[idx * 2- odd] - 'A'+10) * 16;
		}
		else if(hex[idx * 2-odd] >='a' && hex[idx * 2-odd] <= 'f')
		{
			ch[idx] = (hex[idx * 2- odd] - 'a'+10) * 16;
		}
		if(hex[idx * 2 +1-odd]>= '0' && hex[idx * 2 +1-odd] <= '9')
		{
			ch[idx] += (hex[idx * 2+1- odd] - '0');
		}
		else if(hex[idx * 2 +1-odd] >='A' && hex[idx * 2+1-odd] <= 'F')
		{
			ch[idx] += (hex[idx * 2+1- odd] - 'A'+10);
		}
		else if(hex[idx * 2 +1-odd] >='a' && hex[idx * 2+1-odd] <= 'f')
		{
			ch[idx] += (hex[idx * 2+1- odd] - 'a'+10);
		}
	}
}
//-------------------------------------------
//name:		hex2str
//function:	convert a hex value to a hex string
//para:		hex - the source hex value string, string - destination string, print the hex number, len - the value length 
//-------------------------------------------
void hex2str(char* hex, char* str, char len)
{
	unsigned char idx;
	unsigned char temp;
	
	for(idx=0; idx < len; idx++)
	{
		temp = (hex[idx]>>4) & 0x0F;
		if(temp >= 0x0A)
		{
			str[idx*2] = 'A' + temp - 0x0A;
		}
		else
		{
			str[idx*2] = '0' + temp;
		}
		temp = hex[idx] & 0x0F;
		if(temp >= 0x0A)
		{
			str[idx*2+1] = 'A' + temp - 0x0A;
		}
		else
		{
			str[idx*2+1] = '0' + temp;
		}
	}
	
}
 void decstr2ch(char* dec, char* ch)
{
	unsigned int i = atoi(dec);
	ch[0] = i;
	ch[1] = i>>8;
}

unsigned int ch2dec(char* ch)
{
	unsigned int ret;
	ret = (unsigned char)ch[0];
	ret+= (unsigned char)ch[1]<<8;
	return ret;
}


void Delay(int iDelay)
{
	int i,j;
	for(i=0;i<iDelay;i++)
	{
		for(j=0;j<255;j++);
	}
}
void strLow2Cap(char* str)
{
	char idx;
	for(idx = 0; idx < strlen(str); idx++)
	{
		if(str[idx] == 0x0d)return;
		if(str[idx]>='a' && str[idx]<='z')
		{
			str[idx]-=32;
		}
	}
}

int pow2(char x)
{
	char idx = 0;
	int ret= 1;
	for(; idx<x; idx++)
	{
		ret *= 2;
	}
	return ret;
}

void reverse(char* s)
{
	char t;
	char idx = 0;
	for(;idx<strlen(s)/2;idx++)
	{
		t = *(s+strlen(s) -idx -1);
		*(s+strlen(s) -idx -1) = *(s+idx);
		*(s+idx) = t;
	}
}
//reverse the hex value
void hex_reverse(char* s, char len)
{
	char t;
	char idx = 0;
	for(;idx<len;idx++)
	{
		t = *(s+idx);
		*(s+idx) = (((t&0xF0)>>4)&0x0F)+ (((t&0x0F)<<4)&0xF0);
	}
}
