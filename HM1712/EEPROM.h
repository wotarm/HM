#ifndef _EEPROM_H_
#define _EEPROM_H_

#include	<stdlib.h>
#include	<string.h>
#include	"flash_rw.h"
#include	"gs_protocol.h"

#define DATA_REGION_SIZE	300
#define DEFAULT_DATA_REGION	0x64000
#define DATA_REGION0_ADDR	0x65000
#define DATA_AREA_SIZE		4096
#define DATA_REGION_NUM		(DATA_AREA_SIZE/DATA_REGION_SIZE)

#define CAN_ID			0x01

#define KP					0x06
#define KI					0x08
#define KD					0x0A
#define CURADJ_A		0x0C
#define CURADJ_B		0x0E
#define TEMADJ_A		0x10
#define TEMADJ_B		0x12
#define I_MIN				0x14
#define I_MAX				0x16
#define WORKMODE		0x18
#define W_THERMO 		0x19
#define GS_MAC			0x1A
#define PWR_PARAa		0x1E
#define PWR_PARAb		0x22
#define PWR_PARAc		0x26
#define PWR_PARAd		0x2A
#define ItoPWR_PARAa	0x2E
#define ItoPWR_PARAb	0x32
#define ItoPWR_PARAc	0x36
#define ItoPWR_PARAd	0x3A
#define PRE_ArgA	0x56
#define PRE_ArgB	0x5a
#define MaxP			0x70
#define TECStrength   0x7b
#define AUTORUN   0x7c
#define LANGUAGE  0x7d
#define SN				0x7e
#define PM				0x96
#define TM				0xAC
#define MOSynLD		0xcc
#define MaxLDVLimit	0xcd
#define E_LI		0xd1
#define E_MI		0xd5
#define E_ADCCode		0xd9
#define PRE_CUR			0xDA
#define PRE_TEM			0xDE
#define CHKCODE		0xFF
#define CHECKHEAD	0x11



void EEPROM_init(void);
void writeStrEEPROM(char* str, char shift, char len);
void writeEEPROM(char* str);
void readStrEEPROM(char* str, char shift, char len);
void readEEPROM(char* str);

void writeDefault(void);
void readDefault(char* str);

char findActiveRegion(void);
char getStatusChar(uint32_t addr);
char checkIfWrtRt(char* str, uint32_t addr);
void eraseEEPROM(void);
void saveasDefault(void);

void ifinit(void);
void initDefault(void);

void readDevCanID(char* str);
void writeDevCanID(char* str);

void readPreCur(char* str);
void writePreCur(char* str);

void readPreTem(char* str);
void writePreTem(char* str);

void readKp(char* str);
void writeKp(char* str);
void readKi(char* str);
void writeKi(char* str);
void readKd(char* str);
void writeKd(char* str);



void readTemAdjA(char* str);
void writeTemAdjA(char* str);
void readTemAdjB(char* str);
void writeTemAdjB(char* str);


void readMAC(char* str);
void writeMAC(char* str);
void readCheckCode(char* str);
void writeCheckCode(char* str);
void readWorkMode(char* str);
void writeWorkMode(char* str);
void readBW_Thermo(char* str);
void writeBW_Thermo(char* str);

char readIDfromFlash(void);
void writeIDtoFlash(char id);
int readKpfromFlash(void);
int readKifromFlash(void);
int readKdfromFlash(void);
unsigned int readPreTemfromFlash(void);
void writePreTemtoFlash(unsigned int tem);
unsigned int readPreCurfromFlash(void);
void writePreCurtoFlash(unsigned int thrs);
void writeLanguagetoFlash(char lan);
char readLanguagefromFlash(void);
char readAutorunfromFlash(void);
void writeAutoruntoFlash(char lan);
unsigned int readMaxPowerfromFlash(void);
void writeMaxPowertoFlash(unsigned int tem);
void writeArgAFlash(float tem);
void writeArgBFlash(float tem);
float readArgAfromFlash(void);
float readArgBfromFlash(void);
void writeAutoruntoFlash(char lan);
void writeTECStrengthtoFlash(char tec);
char readTECStrengthfroomFlash(void);
void WriteSN(char * buf,char len);
void ReadSN(char * buf,char len);
void WritePM(char * buf,char len);
void ReadPM(char * buf,char len);
void WriteTM(char * buf,char len);
void ReadTM(char * buf,char len);

unsigned char ReadByteFromEEprom(unsigned int address);
unsigned char WriteByteFromEEprom(unsigned int address,unsigned char data);
unsigned int ReadIntFromEEprom(unsigned int address);
unsigned int Read4ByteIntFromEEprom(unsigned int address);
float ReadFloatFromEEprom(unsigned int address);
unsigned long ReadULongFromEEprom(unsigned int address);
void FlashEEprom_Init(void);
void EraseFPGASector(void);
void FlashEEprom_Init(void);
void WriteIntFromEEprom(unsigned int address,unsigned int data);
void Write4ByteIntFromEEprom(unsigned int address, int data);
void WriteFloatFromEEprom(unsigned int address,float data);
void ReadStrFromEEprom(unsigned int address,char * buf,unsigned char length );
void WriteStrFromEEprom(unsigned int address,char * buf,unsigned char length );
#endif
