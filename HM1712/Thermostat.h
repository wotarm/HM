#ifndef _THERMOSTAT_INCLUDE_
#define _THERMOSTAT_INCLUDE_
#include <mqx.h>
#include "adc_ctrl.h"
#include "dac_ctrl.h"
#include "EEPROM.h"

//-----------------------------------------------------------------------------
// Global Constants
//-----------------------------------------------------------------------------
//#define SYSCLK             24500000/8  // SYSCLK in Hz (24.5 MHz internal
                                       // oscillator / 8)
                                       // the internal oscillator has a
                                       // tolerance of +/- 2%

#define SystemTEClimit 8000
extern long int  T_EMAX;

struct thermoPara
{
	char en_tec;
	long int TEC_Dac;
	int T_Ek, T_Ek1, T_Ek2;
	int T_Kp,T_Ki,T_Kd;
	int T_Real;
	int TReal;
	long T_SampleSum;
	int T_SampleData[WindowWidth];
	int T_Target,pT_End, T_Delta1;
	char ftmHW;
	char ftmCh;
	char HWtype;
	char ifBWThermo;
};

typedef struct thermoPara sThermoPara;

void initThermoPara(sThermoPara* thermoPara);

extern void thermostat_task(uint32_t);
void InitTherm (void);
void ADC0_Conv (void);
void thermostat(void);
void doSample(void);
void thermostat_pad(void);
void thermostat_LD0(void);
void thermostat_LD1(void);
extern unsigned char ifconv;

#endif
