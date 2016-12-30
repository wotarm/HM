#include "api.h"
#include "adc_ctrl.h"
#include "common_fun.h"
#include "dac_ctrl.h"
#include "spi_ctrl.h"
#include "ftm.h"
#include "gpio.h"
#include "Thermostat.h"
#include "eeprom.h"
#include "can_ctrl.h"
#include "PublicHead.h"
#include "i2c_ctrl.h"
unsigned int idat;
unsigned char en_tec;
unsigned int Alarm=0;
unsigned char reg_buf[4];

char motor2scan = 0;
unsigned int test1 = 0;
unsigned int test2 = 0;
unsigned int test3 = 0;
int T_Target;
char work_mode = 0;
char EN_Logic = 1;
unsigned int PDthresDuty = 0;
extern float MaxLDV;

extern char ifBWThermo;
extern sThermoPara thermoPara_LD;
extern float BaseCurrent;
extern unsigned int base_LDI_DAC;
extern float DACREF;
extern unsigned int base_LDI_ADC;
extern char LDADC_code;
s_LD_para LD_para;

void init_LD_para()
{
	LD_para.enLD = 0;
	LD_para.curctrlflag = 0;
	LD_para.I_Target = 0;
}


void LDswitch(char ldnum, char state)
{
	#if VersionHPower
	GPIO_WriteBit(EN_LD, state);
	LD_para.enLD = state&0x01;
	if((state&0x01) ==0x0)
	{
		LD_para.curctrlflag = 0;
	//	writeDAC(D_ChCur,0);				//del by wt
		GPIO_WriteBit(HW_GPIOB, 2,0);      //turn the las pin on add wt
	}
	#else
	LD_para.enLD = state&0x01;
	if((state&0x01) ==0x0)
	{
		LD_para.curctrlflag = 0;
		writeDAC(D_ChCur,0);
	}
	#endif
	
}
char LDstatus(void)
{
	#if VersionHPower 
	return GPIO_ReadBit(EN_LD);
	#else
	return LD_para.enLD;
	#endif
}
void readReg(unsigned char addr, unsigned char* dat,unsigned char len)
{
	unsigned char idx;
	unsigned int buf;
	for(idx = len;idx > 0; idx--)
	{
		buf = SingleRead(addr+idx-1);
		dat[len*2 - 2*idx+1] = buf & 0x00FF;
		dat[len*2 - 2*idx] = ((buf & 0xFF00)>>8);
	}
}
void writeReg(unsigned char addr,unsigned char* dat, unsigned char len)
{
	char idx;
	unsigned int buf;
	unsigned char c = len%4;
	unsigned char offset = (c>0)&&(c<3);
	for(idx = (len/2.0+0.5)/2 + 0.5; idx >0; idx--)
	{
		if(0<c &&c<3 && idx ==1)
			buf = dat[2*idx-1 - offset];
		else
			buf = dat[2*idx-1 - offset] + (dat[2*idx-2 - offset]<<8);
		SingleWrite(addr+(len/2.0+0.5)/2 + 0.5 -idx, buf);
	}
}

void CurCtrl_Task(uint32_t dummy)
{
	while(1)
	{
		if(LD_para.curctrlflag == 1)
		{
			int cur = readDAC(D_ChCur);
			if(cur < LD_para.I_Target)
			{
				cur += CURRENTSTEP;
				if(cur >= LD_para.I_Target)
				{
					cur = LD_para.I_Target;
					LD_para.curctrlflag = 0;
				}
				writeDAC(D_ChCur, cur);
				cur = readDAC(D_ChCur);
			}
			if(cur > LD_para.I_Target)
			{
				cur -= CURRENTSTEP;
				if(cur <= LD_para.I_Target)
				{
					cur = LD_para.I_Target;
					LD_para.curctrlflag = 0;
				}
				writeDAC(D_ChCur, cur);
				if(cur == 0 && LD_para.enLD == 0)
				{
					GPIO_WriteBit(EN_LD,0);
				}
			}
		}
		_time_delay(1000);
	}
}


