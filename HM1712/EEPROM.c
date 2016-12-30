
#include "EEPROM.h"
#include "api.h"
#include "common_fun.h"
#include "mqx.h"
char temp_str[DATA_REGION_SIZE];
//char shield_region[DATA_REGION_NUM];
//char active_region = 0;
//char active_default_region = 0;

char ch[2];





void EEPROM_init()
{
	ifinit();
}
void eraseEEPROM()
{

}

////////////////////////////////////////////////////////////////////////////////
// write part data of a data ragion
//
// shift: the part data start shift address 
// len:the part data length
// str:the part data
// 
////////////////////////////////////////////////////////////////////////////////
void writeStrEEPROM(char* str, char shift, char len)
{

	WriteFlexmem(shift, str, len);
}

void writeEEPROM(char* str)
{
	WriteFlexmem(0, str, DATA_REGION_SIZE);
}


void readEEPROM(char* str)
{
	ReadFlexmem(str, 0, DATA_REGION_SIZE);
	//ReadFlash(str, active_region * DATA_REGION_SIZE + DATA_REGION0_ADDR, DATA_REGION_SIZE, 0);
}



void readStrEEPROM(char* str, char shift, char len)
{
	ReadFlexmem(str, shift, len);
	//ReadFlash(str, active_region * DATA_REGION_SIZE + DATA_REGION0_ADDR + shift, len, 0);
}

char getStatusChar(uint32_t addr)
{
	return ReadFlashByte(addr, 0);
}

char checkIfWrtRt(char* str, uint32_t addr)
{
	char read_buf[DATA_REGION_SIZE];
	memset(read_buf, 0x0, DATA_REGION_SIZE);
	ReadFlash(read_buf, addr, DATA_REGION_SIZE, 0);
	if(strcmp((char *)read_buf, (char *)str)==0)
		return 1;
	else
		return 0;
}
void saveasDefault()
{

}

void readDevCanID(char* str)
{
	readStrEEPROM(str, CAN_ID, 1);
}
void writeDevCanID(char* str)
{
	writeStrEEPROM(str, CAN_ID, 1);
}

void readPreCur(char* str)
{
	readStrEEPROM(str, PRE_CUR, 2);
}
void writePreCur(char* str)
{
	writeStrEEPROM(str, PRE_CUR, 2);
}


void readPreTem(char* str)
{
	readStrEEPROM(str, PRE_TEM, 2);
}
void writePreTem(char* str)
{
	writeStrEEPROM(str, PRE_TEM, 2);
}
void writeArgA(char* str)
{
	writeStrEEPROM(str, PRE_ArgA, 4);
}
void writeArgB(char* str)
{
	writeStrEEPROM(str, PRE_ArgB, 4);
}
void readArgA(char* str)
{
	readStrEEPROM(str, PRE_ArgA, 4);
}
void readArgB(char* str)
{
	readStrEEPROM(str, PRE_ArgB, 4);
}
float readArgAfromFlash(void)
{
	float result;
	char * c = (char*)&result;
	readArgA(c);
	return result;
}
float readArgBfromFlash(void)
{
	float result;
	char * c = (char*)&result;
	readArgB(c);
	return result;
}
void writeArgAFlash(float tem)
{
	char *c = (char*)&tem;
	writeArgA(c);
}
void writeArgBFlash(float tem)
{
	char *c = (char*)&tem;
	writeArgB(c);
}
void readKp(char* str)
{
	readStrEEPROM(str, KP, 2);
}
void writeKp(char* str)
{
	writeStrEEPROM(str, KP, 2);
}

void readKi(char* str)
{
	readStrEEPROM(str, KI, 2);
}
void writeKi(char* str)
{
	writeStrEEPROM(str, KI, 2);
}

void readKd(char* str)
{
	readStrEEPROM(str, KD, 2);
}
void writeKd(char* str)
{
	writeStrEEPROM(str, KD, 2);
}




void readTemAdjA(char* str)
{
	readStrEEPROM(str, TEMADJ_A, 2);
}
void writeTemAdjA(char* str)
{
	writeStrEEPROM(str, TEMADJ_A, 2);
}

