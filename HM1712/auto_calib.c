#include "auto_calib.h"
#include "adc_ctrl.h"
#include "dac_ctrl.h"
#include "api.h"
extern float I0_Para_WA;
extern float I0_Para_WB;
extern float I1_Para_WA;
extern float I1_Para_WB;
int Vref = 0xFFFF;
void auto_calc()
{
	int ADvalueA = 0;
	int	ADvalueB = 0;
	char idx = 0;
	if(1 == LDstatus(LD0))
	{
		writeDAC(D_ChCur0, inputA);
		_time_delay(1);
		for(;idx<20;idx++)
		{
			ADvalueA += ConReadADC(A_RealLD0Cur);
		}
		ADvalueA /= 20;
		if(ADvalueA < (inputA/4095.0*65535/3))
		{
			printf("low Current! can't calc!");
			return;
		}
		writeDAC(D_ChCur0, inputB);
		_time_delay(1);
		for(idx = 0;idx<20;idx++)
		{
			ADvalueB += ConReadADC(A_RealLD0Cur);
		}
		ADvalueB /=20;
		writeDAC(D_ChCur0, 0);
		I0_Para_WA = (inputB - inputA)/((ADvalueB - ADvalueA)/65535.0* 3 * (Vref/65535.0)/2)/4095.0;
		I0_Para_WB = (inputA - I0_Para_WA*4095 *( ADvalueA/65535.0 )* 3 * (Vref /65535.0) /2)/4095.0;
		writeCURAdjWA0toFlash(I0_Para_WA*1000);
		writeCURAdjWB0toFlash(I0_Para_WB*1000);
		printf("I1_Para_WA:%.3f",I0_Para_WA);
		printf("I1_Para_WB:%.3f",I0_Para_WB);
	}
	else
	{
		printf("LD0 disabled,auto failed!");
	}
	if(1 == LDstatus(LD1))
	{
		writeDAC(D_ChCur1, inputA);
		_time_delay(1);
		for(;idx<20;idx++)
		{
			ADvalueA += ConReadADC(A_RealLD1Cur);
		}
		ADvalueA /= 20;
		if(ADvalueA < (inputA/4095.0*65535/3))
		{
			printf("low Current! can't calc!");
			return;
		}
		writeDAC(D_ChCur1, inputB);
		for(idx = 0;idx<20;idx++)
		{
			ADvalueB += ConReadADC(A_RealLD1Cur);
		}
		ADvalueB /=20;
		writeDAC(D_ChCur1, 0);
		_time_delay(1);
		I1_Para_WA = (inputB - inputA)/((ADvalueB - ADvalueA)/65535.0* 3 * (Vref/65535.0)/2)/4095.0;
		I1_Para_WB = (inputA - I1_Para_WA*4095 * ADvalueA * 3 * Vref/65535.0 /65535.0 /2)/4095.0;
		writeCURAdjWA1toFlash(I1_Para_WA*1000);
		writeCURAdjWB1toFlash(I1_Para_WB*1000);
		printf("I1_Para_WA:%.3f",I1_Para_WA);
		printf("I1_Para_WB:%.3f",I1_Para_WB);
	}
	else
	{
		printf("LD1 disabled,auto failed!");
	}
	
}