void CheckLDV_Task(uint32_t dummy)
{
	float ldv = 0;
	float ldv1 =0;
	while(1)
	{	
		ldv = GetLDV();
		if(ldv>=MaxLDV){
			ldv1 = GetCurrent();
			retRqtHead(COMM_UART);
			char tmp[20];
			sprintf(tmp, "LDV out of range%.3f %.3f",ldv,ldv1);
			SendStr((unsigned char*)tmp,COMM_UART);	
			while(1){
				writeDAC(D_ChCur,0);
				LDswitch(0,0);
				_time_delay(50);
				GPIO_ToggleBit(HW_GPIOA, 25);      //turn the LED pin
			}
		}
		_time_delay(100);
	}
}

void stateSwitch(char state)
{

}

unsigned int GetADCWithTemp(float temp)
{
	unsigned int adc = 0;
	#if VersionHPower
	adc = (temp - 12.633f)*32767.0f/3/15.906f;
	#else
	adc = (24.705f - temp)/0.0104f;
	#endif
	return adc;
}

float GetTemp(void)
{
	float temp = 0;
	#if VersionHPower
	temp = 15.906* ConReadADC(A_RealLDTem)/32767.0 *3 + 12.633;
	#else
	temp = get_i2c_tem(0);
	#endif
	return temp;

}
unsigned int GetCurrTempADC(void)
{
	unsigned int adc = 0;
	#if VersionHPower
	adc = ConReadADC(A_RealLDTem);
	#else
	adc = get_i2c_value(0);
	#endif
	return adc;

}
float GetTarTemp(void)
{
	float tartemp = 0;
	#if VersionHPower
	tartemp = 15.906* thermoPara_LD.T_Target/32767.0 *3 + 12.633;
	#else
	tartemp = 24.705 - 0.0104*thermoPara_LD.T_Target;
	#endif
	return tartemp;
}
unsigned int GetDACValueWithCurrent(float current)
{
	unsigned int dac = 0;
	#if VersionHPower
	dac = ((current-BaseCurrent)*0.05f +0.005f + base_LDI_DAC/65535.0f*3)/(DACREF) *4095;
	#else
	dac = current /1.2f *4095;
	#endif
	if(dac>4095)
		dac = 4095;
	return dac;
}

float GetTargetCurrentWithDAC(void)
{
	float tarcurrent = 0;
	#if VersionHPower
	tarcurrent = (readDAC(D_ChCur)/4095.0f*(DACREF) - base_LDI_DAC/65535.0f*3 -0.005f)*20+BaseCurrent;
	#else
	tarcurrent = ConReadADC(A_RealLDCur)/65535.0f *1.2f;
	#endif
	if(tarcurrent < 0)
	{
		tarcurrent=0;
	}
	return tarcurrent;
}

float GetAimCurrWithInitCurr(unsigned int InitTarCurr)
{
	float aimcurr = 0;
	#if VersionHPower
	aimcurr = ((InitTarCurr/4095.0)*(DACREF)-base_LDI_DAC/65535.0*3-0.005)*20+BaseCurrent;
	if(aimcurr < 0 || aimcurr>32767)
	{
		aimcurr=0;
	}
	#else
	aimcurr = InitTarCurr/4095.0 *1.2;
	#endif
	return aimcurr;
}

float GetAimTempWithInitTemp(unsigned int InitTarTemp)
{
	float tartemp = 0;
	#if VersionHPower
	tartemp = 15.906f* InitTarTemp/32767.0f *3 + 12.633f;
	#else
	tartemp = 24.705f - 0.0104f*InitTarTemp;
	#endif
	return tartemp;
}

float GetCurrent(void)
{
	float cerrent = 0;	
	#if VersionHPower
	unsigned dac = 0;
	dac = ConReadADC(A_RealLDCur);
	if(dac<base_LDI_ADC)
		dac= 0;
	else
		dac = dac-base_LDI_ADC;
	cerrent = ((dac)/32767.0f*3)*10;
	#else
	cerrent = ConReadADC(A_RealLDCur)/65535.0f*1.2f;
	#endif
	if(cerrent <0)
	{
		cerrent=0;
	}
	return cerrent;
}