void readTemAdjB(char* str)
{
	readStrEEPROM(str, TEMADJ_B, 2);
}
void writeTemAdjB(char* str)
{
	writeStrEEPROM(str, TEMADJ_B, 2);
}



void readMAC(char* str)
{
	readStrEEPROM(str, GS_MAC, 4);
}
void writeMAC(char* str)
{
	writeStrEEPROM(str, GS_MAC, 4);
}
/*
void readCheckCode(char* str)
{
	readStrEEPROM(str, CHKCODE, 2);
}
void writeCheckCode(char* str)
{
	writeStrEEPROM(str, CHKCODE, 2);
}
*/
void writeLanguage(char* str)
{
	writeStrEEPROM(str, LANGUAGE, 1);
}

void writeLanguagetoFlash(char lan)
{
	writeLanguage(&lan);
}
void readLanguage(char* str)
{
	readStrEEPROM(str, LANGUAGE, 1);
}
char readLanguagefromFlash(void)
{
	readLanguage(ch);
	return ch[0];
}
void writeTEC(char * str)
{
	writeStrEEPROM(str,TECStrength,1);
}
void writeTECStrengthtoFlash(char tec)
{
	writeTEC(&tec);
}
void readTECStrength(char *str)
{
	readStrEEPROM(str,TECStrength,1);
}
char readTECStrengthfroomFlash(void)
{
	readTECStrength(ch);
	return ch[0];
}
void writeAutorun(char* str)
{
	writeStrEEPROM(str, AUTORUN, 1);
}
void writeAutoruntoFlash(char lan)
{
	writeAutorun(&lan);
}
void readAutorun(char* str)
{
	readStrEEPROM(str, AUTORUN, 1);
}
char readAutorunfromFlash(void)
{
	readAutorun(ch);
	return ch[0];
}
void writeMaxPower(char* str)
{
	writeStrEEPROM(str, MaxP, 2);
}
void writeMaxPowertoFlash(unsigned int tem)
{

	ch[0] = tem;
	ch[1] = tem>>8;
	writeMaxPower(ch);
}
void readMaxPower(char* str)
{
	readStrEEPROM(str, MaxP, 1);
}
void readMaxP(char* str)
{
	readStrEEPROM(str, MaxP, 2);
}
unsigned int readMaxPowerfromFlash(void)
{
	readMaxP(ch);
	return ch2dec(ch);
}

void readWorkMode(char* str)
{
	readStrEEPROM(str, WORKMODE, 1);
}
void writeWorkMode(char* str)
{
	writeStrEEPROM(str, WORKMODE, 1);
}
void readBW_Thermo(char* str)
{
	readStrEEPROM(str, W_THERMO, 1);
}
void writeBW_Thermo(char* str)
{
	writeStrEEPROM(str, W_THERMO, 1);
}

void ifinit()
{
	memset(temp_str, 0x0, DATA_REGION_SIZE);

	readEEPROM(temp_str);
	if(temp_str[CHKCODE] == 0x5A)
	{
		//if(temp_str[DATA_REGION_SIZE-1] != crc8l((unsigned char*)temp_str, DATA_REGION_SIZE - 2))
		//{
		//	initDefault();
		//}
		return;
	}
	initDefault();
}
char readIDfromFlash()
{
	readDevCanID(ch);
	return ch[0];
}
void writeIDtoFlash(char id)
{
	writeDevCanID(&id);
}
unsigned int readPreCurfromFlash()
{
	readPreCur(ch);
	return ch2dec(ch);
}
void writePreCurtoFlash(unsigned int cur)
{
	ch[0] = cur;
	ch[1] = cur>>8;
	writePreCur(ch);
}
unsigned int readPreTemfromFlash()
{
	readPreTem(ch);
	return ch2dec(ch);
}
void writePreTemtoFlash(unsigned int tem)
{
	ch[0] = tem;
	ch[1] = tem>>8;
	writePreTem(ch);
}

