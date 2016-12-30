/*--------------------------------------------------------------------------
GS_PROTOCOL.H

Header file for the MCU-PC PROTOCOL definition applied in Project GRAPE microcontroller.
Copyright (c) 2011 GS Lasers Technology (Shanghai) Co., Ltd
All rights reserved.
Version: EBAKIWI-0030A1
Modified: July 22, 2014
--------------------------------------------------------------------------*/

#ifndef __GS_PROTOCOL_H__
#define __GS_PROTOCOL_H__

#include	<stdlib.h>
#include	<string.h>
#include	"queue.h"
#include	"parameter.h"
#include	<MQX.h>
#define byte unsigned char

#define COMM_UART		0x00
#define DEBUG_UART	0x01
#define HMI_UART		0x02

/*------------------------------------------------
	PC-MCU Protocol command definition
------------------------------------------------*/

#define Qinfor		0x01
#define QLDsta		0x03
#define SLDI			0x04
#define QLDI			0x05
#define TECON	 		0x06
#define TECOFF	 	0x07
#define QHARDID 	0x08
#define QPDFRQ	 	0x09
#define EREEPROM	0x0A
#define RstLD			0x0B
#define SelfChk 	0x0C
#define SCANSTOP	0x0D
#define Reg				0x0E
#define RstSys		0x0F
#define QTem			0x10
#define STem			0x11
#define QLDV			0x12
#define QTecCur		0x14
#define QAirTem		0x16
#define Init			0x17
#define InitID		0x18
#define InitMAC		0x19
//#define InitPreV	0x1A
#define InitPreT	0x1B
#define InitKp		0x1C
#define InitKi		0x1D
#define InitKd		0x1E
#define SRATIO		0x1F
#define QRATIO		0x20
#define InitTAdjA	0x21
#define InitTAdjB	0x22
#define Qinit			0x23
#define QID				0x24
#define QPDpwr		0x25
#define LDON			0x26
#define LDOFF			0x27
#define QMAC			0x28
#define RREG			0x29
#define WREG			0x2A
#define TECDIR		0x2B
#define QDIR			0x2c
#define TMSON			0x43
#define TMSOFF		0x44
#define QPRET			0x33
#define PSTAT			0x3D
#define QTARGET		0x3E
#define QTARCUR		0x41
#define QTARTEMP	0x42
#define SDISAP		0x45
#define	SENAP			0x46
#define InitRK		0xfc
#define CANCEL		0xfe

#define QKP					0x4D
#define QKI					0x4E
#define QKD					0x4F

#define QPARA				0x50
#define SI					0x51
#define QI					0x52
#define STecSet			0x6D
#define QTecSet			0x6E
#define InitPreC		0x6F
#define QPREC				0x73
#define SBWTHERMO		0x74
#define PAGECHANGE	0x9A
#define QL					0x9C
#define SL					0x9D
#define SAVE				0x9E
#define HMILOAD			0xfd
#define QAUTO				0x9f
#define SAUTO				0xa0
#define QPower			0xa1
#define SPower			0xa2
#define QW					0xa3
#define SW					0xa4
#define InitKB			0xa5
#define InitKA			0xa6
#define InitQKB			0xa7
#define InitQKA			0xa8
#define InitQES			0xa9
#define InitSES			0xaa
#define QCRC				0xab
#define SCRC				0xac
#define SSN					0xad
#define QSN					0xae
#define QPM					0xaf
#define SPM					0xb0
#define QTM					0xB1
#define STM					0xB2
#define InitLV			0xb3
#define InitQLV			0xb4
#define InitLI			0xb5
#define InitQLI			0xb6
#define InitMI			0xb7
#define InitQMI			0xb8
#define UNKNOWN 		0xFF

/*------------------------------------------------
	PC-MCU Protocol command definition
------------------------------------------------*/



unsigned char getCR(Queue* q, char channel);
void ProcessMsg(Queue *q, char channel);
extern void ProcessUARTMsg(unsigned int dummy);
void SendStr(unsigned char* dat, char channel);
void retRqtHead(char channel);
unsigned char getCmd(Queue *q, float* f, char* strpara);
unsigned char crc8l(unsigned char *ptr, unsigned char len);
void rqtQinfor(char channel);
void rqtQLDcur(void);
void rqtQLDsta(void);
void rqtQPDTH(void);
void rqtQPulNum(void);
void rqtQAirTem(void);
void rqtQPDpwr(void);
void rqtQTem(char channel);
void rqtSTem(char channel,float para);
void noIdentify(char channel);
void CRCerror(char channel);
void rqtSPDThres(float f);
void countCoNum( unsigned char* p, unsigned char* len);
float getpara( unsigned char p, unsigned char len);
void rqtQPDThres(void);
void rqtDIRswitch(char channel);
void rqtQDIRswitch(char channel);
void initID(char* strpara);
void initMAC(char* strpara);
void initPreC(char channel,float para);
void initPreT(char channel,float para);
void initKp(char channel,char* strpara);
void initKi(char channel,char* strpara);
void initKd(char channel,char* strpara);
void rqtQKp(void);
void rqtQKi(void);
void rqtQKd(void);
void initTadjA(float para);
void initTadjB(float para);
void rqtQID(void);
void rqtQinit(char channel);
void rqtmoduleSwitch(unsigned char c);
void getstrpara(unsigned char p, unsigned char len, char* para);
void rqtrReg(char channel,unsigned char* strpara);
void rqtwReg(char channel,unsigned char* strpara);
void rqtQPreTem(void);
void rqtQMAC(void);
void rqtSaveRom(void);
void rqtTECswitch(char channel,char c);
void rqtReadDefault(void);
void rqtTarget(char channel);
void rqtTargetTemp(void);
void rqtQHardID(void);
void rqtQparam(char channel);
void paraOverRange(char channel);
void wrongFormat(char channel);
void rqtLDSwitch(char channel,unsigned char c);
void rqtAPSyncLD(char channel,unsigned char c);
void rqtTMSswitch(char c);
void rqtSLDI(char channel,float para);
void rqtQLDI(char channel);
//float GetLDI(void);
void rqtSTec(char channel, float para);
void rqtQTec(char channel);
void rqtQLDV(char channel);
void rqtSBWThermo(char channel);
extern void debug_info_task(uint32_t dummy);
void HmiCancel(char channel);

void rqtSL(char channel,float para);
void rqtQL(char channel);
void rqtSave(char channel);
void HmiLoad(char channel);
void rqtSAUTO(char channel,float para);
void rqtQAUTO(char channel);
void initRK(char channel,char* strpara);
float GetTecVol(void);
void SwitchLanguage(void);
void rqtQPower(char channel);
void rqtSPower(char channel,float para);
void rqtSW(char channel,float para);
void rqtQW(char channel);
void initKA(char channel,float para);
void initKB(char channel,float para);
void initQKA(char channel);
void initQKB(char channel);
float GetPowerWithCurrent(float current);
float GetCurrentWidthPower(float Power);
void initQES(char channel,float para);
void initSES(char channel,float para);
void initCRC(char channel, char* strpara);
void rqtQCRC(char channel);
void initSN(char channel, char* strpara);
void rqtQSN(char channel);
void initPM(char channel, char* strpara);
void rqtQPM(char channel);
void initTM(char channel, char* strpara);
void rqtQTM(char channel);
void initLV(char channel,float para);
void initQLV(char channel);
void initLI(char channel,float para);
void initQLI(char channel);
void initMI(char channel,float para);
void initQMI(char channel);
void hmi_end(char channel);
#endif