float GetLDV(void)
{
	unsigned int adc = 0;
	float ldv = 0;
	#if VersionHPower
	adc = ConReadADC(A_RealLDVol);
	if(adc > 32767)
		adc= 0;
	ldv = adc/32767.0f * 3 *23 / 3.0f;  //need more calculation
	#else
	adc = ConReadADC(A_RealLDVol);
	if(adc > 65535)
		adc= 0;
	ldv = adc/65535.0f * 3*2;  //need more calculation
	#endif
	return ldv;
}
void TECswitch(char module,unsigned char c)
{
	thermoPara_LD.en_tec = c; 
	if(((~c)&0x01)==0x01)
	{
		thermoPara_LD.TEC_Dac = 0;
		FTM_PWM_ChangeDuty(thermoPara_LD.ftmHW, thermoPara_LD.ftmCh, 0);
	}
	GPIO_WriteBit(EN_TEC, (~c)&0x01);

}

float GetTecCurr(void)
{
	float curr = 0;
	#if VersionHPower
	curr = ConReadADC(A_RealTecCur)/65535.0 * 3 * 12;  //need more calculation 4 -> 12 wt
	#else
	curr =  ConReadADC(A_RealTecCur)/65535.0 *3/2;  //need more calculation
	#endif
	return curr;

}

float GetTecV(void)
{
	float v = 0;
	#if VersionHPower
	v = ConReadADC(A_RealTecVol)/65535.0 *30;
	#else
	v =  ConReadADC(A_RealTecVol)/65535.0 * 3/0.392;  //need more calculation
	#endif
	return v;

}

float GetTECS(void)
{
	float s = 0;
	#if VersionHPower
	s = thermoPara_LD.TEC_Dac/10000.0;
	#else
	s = readDAC(D_ChTec)/4095.0;
	#endif
	return s;

}

void StartLCD(void)
{	
	char temp[20];
	sprintf(temp, "page main");
	SendStr((unsigned char*)temp, HMI_UART);
	hmi_end(HMI_UART);

}
void THOswitch(char c)
{
	thermoPara_LD.en_tec = (c&0x01);
	if((c&0x01) == 0x0)
	{
		thermoPara_LD.TEC_Dac = 0;
		FTM_PWM_ChangeDuty(thermoPara_LD.ftmHW, thermoPara_LD.ftmCh, 0);
	}
}
void SingleWrite(unsigned char address, unsigned int dat)
{
	if(address<=128 && address >= 0)
	{
		return;
	}
	switch(address)
	{
		case(R_enLD)		:LDswitch(LD0,dat);
		case(R_enTEC)		:TECswitch(LD0,dat);
								 break;
		
		case(R_CANID)	:	reg_buf[0] = dat&0x00FF;
								writeDevCanID((char*)reg_buf);break;
		case(R_PreCur)	:	reg_buf[0] = dat;
								reg_buf[1] = dat<<8;
								writePreCur((char*)reg_buf);
		case(R_PreTem)	:	reg_buf[0] = dat;
								reg_buf[1] = dat<<8;
								writePreTem((char*)reg_buf);break;
		case(R_KP)			:	reg_buf[0] = dat;
								reg_buf[1] = dat<<8;
								thermoPara_LD.T_Kp = dat;
								writeKp((char*)reg_buf);break;
		case(R_KI)			:	reg_buf[0] = dat;
								reg_buf[1] = dat<<8;
								thermoPara_LD.T_Ki = dat;
								writeKi((char*)reg_buf);break;
		case(R_KD)			:	reg_buf[0] = dat;
								reg_buf[1] = dat<<8;
								thermoPara_LD.T_Kd = dat;
								writeKd((char*)reg_buf);break;
		case(R_TEMADJ_A)	: reg_buf[0] = dat;
								reg_buf[1] = dat>>8;
								writeTemAdjA((char*)reg_buf);break;
		case(R_TEMADJ_B)	: reg_buf[0] = dat;
								reg_buf[1] = dat>>8;
								writeTemAdjB((char*)reg_buf);break;			
		case(R_GS_MAC)	:	reg_buf[0] = dat;
								reg_buf[1] = dat<<8;break;
		case(R_GS_MAC+1):	reg_buf[2] = dat;
								reg_buf[3] = dat<<8;
								writeMAC((char*)reg_buf);break;
		case(R_TargCur):writeDAC(D_ChCur,dat);break;
		case(R_EN_ARM_OUT): if(dat>0)GPIO_WriteBit(EN_ARM_OUT,1);
												else GPIO_WriteBit(EN_ARM_OUT,0);break;
		case(R_EN_LOGIC)	: EN_Logic = dat;break;
		case(R_testAlarm) : FLEXCAN_Tx_mailbox(CAN_DEVICE, 10, &dat);break;
		case(R_WORKMODE)	:	if(dat>1)dat=1; 
												work_mode = dat;
												writeWorkMode(&work_mode);break;
		case(R_MOTORRESET): break;
		case(R_BWTHERMO)	: if(dat>1)dat=1;
												ifBWThermo = dat;
												writeBW_Thermo(&ifBWThermo);break;
		case(R_LDADCCODE):if(dat>7)dat=7;LDADC_code = dat;
											WriteByteFromEEprom(E_ADCCode,dat);writeADS_Cfg(0,0);break;
		case(250)			:test1 = dat;break;
		case(251)			:test2 = dat;break;
		case(252)			:test3 = dat;break;
		default				: break;
	}
}