int readKpfromFlash()
{
	readKp(ch);
	return ch2dec(ch);
}
int readKifromFlash()
{
	readKi(ch);
	return ch2dec(ch);
}
int readKdfromFlash()
{
	readKd(ch);
	return ch2dec(ch);
}
void WriteSN(char * buf,char len)
{
	writeStrEEPROM(buf, SN, len);
}
void ReadSN(char * buf,char len)
{
	ReadFlexmem(buf, SN, len);
}
void WritePM(char * buf,char len)
{
	writeStrEEPROM(buf, PM, len);
}
void ReadPM(char * buf,char len)
{
	ReadFlexmem(buf, PM, len);
}
void WriteTM(char * buf,char len)
{
	writeStrEEPROM(buf, TM, len);
}
void ReadTM(char * buf,char len)
{
	ReadFlexmem(buf, TM, len);
}
void initDefault()
{
	memset(temp_str, 0x0, DATA_REGION_SIZE);

	/*
	temp_str[SDPOLAR] = SDPOLAR_DEFAULT;*/
	if(temp_str[CHKCODE]!=0x5A)
	{
		temp_str[CAN_ID] = CAN_ID_DEFAULT;
		temp_str[PRE_CUR] = C_DEFAULT;
		temp_str[PRE_CUR+1] = C_DEFAULT>>8;
		temp_str[PRE_TEM] = (char)T_DEFAULT;
		temp_str[PRE_TEM+1] = T_DEFAULT>>8;
		temp_str[KP] = (char)T_KPDEFAULT;
		temp_str[KP+1] = T_KPDEFAULT>>8;
		temp_str[KI] = T_KIDEFAULT;
		temp_str[KI+1] = T_KIDEFAULT>>8;
		temp_str[KD] = T_KDDEFAULT;
		temp_str[KD+1] = T_KDDEFAULT>>8;
		temp_str[GS_MAC] = (char)GS_MAC_DEFAULT ;
		temp_str[GS_MAC+1] = (char)(GS_MAC_DEFAULT >> 8);
		temp_str[GS_MAC+2] = (char)(GS_MAC_DEFAULT >>16);
		temp_str[GS_MAC+3] = GS_MAC_DEFAULT >> 24;
		temp_str[TEMADJ_A] = (char)T_PADEFAULT;
		temp_str[TEMADJ_A+1] = T_PADEFAULT>>8;
		temp_str[TEMADJ_B] = T_PBDEFAULT;
		temp_str[TEMADJ_B+1] = T_PBDEFAULT>>8;
		temp_str[WORKMODE]  = 1;
		temp_str[W_THERMO]	= 0;
	}
	temp_str[CHKCODE] = 0x5A;
//	temp_str[CHKCODE+1] = crc8l((unsigned char*)temp_str, DATA_REGION_SIZE - 2);
	writeEEPROM(temp_str);
}


void ReadFlashEEprom(unsigned char * buf,unsigned int address,unsigned int len)
{
	uint32_t wAddr = 0;
	unsigned char result = 0;
	wAddr = FLASHSctorNo*SECTOR_SIZE + address;
	while(len--){
		result =*(uint8_t*)wAddr++;
		buf[address++] = result;
	}
}
#define EEPROM_ADDR		0x14000000
unsigned char EEpromBuff[4096];
unsigned char ReadByteFromEEprom(unsigned int address)
{
	uint32_t wAddr = 0;
	unsigned char result = 0;
	#if FalshEEprom
		wAddr = EEPROM_ADDR + address;		
	#else		
		wAddr = FLASHSctorNo*SECTOR_SIZE + address;
  #endif
	result =*(uint8_t*)wAddr++;
	return result; 
}

unsigned char WriteByteFromEEprom(unsigned int address,unsigned char data)
{
	uint32_t wAddr = 0;
	unsigned char result = 0;
	#if FalshEEprom
		writeStrEEPROM(&data, address, 1);	
	#else		
		ReadFlashEEprom((unsigned char *)temp_str,0,DATA_REGION_SIZE);
		temp_str[address] = data;
		FLASH_EraseSector(FLASHSctorNo*SECTOR_SIZE);
		FLASH_WriteSector(FLASHSctorNo*SECTOR_SIZE, (unsigned char *)temp_str, SECTOR_SIZE);
  #endif
	result =*(uint8_t*)wAddr++;	
  return result; 
}

