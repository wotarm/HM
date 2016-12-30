#ifndef _API_H_
#define _API_H_
#include	<stdlib.h>
#include	<string.h>
#include	"queue.h"
#include	"EEPROM.h"
#include	"parameter.h"
#include	"uart_ctrl.h"



void init_LD_para(void);
void LDswitch(char ldnum, char state);
char LDstatus(void);
void TECswitch(char module,unsigned char c);
void apiParse(char* cmd);
unsigned char getModuleState(void);
void stateSwitch(char c);
void readReg(unsigned char addr, unsigned char* dat, unsigned char len);
void writeReg(unsigned char addr,unsigned char* dat, unsigned char len);
unsigned int SingleRead(unsigned char address);
void SingleWrite(unsigned char address, unsigned int dat);

void getModeLock(unsigned int* x, unsigned int* y);

void THOswitch(char c);
unsigned char getHardID(void);
void init_LD_para(void);
extern void CurCtrl_Task(uint32_t dummy);
unsigned char CalCRC(char * str, unsigned char length);
void CheckLDV_Task(uint32_t dummy);
unsigned int GetDACValueWithCurrent(float current);
float GetTargetCurrentWithDAC(void);
float GetCurrent(void);
void CurrCalibration(void);
unsigned int Getbase_LDI_ADC(void);
unsigned int Getbase_LDI_DAC(void);
float GetDACRef(void);
unsigned int GetADCWithTemp(float temp);
float GetTemp(void);
float GetTarTemp(void);
float GetAimCurrWithInitCurr(unsigned int InitTarCurr);
float GetAimTempWithInitTemp(unsigned int InitTarTemp);
float GetLDV(void);
unsigned int GetCurrTempADC(void);
float GetTecCurr(void);
float GetTecV(void);
float GetTECS(void);
void StartLCD(void);
#endif