unsigned int SingleRead(unsigned char address)
{			
	//unsigned char dat;
	if(address<=128 && address >=0)
	{
		return 0;
	}
	switch(address)
	{
		case(R_MCUVer)		: 	return(Version);
		case(R_MCURevL)		: 	return(Reversion);
		case(R_FPGA_INT)	:	return 1;
		case(R_HardID)		:		return getHardID();
		case(R_enLD)			:return LDstatus();
		case(R_enTEC)		: return thermoPara_LD.en_tec;
		case(R_RealLDCur)	: idat = ConReadADC(A_RealLDCur); return (idat);
		case(R_RealLDTem)	: idat = ConReadADC(A_RealLDTem); return (idat);
		case(R_RealTECCur)	: idat = ConReadADC(A_RealTecCur);return (idat);
		case(R_RealPDPwr)	: idat = ConReadADC(A_RealPDPwr); return (idat);
		
		case(R_TargTem)			: idat = thermoPara_LD.T_Target;return(idat);
		case(R_TargCur)			: idat = readDAC(D_ChCur);
		case(R_PreCur)		:	readPreCur((char*)reg_buf);
								idat = reg_buf[0]+(reg_buf[1]<<8);
								return(idat);
		case(R_CANID)		:	readDevCanID((char*)reg_buf);return(reg_buf[0]);

		case(R_PreTem)		:	readPreTem((char*)reg_buf);
								idat = reg_buf[0]+(reg_buf[1]<<8);
								return(idat);
		case(R_KP)			:	readKp((char*)reg_buf);
								idat = reg_buf[0]+(reg_buf[1]<<8);
								return(idat);
		case(R_KI)			:	readKi((char*)reg_buf);
								idat = reg_buf[0]+(reg_buf[1]<<8);
								return(idat);
		case(R_KD)			:	readKd((char*)reg_buf);
								idat = reg_buf[0]+(reg_buf[1]<<8);
								return(idat);
		case(R_TEMADJ_A): readTemAdjA((char*)reg_buf);
								idat = reg_buf[0]+(reg_buf[1]<<8);
								return(idat);
		case(R_TEMADJ_B): readTemAdjB((char*)reg_buf);
								idat = reg_buf[0]+(reg_buf[1]<<8);
								return(idat);		
		case(R_GS_MAC)		:	readMAC((char*)reg_buf);
								idat = reg_buf[0]+(reg_buf[1]<<8);
								return(idat);
		case(R_GS_MAC+1)	:	idat = reg_buf[2]+(reg_buf[3]<<8);
								return(idat);
//		case(R_EN_ARM_IN)	:		return GPIO_ReadBit(EN_ARM_IN);
//		case(R_EN_ARM_OUT):		return GPIO_ReadBit(EN_ARM_OUT);
		case(R_STATUS)		:		return GPIO_ReadBit(STATUS_N);
		case(R_STATUSNP1)	:		return GPIO_ReadBit(STATUS_Np1);
		case(R_BWTHERMO)	:		return ifBWThermo;
		case(R_LDADCCODE):return ReadByteFromEEprom(E_ADCCode);
		case(250)			:return test1;
		case(251)			:return test2;
		case(252)			:return test3;
		case(253)			:return 1;
		
		default		: return(0xFF);
	}
}
unsigned char getHardID()
{
	unsigned char ret;
	ret = GPIO_ReadBit(HW_GPIOD,1) & 0x01;
	ret = (ret<<1) | (GPIO_ReadBit(HW_GPIOD,0) & 0x01);
	ret = (ret<<1) | (GPIO_ReadBit(HW_GPIOC,19) & 0x01);
	ret = (ret<<1) | (GPIO_ReadBit(HW_GPIOC,18) & 0x01);
	ret = (ret<<1) | (GPIO_ReadBit(HW_GPIOC,17) & 0x01);
	ret = (ret<<1) | (GPIO_ReadBit(HW_GPIOC,16) & 0x01);
	ret = (ret<<1) | (GPIO_ReadBit(HW_GPIOC,13) & 0x01);
	ret = (ret<<1) | (GPIO_ReadBit(HW_GPIOC,12) & 0x01);
	return ~ret;
}