void WriteIntFromEEprom(unsigned int address,unsigned int data)
{
	#if FalshEEprom
		writeStrEEPROM(&data, address, 2);	
	#else		
		ReadFlashEEprom((unsigned char *)temp_str,0,DATA_REGION_SIZE);
		temp_str[address] = ((data>>8)&0xff);
		temp_str[address+1] = (data&0xff);
		FLASH_EraseSector(FLASHSctorNo*SECTOR_SIZE);
		FLASH_WriteSector(FLASHSctorNo*SECTOR_SIZE, (unsigned char *)temp_str, SECTOR_SIZE);
  #endif
  return ; 
}

void WriteFloatFromEEprom(unsigned int address,float data)
{
	char * p = (char *)&data;
	#if FalshEEprom
		writeStrEEPROM(&data, address, 4);	
	#else		
		ReadFlashEEprom((unsigned char *)temp_str,0,DATA_REGION_SIZE);
		for(int i=0; i<4;i++){
			temp_str[address+i] = *(p+3-i);
		}
		FLASH_EraseSector(FLASHSctorNo*SECTOR_SIZE);
		FLASH_WriteSector(FLASHSctorNo*SECTOR_SIZE, (unsigned char *)temp_str, SECTOR_SIZE);
  #endif
  return ; 
}

void Write4ByteIntFromEEprom(unsigned int address, int data)
{
	char * p = (char *)&data;
	#if FalshEEprom
		writeStrEEPROM(&data, address, 4);	
	#else		
		ReadFlashEEprom((unsigned char *)temp_str,0,DATA_REGION_SIZE);
		temp_str[address] = *p;
		temp_str[address] = *(p+1);
		temp_str[address] = *(p+2);
		temp_str[address] = *(p+3);
		FLASH_EraseSector(FLASHSctorNo*SECTOR_SIZE);
		FLASH_WriteSector(FLASHSctorNo*SECTOR_SIZE, (unsigned char *)temp_str, SECTOR_SIZE);
  #endif
  return ; 
}

unsigned int ReadIntFromEEprom(unsigned int address)
{
	uint32_t wAddr = 0;
	unsigned int result = 0;
	#if FalshEEprom
		wAddr = EEPROM_ADDR + address;		
	#else		
		wAddr = FLASHSctorNo*SECTOR_SIZE + address;
  #endif
	result = *(uint8_t*)(wAddr);
	result = (result<<8)+*(uint8_t*)(wAddr+1);
	return result; 
}

unsigned int Read4ByteIntFromEEprom(unsigned int address)
{
	uint32_t wAddr = 0;
	#if FalshEEprom
		wAddr = EEPROM_ADDR + address;		
	#else		
		wAddr = FLASHSctorNo*SECTOR_SIZE + address;
  #endif
	union QFLOAT {unsigned int result;unsigned char c[4];} udata;
	for(int i=3;i>=0;i--){
		udata.c[i] = *(uint8_t*)wAddr++;
	}
  return udata.result; 
}

float ReadFloatFromEEprom(unsigned int address)
{
	uint32_t wAddr = 0;
	union QFLOAT {float result;unsigned char c[4];} udata;
	#if FalshEEprom
		wAddr = EEPROM_ADDR + address;		
	#else		
		wAddr = FLASHSctorNo*SECTOR_SIZE + address;
  #endif
	for(int i=3;i>=0;i--){
		udata.c[i] = *(uint8_t*)wAddr++;
	}
  return udata.result; 
}

unsigned long ReadULongFromEEprom(unsigned int address)
{
	uint32_t wAddr = 0;
	union QFLOAT {unsigned int result;unsigned char c[4];} udata;
	#if FalshEEprom
		wAddr = EEPROM_ADDR + address;		
	#else		
		wAddr = FLASHSctorNo*SECTOR_SIZE + address;
  #endif
	for(int i=3;i>=0;i--){
		udata.c[i] = *(uint8_t*)wAddr++;
	}
  return udata.result; 
}
void ReadStrFromEEprom(unsigned int address,char * buf,unsigned char length )
{
	uint32_t wAddr = 0;
	#if FalshEEprom
		wAddr = EEPROM_ADDR + address;		
	#else		
		wAddr = FLASHSctorNo*SECTOR_SIZE + address;
  #endif
	for(int i=0;i<length;i++){
		buf[i] = *(uint8_t*)wAddr++;
	}
  return ; 
}

