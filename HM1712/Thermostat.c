#include "Thermostat.h"
#include "ftm.h"
#include "gpio.h"
#include  <stdlib.h> 
#include "api.h"
#include "PublicHead.h"
#include "i2c_ctrl.h"
//int T_Target,pT_End, T_Delta1;
unsigned char Index;
sThermoPara thermoPara_LD;
extern char ifBWThermo;
long int  T_EMAX=5000;
void thermostat_task(uint32_t dummy)
{
	initThermoPara(&thermoPara_LD);
	while(1)
	{
		thermostat();
		_time_delay(10);
	}
}

void initThermoPara(sThermoPara* thermoPara)
{
	thermoPara->en_tec =1;
	thermoPara->T_Ek1 = 0;
	thermoPara->T_Ek1 = 0;
	thermoPara->T_Ek2 = 0;
	thermoPara->T_Delta1 = 0;
	thermoPara->pT_End = 0;
	thermoPara->T_SampleSum = 0;
	thermoPara->TEC_Dac = 0;
	thermoPara->T_Real = 0;
	thermoPara->T_Target = ReadIntFromEEprom(PRE_TEM);
	thermoPara->T_Kp = ReadIntFromEEprom(KP);
	thermoPara->T_Ki = ReadIntFromEEprom(KI);
	thermoPara->T_Kd = ReadIntFromEEprom(KD);
	thermoPara->ftmCh = HW_FTM1;
	thermoPara->ftmHW = HW_FTM_CH1;
	thermoPara->T_Real = GetCurrTempADC();
	thermoPara->ifBWThermo = ReadByteFromEEprom(W_THERMO);
	for(int i=0;i<WindowWidth;i++)
	{
		thermoPara->T_SampleData[i]=thermoPara->T_Real;
		thermoPara->T_SampleSum+=thermoPara->T_SampleData[i];
	}
}

void InitTherm(void)
{
	initThermoPara(&thermoPara_LD);
}


void sample_single(sThermoPara* tpara)
{
	tpara->TReal = GetCurrTempADC();
	tpara->T_SampleSum = tpara->T_SampleSum + tpara->TReal - tpara->T_SampleData[tpara->pT_End];
	tpara->T_Real = tpara->T_SampleSum/WindowWidth;
	tpara->T_SampleData[tpara->pT_End] = tpara->TReal;
	tpara->pT_End++;
	if(tpara->pT_End>=WindowWidth)
		tpara->pT_End=0;
}
void doSample()
{
	
	sample_single(&thermoPara_LD);
	//ifsample == 0;
	
}

void thermostat_single(sThermoPara* tpara)
{
	long int Delta;             //clear ADC conversion complete overflow	
	if(tpara->en_tec ==1 /*&& GPIO_ReadBit(EN_TEC)==0*/)
	{
		#if VersionHPower
		if(tpara->T_Real >0x71A4 )//tem > 55¡ãC, shutdown LD (55 - 12.633)*32767.0/3/15.906 = 29092 0x71A4
		{
			tpara->TEC_Dac = 0;
			LDswitch(0,0);
			TECswitch(0,0);
			return ;
		}	
		#endif
		
		tpara->T_Ek2=tpara->T_Ek1;
		tpara->T_Ek1=tpara->T_Ek;
		tpara->T_Ek=tpara->T_Target - tpara->T_Real;  
		long int last_Tec = tpara->TEC_Dac;
		Delta=0;
		Delta+=		tpara->T_Kp*(tpara->T_Ek-tpara->T_Ek1)+ 
							(long)tpara->T_Ki*tpara->T_Ek + 
							tpara->T_Kd*(tpara->T_Ek+tpara->T_Ek2-2*tpara->T_Ek1);						
		Delta+=tpara->T_Delta1;
		tpara->T_Delta1=Delta-(Delta/1000)*1000;
		Delta=Delta/1000;
		tpara->TEC_Dac+=Delta;
		
		if(1 == tpara->ifBWThermo)
		{
			if(tpara->TEC_Dac> T_EMAX)
			{
				tpara->TEC_Dac = T_EMAX;
			}
			else if(tpara->TEC_Dac < -T_EMAX)
			{
				tpara->TEC_Dac = -T_EMAX;
			}
		
			if(tpara->TEC_Dac > 0)
			{	
				#if VersionHPower
				if((GPIO_ReadBit(HW_GPIOE, 8) == 0x0 )&&(tpara->TEC_Dac > 50) )    //heating  
				{
					FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1, 0);	
					GPIO_WriteBit(HW_GPIOE,8,1);		//1 -> 0				
				}
				#else
				if(GPIO_ReadBit(HW_GPIOD, 7) == 0x01 && tpara->TEC_Dac>50)
					{
						writeDAC(D_ChTec,  0);
						GPIO_WriteBit(HW_GPIOD,7,0);
					}
				#endif
	//		GPIO_WriteBit(HW_GPIOE,8,1);		//1 -> 0
		}
		else
		{	
			#if VersionHPower
			if((GPIO_ReadBit(HW_GPIOE, 8) == 0x1 )&&(tpara->TEC_Dac < -50))    //cooling
			{
				FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1, 0);
				GPIO_WriteBit(HW_GPIOE,8,0);	//0 -> 1
			}
			#else
				if(GPIO_ReadBit(HW_GPIOD, 7) == 0x0 && tpara->TEC_Dac<-50)
			{
				writeDAC(D_ChTec,  0);
				GPIO_WriteBit(HW_GPIOD,7,1);
			}
		
			#endif
		}
			
			if(last_Tec!= tpara->TEC_Dac)
			{
				if(tpara->TEC_Dac >=0)
				{
					#if VersionHPower
					FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1, tpara->TEC_Dac);						
					#else
					writeDAC(D_ChTec, tpara->TEC_Dac);
					#endif
				}
				else
				{
					#if VersionHPower
					FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1, -(tpara->TEC_Dac));		
					#else
					writeDAC(D_ChTec, -(tpara->TEC_Dac));
					#endif
				}
			}
		}
		else
		{
			if(tpara->TEC_Dac >= 0)
			{
				tpara->TEC_Dac = 0;
			}
			else if(tpara->TEC_Dac < -T_EMAX)
			{
				tpara->TEC_Dac = -T_EMAX;
			}
			if(last_Tec!= tpara->TEC_Dac)
			{
				if(tpara->TEC_Dac <=0)
				{
					FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1, -tpara->TEC_Dac);						
				}
			}
		
		}
	}
}
void thermostat()
{
	for(char idx = 0; idx <15;idx++)
	{
		doSample();
	}
	

	thermostat_single(&thermoPara_LD);
	
}