unsigned char CalCRC(char * str, unsigned char length)
{
	unsigned char result = 0;
	unsigned char i = 0;
	for(i=0;i<length;i++){
		result += str[i];
	}
	return result;
}

void CurrCalibration(void)
{
	unsigned int dac = 0;
	LDswitch(1,1);
	dac = (0.005 + base_LDI_DAC/65535.0*3)/(DACREF) *4095;
	writeDAC(D_ChCur,dac);
	//wait...
	_time_delay(1000);
	BaseCurrent = GetCurrent();
	dac = GetDACValueWithCurrent(0);
	LD_para.BaisI = dac;
	writeDAC(D_ChCur,dac);
	LDswitch(1,0);
}

unsigned int Getbase_LDI_ADC(void)
{
	float bu = 0;
	unsigned int temp = 0;
	float buff[10] = {0,0,0,0,0,0,0,0,0,0};
	for(int n=0;n<10;n++){
		buff[n]  += ConReadADC(A_RealLDCur);
	}
	for(int n=0;n<10;n++){
		bu = bu + buff[n];
	}
	temp  = bu/10;	
	return temp;
}

unsigned int Getbase_LDI_DAC(void)
{
	float buff[10] = {0,0,0,0,0,0,0,0,0,0};
	float bu = 0;
	unsigned int temp = 0;
	for(int n=0;n<10;n++){
		buff[n]  = ConReadADC(HW_ptr.ldi_ref);
	}
	for(int n=0;n<10;n++){
		bu = bu + buff[n];
	}
	temp  = bu/10;	
	return temp;
}
float GetDACRef(void)
{
	float buff[10] = {0,0,0,0,0,0,0,0,0,0};
	float bu = 0;
	float temp = 0;
	for(int n=0;n<10;n++){
		buff[n]  = 3 * ConReadADC(A_RealDACRef)/65535.0;
	}
	for(int n=0;n<10;n++){
		bu = bu + buff[n];
	}
	temp =bu/10;
	return temp;
}