void WriteStrFromEEprom(unsigned int address,char * buf,unsigned char length )
{
	#if FalshEEprom
		writeStrEEPROM(&data, address, 1);	
	#else		
		ReadFlashEEprom((unsigned char *)temp_str,0,DATA_REGION_SIZE);
		for(int i=0;i<length;i++){
			temp_str[address+i] = buf[i];
		}
		FLASH_EraseSector(FLASHSctorNo*SECTOR_SIZE);
		FLASH_WriteSector(FLASHSctorNo*SECTOR_SIZE, (unsigned char *)temp_str, SECTOR_SIZE);
  #endif

  return ; 
}

void EraseFPGASector(void)
{
	unsigned char no = 0;
	for(no = FPGAProgramSctorNo; no<127; no++)
		FLASH_EraseSector(no*SECTOR_SIZE);

}

void FlashEEprom_Init(void)
{
	ReadFlashEEprom((unsigned char *)temp_str,0,DATA_REGION_SIZE);
	if(temp_str[CHKCODE]!= CHECKHEAD)
	{
		temp_str[CAN_ID] = CAN_ID_DEFAULT;
		temp_str[PRE_CUR+1] = C_DEFAULT;
		temp_str[PRE_CUR] = C_DEFAULT>>8;
		temp_str[PRE_TEM+1] = (char)T_DEFAULT;
		temp_str[PRE_TEM] = T_DEFAULT>>8;
		temp_str[KP+1] = (char)T_KPDEFAULT;
		temp_str[KP] = T_KPDEFAULT>>8;
		temp_str[KI+1] = T_KIDEFAULT;
		temp_str[KI] = T_KIDEFAULT>>8;
		temp_str[KD+1] = T_KDDEFAULT;
		temp_str[KD] = T_KDDEFAULT>>8;
		temp_str[GS_MAC] = (char)GS_MAC_DEFAULT ;
		temp_str[GS_MAC+1] = (char)(GS_MAC_DEFAULT >> 8);
		temp_str[GS_MAC+2] = (char)(GS_MAC_DEFAULT >>16);
		temp_str[GS_MAC+3] = GS_MAC_DEFAULT >> 24;
		temp_str[TEMADJ_A+1] = (char)T_PADEFAULT;
		temp_str[TEMADJ_A] = T_PADEFAULT>>8;
		temp_str[TEMADJ_B+1] = T_PBDEFAULT;
		temp_str[TEMADJ_B] = T_PBDEFAULT>>8;
		temp_str[CHKCODE] = CHECKHEAD;
		FLASH_EraseSector(FLASHSctorNo*SECTOR_SIZE);
		FLASH_WriteSector(FLASHSctorNo*SECTOR_SIZE, (unsigned char *)temp_str, SECTOR_SIZE);
		WriteByteFromEEprom(TECStrength,E_ES);	//es
		WriteByteFromEEprom(LANGUAGE,E_LAN);		//lan
		WriteByteFromEEprom(W_THERMO,E_TEM);		//tec dir
		WriteByteFromEEprom(AUTORUN,E_AUTO);		//auto
		WriteFloatFromEEprom(MaxP,SystemMaxP);	//max power
		WriteFloatFromEEprom(E_MI,E_UMI);				//mi
		WriteFloatFromEEprom(MaxLDVLimit,E_LV);	//max ldv
		WriteFloatFromEEprom(PRE_ArgA,E_KA);		//KA
		WriteFloatFromEEprom(PRE_ArgB,E_KB);		//KB
//		FLASH_EraseSector(FLASHSctorNo*SECTOR_SIZE);
//		FLASH_WriteSector(FLASHSctorNo*SECTOR_SIZE, (unsigned char *)temp_str, SECTOR_SIZE);
	}
}
