#include	"gs_protocol.h"
//#include "includes.h"
#include "parameter.h"
#include "adc_ctrl.h"
#include "api.h"
#include "queue.h"
#include "uart_ctrl.h"
#include "common_fun.h"
#include "spi_ctrl.h"
#include "ftm.h"
#include "dac_ctrl.h"
#include "Thermostat.h"
#include "gpio.h"
#include "capture.h"
#include "PublicHead.h"
unsigned char idx;
char tmp[32];
char cmdbuf[48];
char temp[62];	//32 -> 62
char head_buf[4];
char buf[8]={' ','0','x','0','0','0','0','\0'};
float f;
unsigned int i;
unsigned char idx;
extern int T_Target;
int T_Kp,T_Ki,T_Kd;
char hexIn = 0;

int T_param_A;
int T_param_B;
unsigned int curLDI = 0;

char ifBWThermo = 0;
char if_process = 0;
char if_comm_process = 0;
char if_hmi_process = 0;

extern s_HW_ptr HW_ptr;
extern Queue RxD;
extern Queue Comm_RxD;
extern Queue Hmi_RxD;
extern char EN_LD1;
extern sThermoPara thermoPara_LD;
extern s_LD_para LD_para;
extern unsigned int base_LDI_ADC;
extern unsigned int base_LDI_DAC;
extern uint32_t freq;
char currentPage = 0;
int InitTarCurr = 0;
int InitTarTemp = 0;
char Autorun;
char language;
float ArgA = 1.0;
float ArgB = 0;
unsigned char objectS = 1;
float MaxPower = 60;
float MaxCurrent = 20;
int CommandCRC = 0;
extern int CalCurrBase;
extern float BaseCurrent;
extern char MOENLD;
extern float DACREF;
void SendStr(unsigned char* dat, char channel)
{
	//for(idx = 0; idx < strlen(dat); idx++)
	//{
	//	UART_SendByte(dat[idx]);
	//}
	if(channel == COMM_UART)
	{
		uart_comm_send((char*)dat);
	}
	else if(channel == HMI_UART)
	{
		uart_hmi_send((char*)dat);
	}
	else
		printf("%s",dat);
}
void UART_SendByte(unsigned char dat, char channel)
{
	if(channel == COMM_UART)
	{
		uart_comm_send((char*)&dat);
	}
	if(channel == HMI_UART)
	{
		uart_hmi_send((char*)&dat);
	}
	else
		printf("%c",dat);
}
void ProcessUARTMsg(unsigned int dummy)
{
	while(1)
	{
		_lwsem_wait(&HW_ptr.uart_sem);
		if(if_process == 1)
		{
			ProcessMsg(&RxD, DEBUG_UART);
			if(RxD.size == 0)
			{
				if_process = 0;
			}
		}
		if(if_comm_process == 1)
		{
			ProcessMsg(&Comm_RxD, COMM_UART);
			if(Comm_RxD.size == 0)
			{
				if_comm_process = 0;
			}
		}
		if(if_hmi_process ==1)
		{
			ProcessMsg(&Hmi_RxD, HMI_UART);
			if(Hmi_RxD.size == 0)
			{
				if_hmi_process = 0;
			}
		}
		//_time_delay(50);
		
	}
}



/*************************************************************************/
void ProcessMsg(Queue * q, char channel)
{
	if(!EmptyQ(q))									//
	{
		unsigned char cmd;
		unsigned char ret;
		float para;
		char strpara[36];
		memset(strpara, 0x0, 24);
		ret = getCR(q, channel);
		if(ret == 0x01)
		{
			cmd = getCmd(q, &para, strpara);
			switch(cmd)
			{
				case (Qinfor)	:rqtQinfor(channel);break;
				case (Init)		:EEPROM_init();break;
				case (InitID)	:initID(strpara);break;
				case (InitMAC)	:initMAC(strpara);break;
				case (InitPreT)	:initPreT(channel,para);break;
				case (InitPreC)	:initPreC(channel,para);break;
				case (InitKp)	:initKp(channel,strpara);break;
				case (InitKi)	:initKi(channel,strpara);break;
				case (InitKd)	:initKd(channel,strpara);break;
				case (InitTAdjA):initTadjA(para);break;
				case (InitTAdjB):initTadjB(para);break;
				case (Qinit)	:rqtQinit(channel);break;
				case (QID)		:rqtQID();break;
				case (QPDpwr)	:rqtQPDpwr();break;
				case (UNKNOWN)	:noIdentify(channel);break;
				case (RREG)		:rqtrReg(channel,(unsigned char*)strpara);break;
				case (WREG)		:rqtwReg(channel,(unsigned char*)strpara);break;
				//case (SRATIO)	:rqtSetDutyRatio(para);break;
				//case (QRATIO)	:rqtQDutyRatio();break;
				case (TECON)	:rqtTECswitch(channel,1);break;
				case (TECOFF)	:rqtTECswitch(channel,0);break;
				case (TECDIR)	:rqtDIRswitch(channel);break;
				case (QDIR):	rqtQDIRswitch(channel);break;
				case (QMAC)		:rqtQMAC();break;
				case (QPRET)	:rqtQPreTem();break;
				case (QTARGET)		:rqtTarget(channel);break;
				case (QTARTEMP)		:rqtTargetTemp();break;
				case (QKP)				:rqtQKp();break;
				case (QKI)				:rqtQKi();break;
				case (QKD)				:rqtQKd();break;
				case (QHARDID)		:rqtQHardID();break;
				case (QPARA)			:rqtQparam(channel);break;
				case (LDON)			:rqtLDSwitch(channel,1);break;
				case (LDOFF)		:rqtLDSwitch(channel,0);break;
				case (STecSet)	:rqtSTec(channel,para);break;
				case (QTecSet)	:rqtQTec(channel);break;
				case (SI)				:rqtSLDI(channel,para);break;
				case (QI)				:rqtQLDI(channel);break;
				case (QTem)			:rqtQTem(channel);break;
				case (STem)			:rqtSTem(channel,para);break;
				case (QLDV)			:rqtQLDV(channel);break;
				case (TMSON)		:rqtTMSswitch(1);break;
				case (TMSOFF)		:rqtTMSswitch(0);break;
				case (SBWTHERMO):rqtSBWThermo(channel);break;
				case (PAGECHANGE)	:currentPage = para;break;
				case (CANCEL):HmiCancel(channel);break;
				case (QL):rqtQL(channel);break;
				case (SL):rqtSL(channel,para);break;
				case (SAVE): rqtSave(channel);break;
				case (HMILOAD):HmiLoad(channel);break;
				case (QAUTO):rqtQAUTO(channel);break;
				case (SAUTO): rqtSAUTO(channel,para);break;
				case (InitRK):initRK(channel,strpara);break;
				case (QPower)			:rqtQPower(channel);break;
				case (SPower)			:rqtSPower(channel,para);break;
				case (SW)				:rqtSW(channel,para);break;
				case (QW)				:rqtQW(channel);break;
				case (InitKA)	:initKA(channel,para);break;
				case (InitKB)	:initKB(channel,para);break;
				case (InitQKA)	:initQKA(channel);break;
				case (InitQKB)	:initQKB(channel);break;
				case (InitSES)	:initSES(channel,para);break;
				case (InitQES)	:initQES(channel,para);break;
				case (SCRC):		initCRC(channel,strpara);break;
				case (QCRC):		rqtQCRC(channel);break;
				case (SSN):	initSN(channel,strpara);break;
				case (QSN):	rqtQSN(channel);break;
				case (SPM):	initPM(channel,strpara);break;
				case (QPM):	rqtQPM(channel);break;
				case (STM):	initTM(channel,strpara);break;
				case (QTM):	rqtQTM(channel);break;
				case (SDISAP):rqtAPSyncLD(channel,0);break;
				case (SENAP):rqtAPSyncLD(channel,1);break;
				case (InitLV):	initLV(channel,para);break;
				case (InitQLV):	initQLV(channel);break;
				case (InitLI):	initLI(channel,para);break;
				case (InitQLI):	initQLI(channel);break;
				case (InitMI):	initMI(channel,para);break;
				case (InitQMI):	initQMI(channel);break;
			}
		}
		else 
		{
			if(ret == 0x02)
			{
				ClearElement(q,3);
				memset(head_buf,0x0,4);
				CRCerror(channel);
			}
			if(ret == 0x03)
			{
				ClearElement(q, q->size);
			}
			if(ret == 0x04)
			{
				wrongFormat(channel);
				ClearElement(q, q->size);
			}
		}
	}
}


void hmi_end(char channel)
{
	UART_SendByte(0xFF, channel);
	UART_SendByte(0xFF, channel);
	UART_SendByte(0xFF, channel);
}

void retRqtHead(char channel)
{
	UART_SendByte(0x0D, channel);
	UART_SendByte(0x0A, channel);
}

unsigned char getCR(Queue* q, char channel)
{
	char ch;
	memset(cmdbuf, 0x00, 48);
	if(q->element[q->rear-1]!=0x0d && q->element[q->front]==0x0d)
	{
		q->element[q->front] = q->element[q->rear-1];
		q->element[q->rear-1] = 0x0d;
	}
	for(idx = 0; idx < 47; idx++)
	{
		
			if(1/*channel!=HMI_UART*/)
			{
				if(idx<3)
				{ 
					continue;
				}
			}
		
			ch = q->element[(q->front+idx)%MAXLENGTH];
			if(ch == 0x00)
			{
				return 0x03;
			}
			if(ch != 0x0d)
			{
				//if(channel != HMI_UART)
			//	{
					cmdbuf[idx-3] = q->element[(q->front+idx)%MAXLENGTH];
			//	}
				/*else
				{
					cmdbuf[idx] = q->element[(q->front+idx)%MAXLENGTH];
				}*/
			}
			else
			{
				if(q->size >= idx+2 -1)
				{
					if(1/*channel != HMI_UART*/)
					{
						cmdbuf[idx-3] = 0x0d;
						for(idx = 0; idx<3; idx++)
						{
							head_buf[idx] = q->element[(q->front+idx)%MAXLENGTH];
						}
						if(strcmp(head_buf,"GS_") != 0)
						{
							ClearElement(q,3);
							memset(head_buf,0x0,4);
							return 0x04;
						}
						ClearElement(q,3);
					}
				//	else
				//		cmdbuf[idx] = 0x0d;
					
					strLow2Cap(cmdbuf);
					memset(head_buf,0x0,4);
					return 0x01;
				}
				strLow2Cap(cmdbuf);
				ClearElement(q,3);
				memset(head_buf,0x0,4);
				return 0x01;
			}
	}
	if(idx >=46)
	{
		//ClearElement(q,3);
		memset(head_buf,0x0,4);
		return 0x03;
	}
	return 0x00;
}

unsigned char getCmd(Queue *q,  float* para, char* strpara)
{
	unsigned char pos;
	unsigned char len;
	memset(tmp, 0x00, 24);
	countCoNum( &pos, &len);
	

	memcpy(tmp, cmdbuf, 1);
	if(cmdbuf[0] == 'P')
	{
		if(cmdbuf[1] == '?' && cmdbuf[2] == 0x0d)
		{
			ClearElement(q, 4-1);
			return QPower;
		}
		else
		{
			if(cmdbuf[1 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 3 + len-1);
				return SPower;
			}
		}
	}
	if(cmdbuf[0] == 'T')
	{
		if(cmdbuf[1] == '?' && cmdbuf[2] == 0x0d)
		{
			ClearElement(q, 4-1);
			return QTem;
		}
		else
		{
			if(cmdbuf[1 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 3 + len-1);
				return STem;
			}
		}
	}
	if(cmdbuf[0] == 'W')
	{
		if(cmdbuf[1] == '?' && cmdbuf[2] == 0x0d)
		{
			ClearElement(q, 4-1);
			return QW;
		}
		else
		{
			if(cmdbuf[1 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 3 + len-1);
				return SW;
			}
		}
	}
	if(cmdbuf[0] == 'I')
	{
		if(cmdbuf[1] == '?' && cmdbuf[2] == 0x0d)
		{
			ClearElement(q, 4-1);
			return QI;
		}
		else
		{
			if(cmdbuf[1 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 3 + len-1);
				return SI;
			}
		}
	}
	if(cmdbuf[0] == 'L')
	{
		if(cmdbuf[1] == '?' && cmdbuf[2] == 0x0d)
		{
			ClearElement(q, 4-1);
			return QL;
		}
		else
		{
			if(cmdbuf[1 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 3 + len-1);
				return SL;
			}
		}
	}
	if(cmdbuf[0] == 'V')
	{
		if(cmdbuf[1] == '?' && cmdbuf[2] == 0x0d)
		{
			ClearElement(q, 4-1);
			return QLDV;
		}
		/*else
		{
			if(cmdbuf[1 + len] == 0x0d && len!=0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 3 + len-1);
				return SLDV;
			}
		}*/
	}
	

	memcpy(tmp, cmdbuf, 2);
	if(strcmp(tmp, "ES") == 0)
	{
		if(cmdbuf[2 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len-1);
				return InitSES;
			}
	}
	if(strcmp(tmp, "SN") == 0)
	{
		if(cmdbuf[2]!='?'){			
			len=0;
			while(1){
				if(cmdbuf[2 + len] == 0x0d ){
					
					break;
				}
				len++;		
			}
			ClearElement(q, 3 + len);
			return SSN;
		}
	}
	if(strcmp(tmp, "PM") == 0)
	{
		if(cmdbuf[2]!='?'){			
			len=0;
			while(1){
				if(cmdbuf[2 + len] == 0x0d ){
					
					break;
				}
				len++;		
			}
			ClearElement(q, 3 + len);
			return SPM;
		}
	}
	if(strcmp(tmp, "TM") == 0)
	{
		if(cmdbuf[2]!='?'){			
			len=0;
			while(1){
				if(cmdbuf[2 + len] == 0x0d ){
					
					break;
				}
				len++;		
			}
			ClearElement(q, 3 + len);
			return STM;
		}
	}
	if(strcmp(tmp, "KA") == 0)
	{
		if(cmdbuf[2 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len-1);
				return InitKA;
			}
			else if(cmdbuf[3 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len);
				*para = -*para;
				return InitKA;
			}
	}
	if(strcmp(tmp, "KB") == 0)
	{
		if(cmdbuf[2 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len-1);
				return InitKB;
			}
			else if(cmdbuf[3 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len);
				*para = -*para;
				return InitKB;
			}
	}
	if(strcmp(tmp, "LV") == 0)
	{
		if(cmdbuf[2 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len-1);
				return InitLV;
			}
			else if(cmdbuf[3 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len);
				*para = -*para;
				return InitLV;
			}
	}
	if(strcmp(tmp, "LI") == 0)
	{
		if(cmdbuf[2 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len-1);
				return InitLI;
			}
			else if(cmdbuf[3 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len);
				*para = -*para;
				return InitLI;
			}
	}
	if(strcmp(tmp, "MI") == 0)
	{
		if(cmdbuf[2 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len-1);
				return InitMI;
			}
			else if(cmdbuf[3 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 4 + len);
				*para = -*para;
				return InitMI;
			}
	}
	if(strcmp(tmp, "RK") == 0)
	{
		if(cmdbuf[2 + len] ==0x0d)
		{
			getstrpara(pos, len, strpara);
			ClearElement(q, 4 + len-1);
			return InitRK;
		}
	}
	if(strcmp(tmp, "KP") == 0)
	{
		if(cmdbuf[2 + len] ==0x0d)
		{
			getstrpara(pos, len, strpara);
			ClearElement(q, 4 + len-1);
			return InitKp;
		}
	}
	if(strcmp(tmp, "KI") == 0)
	{
		if(cmdbuf[2 + len] ==0x0d)
		{
			getstrpara(pos, len, strpara);
			ClearElement(q, 4 + len-1);
			return InitKi;
		}
	}
	if(strcmp(tmp, "KD") == 0)
	{
		if(cmdbuf[2 + len] ==0x0d)
		{
			getstrpara(pos, len, strpara);
			ClearElement(q, 4 + len-1);
			return InitKd;
		}
	}
	if(strcmp(tmp, "ID") == 0 && cmdbuf[2+len] == 0x0d)
	{
		getstrpara(pos, len, strpara);
		ClearElement(q, 4 + len-1);
		return InitID;
	}

	memcpy(tmp, cmdbuf, 3);
	if(strcmp(tmp, "ES?")  == 0 && cmdbuf[3] ==0x0d)
	{
			ClearElement(q, 5-1);
			return InitQES;
	}
	if(strcmp(tmp, "SN?")  == 0 && cmdbuf[3] ==0x0d)
	{
			ClearElement(q, 5-1);
			return QSN;
	}
	if(strcmp(tmp, "PM?")  == 0 && cmdbuf[3] ==0x0d)
	{
			ClearElement(q, 5-1);
			return QPM;
	}
	if(strcmp(tmp, "TM?")  == 0 && cmdbuf[3] ==0x0d)
	{
			ClearElement(q, 5-1);
			return QTM;
	}
	if(strcmp(tmp, "KA?")  == 0 && cmdbuf[3] ==0x0d)
	{
			ClearElement(q, 5-1);
			return InitQKA;
	}
	if(strcmp(tmp, "KB?")  == 0 && cmdbuf[3] ==0x0d)
	{
			ClearElement(q, 5-1);
			return InitQKB;
	}
	if(strcmp(tmp, "LV?")  == 0 && cmdbuf[3] ==0x0d)
	{
			ClearElement(q, 5-1);
			return InitQLV;
	}
	if(strcmp(tmp, "LI?")  == 0 && cmdbuf[3] ==0x0d)
	{
			ClearElement(q, 5-1);
			return InitQLI;
	}
	if(strcmp(tmp, "MI?")  == 0 && cmdbuf[3] ==0x0d)
	{
			ClearElement(q, 5-1);
			return InitQMI;
	}
	if(strcmp(tmp, "ID?") == 0 && cmdbuf[3] ==0x0d)
	{
		ClearElement(q, 5-1);
		return QID;
	}
	if(strcmp(tmp, "PD?") == 0 && cmdbuf[3] ==0x0d)
	{
		ClearElement(q, 5-1);
		return QPDpwr;
	}
	if(strcmp(tmp, "LD?")==0 && cmdbuf[3] == 0x0d)
	{
		ClearElement(q, 5-1);
		return QLDsta;
	}
	if(strcmp(tmp, "MAC") == 0)
	{
		if(cmdbuf[3] == '?' && cmdbuf[4] == 0x0d)
		{
			ClearElement(q, 6 -1);
			return QMAC;
		}
		else if( cmdbuf[3+len] == 0x0d)
		{
			getstrpara(pos, len, strpara);
			ClearElement(q, 5 + len-1);
			return InitMAC;
		}
	}
	if(strcmp(tmp, "CRC") == 0)
	{
		if(cmdbuf[3] == '?' && cmdbuf[4] == 0x0d)
		{
			ClearElement(q, 6 -1);
			return QCRC;
		}
		else if( cmdbuf[3+len] == 0x0d)
		{
			getstrpara(pos, len, strpara);
			ClearElement(q, 5 + len-1);
			return SCRC;
		}
	}
	if(strcmp(tmp, "REG") == 0)
	{
		if(cmdbuf[3+len]=='?' && cmdbuf[4+len] == 0x0d)
		{
			getstrpara(pos, len, strpara);
			ClearElement(q, 6 + len - 1);
			return RREG;
		}
		if(cmdbuf[3+len] == 0x0d)
		{
			getstrpara(pos, len , strpara);
			ClearElement(q, 5 + len - 1);
			return WREG;
		}
	}
	
	if(strcmp(tmp, "TEC") == 0)
	{
		if(cmdbuf[3] == '?' && cmdbuf[4] == 0x0d)
		{
			ClearElement(q, 6-1);
			return QTecSet;
		}
		else
		{
			if(cmdbuf[3 + len] == 0x0d && len != 0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 5 + len-1);
				return STecSet;
			}
		}
	}
	
	if(strcmp(tmp,"DIR")==0 && cmdbuf[3] == 0x0d)
	{
		ClearElement(q, 5 -1);
		return TECDIR;
	}
	
	memcpy(tmp, cmdbuf, 4);
	if(strcmp(tmp, "VER?") == 0 && cmdbuf[4] == 0x0d)
	{
		ClearElement(q, 6-1);
		return Qinfor;
	}
	if(strcmp(tmp, "DIR?") == 0 && cmdbuf[4] == 0x0d)
	{
		ClearElement(q, 6-1);
		return QDIR;
	}
	if(strcmp(tmp, "SAVE") == 0 && cmdbuf[4] == 0x0d)
	{
		ClearElement(q, 6-1);
		return SAVE;
	}
	if(strcmp(tmp, "ENLD") == 0 && cmdbuf[4] == 0x0d)
	{
		ClearElement(q, 6-1);
		return LDON;
	}
	if(strcmp(tmp, "ENAP") == 0 && cmdbuf[4] == 0x0d)
	{
		ClearElement(q, 6-1);
		return SENAP;
	}
	if(strcmp(tmp,"PAGE")==0 && cmdbuf[4+len]==0x0d && len!=0)
	{
		*para = getpara(pos,len);
		ClearElement(q, 5+len);
		return PAGECHANGE;
	}
	
	memcpy(tmp, cmdbuf, 5);
	if(strcmp(tmp, "TADJA") == 0)
	{
		if(cmdbuf[5 + len] == 0x0d)
		{
			*para = getpara( pos, len);
			ClearElement(q, 7 + len-1);
			return InitTAdjA;
		}
	}
	if(strcmp(tmp, "TADJB") == 0)
	{
		if(cmdbuf[5 + len] == 0x0d)
		{
			*para = getpara( pos, len);
			ClearElement(q, 7 + len-1);
			return InitTAdjB;
		}
	}

	if(strcmp(tmp, "ENTEC") ==0 && cmdbuf[5]== 0x0d)
	{
		ClearElement(q, 7 -1);
		return TECON;
	}
	if(strcmp(tmp, "ENTMS") ==0 && cmdbuf[5]== 0x0d)
	{
		ClearElement(q, 7 -1);
		return TMSON;
	}
	if(strcmp(tmp, "DISLD") ==0 && cmdbuf[5]== 0x0d)
	{
		ClearElement(q, 7 -1);
		return LDOFF;
	}
	if(strcmp(tmp, "DISAP") ==0 && cmdbuf[5]== 0x0d)
	{
		ClearElement(q, 7 -1);
		return SDISAP;
	}
	if(strcmp(tmp, "BWTMS") ==0 && cmdbuf[5]== 0x0d)
	{
		ClearElement(q, 7-1);
		return SBWTHERMO;
	}

	
	memcpy(tmp, cmdbuf, 6);	
	if(strcmp(tmp, "HMLOAD") == 0 && cmdbuf[6] == 0x0d)
	{
		ClearElement(q, 8-1);
		return HMILOAD;
	}
	if(strcmp(tmp, "DISTEC") == 0 && cmdbuf[6] == 0x0d)
	{
		ClearElement(q, 8-1);
		return TECOFF;
	}
	if(strcmp(tmp, "DISTMS") == 0 && cmdbuf[6] == 0x0d)
	{
		ClearElement(q, 8-1);
		return TMSOFF;
	}
	if(strcmp(tmp, "CANCEL") == 0 && cmdbuf[6] == 0x0d)
	{
		ClearElement(q, 8-1);
		return CANCEL;
	}
	if(strcmp(tmp, "PARAM?") == 0 && cmdbuf[6] == 0x0d)
	{
		ClearElement(q, 8-1);
		return QPARA;
	}
	
	memcpy(tmp, cmdbuf, 7);
	if(strcmp(tmp, "RSETSYS") == 0 && cmdbuf[7] == 0x0d)
	{
		ClearElement(q, 9-1);
		return RstSys;
	}
	if(strcmp(tmp, "TARGET?") == 0)
	{
		if(cmdbuf[7] == 0x0d)
		{
			ClearElement(q, 9 -1);
			return QTARGET;
		}
	}
	if(strcmp(tmp, "HARDID?") == 0 && cmdbuf[7] == 0x0d)
	{
		ClearElement(q, 9-1);
		return QHARDID;
	}
	
	memcpy(tmp, cmdbuf, 8);
	if(strcmp(tmp, "REGISTRY") == 0 && cmdbuf[8] == 0x0d)
	{
		ClearElement(q, 10-1);
		return Reg;
	}
	if(strcmp(tmp, "AUTO-RUN") == 0)
	{
		if(cmdbuf[8] == '?' && cmdbuf[9] == 0x0d)
		{
			ClearElement(q, 11 -1);
			return QAUTO;
		}
		if(cmdbuf[8 + len] == 0x0d && len!= 0)
		{
			*para = getpara( pos, len);
			ClearElement(q, 10 + len-1);
			return SAUTO;
		}
	}
	if(strcmp(tmp, "I_PRESET") == 0)
	{
		if(cmdbuf[8] == '?' && cmdbuf[9] == 0x0d)
		{
			ClearElement(q, 11 -1);
			return QPREC;
		}
		if(cmdbuf[8 + len] == 0x0d)
		{
			if(len!=0)
			{
				*para = getpara( pos, len);
				ClearElement(q, 10 + len-1);
				return InitPreC;
			}
		}
	}
	if(strcmp(tmp, "T_PRESET") == 0)
	{
		if(cmdbuf[8] == '?' && cmdbuf[9] == 0x0d)
		{
			ClearElement(q, 11 -1);
			return QPRET;
		}
		if(cmdbuf[8 + len] == 0x0d && len!= 0)
		{
			*para = getpara( pos, len);
			ClearElement(q, 10 + len-1);
			return InitPreT;
		}
	}
	
	memcpy(tmp, cmdbuf, 9);
	if(strcmp(tmp, "SELFCHECK") == 0 && cmdbuf[9] == 0x0d)
	{
		ClearElement(q, 11-1);
		return SelfChk;
	}
	if(strcmp(tmp, "T_TARGET?") == 0 && cmdbuf[9] == 0x0d)
	{
		ClearElement(q, 11-1);
		return QTARTEMP;
	}
	if(strcmp(tmp, "I_TARGET?") == 0 && cmdbuf[9] == 0x0d)
	{
		ClearElement(q, 11-1);
		return QTARCUR;
	}
	if(strcmp(tmp, "T_PARAM_A") == 0 && len!=0)
	{
		if(cmdbuf[9 + len] == 0x0d)
		{
			*para = getpara( pos, len);
			ClearElement(q, 11 + len-1);
			return InitTAdjA;
		}
	}
	if(strcmp(tmp, "T_PARAM_B") == 0 && len!=0)
	{
		if(cmdbuf[9 + len] == 0x0d)
		{
			*para = getpara( pos, len);
			ClearElement(q, 11 + len-1);
			return InitTAdjB;
		}
	}	
	memcpy(tmp, cmdbuf, 11);
	
	if(strcmp(tmp, "DISP_EEPROM") ==0 && cmdbuf[11] == 0x0d)
	{
		ClearElement(q, 13 -1);
		return Qinit;
	}

	ClearElement(q, strlen(cmdbuf));
	return UNKNOWN;
}

float getpara(unsigned char p, unsigned char len)
{
	memset(tmp, 0x0, 24);
	strncpy(tmp, cmdbuf + p, len);
	if(len == 0)strcpy(tmp, "0");
	return atof(tmp);
}

void getstrpara(unsigned char p, unsigned char len, char* para)
{
	memset(para, 0x0, 24);
	strncpy(para, cmdbuf + p, len);
}

void countCoNum(unsigned char* p, unsigned char* len)
{
	hexIn = 0;
	idx = 0;
	*p = 0;
	*len = 0;
	if(strstr(cmdbuf,"0X")!=0)hexIn=1;
	for(; idx < strlen(cmdbuf); idx++)
	{
		if(idx ==0 && strstr(cmdbuf,"REG")!=0)
		{
			idx = 3;
		}
		if(idx ==0 && strstr(cmdbuf,"ID")!=0)
		{
			idx = 2;
		}
		if(idx ==0 && strstr(cmdbuf,"DB25")!=0)
		{
			idx = 4;
		}
		//if(cmdbuf[idx]==',' || cmdbuf[idx]==':')hexIn =1;
		if(*p == 0)
		{
			if(cmdbuf[idx] >= '0' && cmdbuf[idx] <= '9')
			{
				*p = idx;
				(*len)++;
			}
			else if(hexIn == 1)
			{
				if(cmdbuf[idx]>='A' && cmdbuf[idx]<='F')
				{
					*p = idx;
					(*len)++;
				}
			}
		}
		else
		{
			if((cmdbuf[idx] >= '0' && cmdbuf[idx] <= '9') || cmdbuf[idx] == '.' || cmdbuf[idx] == ',')
			{
				(*len)++;
			}
			else if(hexIn == 1)
			{
				if((cmdbuf[idx]>='A' && cmdbuf[idx]<='F') || cmdbuf[idx]=='X')
				(*len)++;
			}
			else
				break;
		}
	}
}




//-------------------------
//register r/w
//
//-------------------------
void rqtwReg(char channel,unsigned char* strpara)
{
	unsigned char addr;
	unsigned char dat[8];
	unsigned char len;
	unsigned char chlen;
	char split = 0;
	memset(dat, 0x0, 8);
	retRqtHead(COMM_UART);
	for(idx = 0;idx <strlen((char*)strpara); idx++)
	{
		if(strpara[idx] == ',')
		{
			split = idx;
			break;
		}
	}
	if(split == 0)
	{
		strcpy(tmp,"plz input value");
		SendStr((unsigned char*)tmp, channel);		
		return;
	}
	/*memset(tmp, 0x0, 24);
	memcpy(tmp, strpara+split +1, 2);
	if(strcmp(tmp,"0X")!=0)
	{
		strcpy(tmp,"plz input Hex value");
		SendStr((unsigned char*)tmp);		
		return;
	}*/
	memset(tmp, 0x0, 24);
	memcpy(tmp, strpara, split);
	addr = atoi(tmp);
	memset(tmp, 0x0, 24);
	if(hexIn == 1)
	{
		memcpy(tmp, strpara + split + 3, strlen((char*)strpara) - split -2);
		chlen = strlen(tmp);
		len = chlen/2.0+0.5;
		hexstr2ch(tmp, (char*)dat, len);
	}
	else
	{
		memcpy(tmp,strpara + split +1, strlen((char*)strpara) - split);
		i = atoi(tmp);
		dat[0] = i>>8;
		dat[1] = i;
		chlen = 4;
	}
	writeReg(addr, dat, chlen);
	if(hexIn == 1)
	{
		sprintf(temp,"write reg%d,0x%s",(int)addr,tmp);
	}
	else
	{
		sprintf(temp,"write reg%d,%d",(int)addr,i);
	}
	SendStr((unsigned char*)temp, channel);
}
void rqtrReg(char channel,unsigned char* strpara)
{
	//todo
	unsigned char addr;
	unsigned char split=0;
	unsigned char dat[8]; 
	unsigned char len;
	retRqtHead(channel);
	for(idx = 0;idx <strlen((char*)strpara); idx++)
	{
		if(strpara[idx] == ',')
		{
			split = idx;
			break;
		}
	}
	if(split == 0)
	{
		len = 1;
		memset(tmp, 0x0, 24);
		memcpy(tmp, strpara, strlen((char*)strpara));
		addr = atoi(tmp);
	}
	else
	{
		memset(tmp, 0x0, 24);
		memset(dat, 0x0, 8);
		memcpy(tmp, strpara, split);
		addr = atoi(tmp);
		memset(temp, 0x0, 24);
		memcpy(temp, strpara + split + 1, strlen((char*)strpara) - split -1 );
		len = temp[0] - '0';
		if(len>4)len=4;
		if(len<0)return;
	}
	readReg(addr, dat, len);
	strcat(tmp, "RegValue:");
	SendStr((unsigned char*)tmp,channel);
	memset(tmp, 0x0, 24);
	hex2str((char*)dat, tmp, len*2);
	//strcpy(temp,"0x");
	//strcat(temp, tmp);
	sprintf(temp,"0x%s",tmp);
	SendStr((unsigned char*)temp,channel);
}

void getver(char channel)
{
	temp[0] = Version;
	memset(tmp,0x0,4);
	hex2str(temp, tmp, 1);
	strcpy(temp, "Prog_ver:");
	strncat(temp,tmp,2);
	SendStr((unsigned char*)temp, channel);
	temp[0] = Reversion>>8;
	temp[1] = Reversion&0xff;
	hex2str(temp, tmp, 2);
	strcpy(temp, " Prog_rev:");
	strncat(temp,tmp,4);
	SendStr((unsigned char*)temp, channel);
}
void rqtQinfor(char channel)
{
	retRqtHead(channel);
	getver(channel);
	retRqtHead(channel);
}

void rqtSTem(char channel, float para)
{
	unsigned char flag=0;
	if(para > HIGHTEMLD){
		para = HIGHTEMLD;
		flag = 1;
	}
	if(para < LOWTEMLD){
		para = LOWTEMLD;
		flag = 1;
	}
	sprintf(tmp, "GS_T%.3f", para);
	if((CommandCRC != 0)&&(CommandCRC != CalCRC(tmp,strlen(tmp)))){
		retRqtHead(channel);
		sprintf(tmp, "CRCError:%d",CalCRC(tmp,strlen(tmp)));
		SendStr((unsigned char*)tmp, channel);	
		return;
	}
	thermoPara_LD.T_Target = GetADCWithTemp(para);	
	if(channel != HMI_UART){
		retRqtHead(channel);
		sprintf(temp, "LD Temp sets to %.2f", para);
		SendStr((unsigned char*)temp,channel);
		sprintf(temp, "tLdTartemp.txt=\"%.2f\"", para);
		SendStr((unsigned char*)temp,HMI_UART);	
		hmi_end(HMI_UART);
	}
	if(channel == HMI_UART){
		if(flag){
			sprintf(temp, "page error");
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);
			return ;
		}
		else{
			sprintf(temp, "page main");
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);
			sprintf(temp, "tLdTartemp.txt=\"%.2f\"", para);
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);
		}
	}	
}

void rqtQTem(char channel)
{
	f = GetTemp();
	if(DEBUG_UART == channel){
		retRqtHead(channel);
		sprintf(tmp,"TEMP:%.2f",f);
		SendStr((unsigned char*)tmp,channel);	
	}
	else if(COMM_UART == channel){
		retRqtHead(channel);
		sprintf(tmp,"TEMP:%.2f",f);
		SendStr((unsigned char*)tmp,channel);	
	}
	else{
		sprintf(tmp, "tLdtemp.txt=\"%.3f\"",f);
		SendStr((unsigned char*)tmp,channel);	
		hmi_end(channel);
		switch(objectS)
		{
			case 1:		sprintf(tmp, "show.s3.txt=\"%.2fC\"",f);
								SendStr((unsigned char*)tmp,channel);	
								hmi_end(channel);
								break;
			case 2:		sprintf(tmp, "show.s3.txt=\"%.2fC\"",f);
								SendStr((unsigned char*)tmp,channel);	
								hmi_end(channel);
								break;
			case 3:		sprintf(tmp, "show.s1.txt=\"%.2fC\"",f);
								SendStr((unsigned char*)tmp,channel);	
								hmi_end(channel);
								break;
			default:break;
		}
	}
}
void noIdentify(char channel)
{
	sprintf(temp, "page error");
		SendStr((unsigned char*)temp, COMM_UART);
		hmi_end(channel);
	if(channel==HMI_UART)
	{
		sprintf(temp, "page error");
		SendStr((unsigned char*)temp, channel);
		hmi_end(channel);
	}
	else
	{
		retRqtHead(channel);
		sprintf(temp, "para over range");
		SendStr((unsigned char*)temp, channel);
	}
}
void paraOverRange(char channel)
{
	if(channel==HMI_UART)
	{
		sprintf(temp, "page error");
		SendStr((unsigned char*)temp, channel);
		hmi_end(channel);
	}
	else
	{
		retRqtHead(channel);
		sprintf(temp, "para over range");
		SendStr((unsigned char*)temp, channel);
	}
}
void wrongFormat(char channel)
{
	retRqtHead(channel);
	sprintf(temp, "wrong command format");
	SendStr((unsigned char*)temp, channel);
}

void CRCerror(char channel)
{
	retRqtHead(channel);
	strcpy(temp,"CRC check error");
	SendStr((unsigned char*)temp,channel);
}


//float GetLDI(void)
//{
//	unsigned int ab;
//	unsigned int ac;
//	float dl;
//	ac = ConReadADC(A_RealLDCur);
//	ab 	= ConReadADC(A_RealLDCur);
//	if(ab > 32767)
//		ab= 0;
//	dl = ((ab- base_LDI_ADC)/32767.0*3)*10;
//	if(dl < 0 || dl>32767)
//	{
//		dl=0;
//	}
////	sprintf(tmp,"LD cur is:%.3f %d %d\n",dl,ac,ab);
////		SendStr((unsigned char*)tmp, COMM_UART);
//	return dl;
//	
//}
void rqtQLDI(char channel)
{
	f = GetCurrent();
	if(channel == DEBUG_UART)
	{
		retRqtHead(channel);
		sprintf(tmp,"LD cur is:%.3f",f);
		SendStr((unsigned char*)tmp, channel);
	}
	if(channel == COMM_UART){
		retRqtHead(channel);
		sprintf(tmp,"LD cur is:%.3f",f);
		SendStr((unsigned char*)tmp, channel);
	}
	if(channel == HMI_UART)
	{
		sprintf(tmp, "tLdcurrent.txt=\"%.3f\"",f);
		SendStr((unsigned char*)tmp, channel);
		hmi_end(HMI_UART);
		switch(objectS)
		{
			case 1:		sprintf(tmp, "show.s2.txt=\"%.2fA\"",f);
								SendStr((unsigned char*)tmp,channel);	
								hmi_end(channel);
								break;
			case 2:		sprintf(tmp, "show.s1.txt=\"%.2fA\"",f);
								SendStr((unsigned char*)tmp,channel);	
								hmi_end(channel);
								break;
			case 3:		sprintf(tmp, "show.s2.txt=\"%.2fA\"",f);
								SendStr((unsigned char*)tmp,channel);	
								hmi_end(channel);
								break;
			default:break;
		}
	}
}
void rqtSLDI(char channel,float para)
{
	unsigned char flag=0;
	float buff=0;
	#if VersionHPower
	#else
	if(!LDstatus()){
		if(channel != HMI_UART){
			retRqtHead(channel);
			sprintf(tmp, "LD is off", para);
			SendStr((unsigned char*)tmp, channel);	
		}
		else if(channel == HMI_UART){
		 sprintf(temp, "page main");
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);	
		}
		return ;
	}
	#endif
	if(para>MaxCurrent)
	{
		flag=1;
		para = MaxCurrent;
	}
		//CRC
	sprintf(tmp, "GS_I%.3f", para);
	if((CommandCRC != 0)&&(CommandCRC != CalCRC(tmp,strlen(tmp)))){
		retRqtHead(channel);
		sprintf(tmp, "CRCError");
		SendStr((unsigned char*)tmp, channel);	
		return;
	}
	i = GetDACValueWithCurrent(para);
	#if VersionHPower
	GPIO_WriteBit(HW_GPIOB, 2,1);      //turn the las pin on add wt
	#endif
	writeDAC(D_ChCur,i);	
	LD_para.curctrlflag = 0;
	LD_para.I_Target = i;
	if(channel != HMI_UART){
		retRqtHead(channel);
		sprintf(tmp, "LD cur sets to:%.2f dac:%d", para,i);
		SendStr((unsigned char*)tmp, channel);
		sprintf(tmp, "tLdTarcurrent.txt=\"%.2f\"", para);
		SendStr((unsigned char*)tmp, HMI_UART);
		hmi_end(HMI_UART);	
		buff = GetPowerWithCurrent(para);
		sprintf(temp, "tLdTargetpower.txt=\"%.2f\"", buff);
		SendStr((unsigned char*)temp,HMI_UART);	
		hmi_end(HMI_UART);		
	}
	if(channel == HMI_UART){
		if(flag){
			sprintf(temp, "page error");
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);
			return ;
		}
		else{
			sprintf(temp, "page main");
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);			
			sprintf(tmp, "tLdTarcurrent.txt=\"%.2f\"", para);
			SendStr((unsigned char*)tmp, HMI_UART);
			hmi_end(HMI_UART);
			buff = GetPowerWithCurrent(para);
			sprintf(temp, "tLdTargetpower.txt=\"%.2f\"", buff);
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);
			
		}
	}

}

void rqtQTec(char channel)
{
	retRqtHead(channel);
	SendStr((unsigned char*)tmp, channel);
	if(thermoPara_LD.en_tec == 1)
	{
		strcpy(tmp, " TEC auto adjust is on;");
	}
	else
	{
		strcpy(tmp," TEC auto adjust is off;");
	}
	SendStr((unsigned char*)tmp, channel);
	if(1 == ReadByteFromEEprom(W_THERMO))
	{
		if(GPIO_ReadBit(TEC_DIR) == 1)		//0 -> 1 by wt
		{
			sprintf(tmp, " dir heating");
		}
		else
		{
			sprintf(tmp, " dir cooling");
		}
	}
	else
		sprintf(tmp, " only cooling mode");
	SendStr((unsigned char*)tmp, channel);
		
		
	sprintf(tmp," TEC set is %.2f", GetTECS());
	SendStr((unsigned char*)tmp, channel);
	
	f = GetTecCurr(); 
	if(channel == DEBUG_UART)
	{
		sprintf(tmp," TEC real current is:%.2f    %.2fV",f,i/65535.0*3);
	}
	else
		sprintf(tmp," TEC real current is:%.2f ",f);
	SendStr((unsigned char*)tmp, channel);
	f = GetTecV(); 
	if(channel == DEBUG_UART)
	{
		sprintf(tmp," TEC real voltage is:%.2f    %.2fV",f,i/65535.0*3);
	}
	else
		sprintf(tmp," TEC real voltage is:%.2f ",f);
	SendStr((unsigned char*)tmp, channel);
	
}

void rqtSTec(char channel,float para)
{
	retRqtHead(channel);
	if(para>1 || para<0)
	{
		paraOverRange(channel);
		return;
	}
	i = para * 10000;
	if((thermoPara_LD.en_tec | 0x0) ==0)
	{
		sprintf(tmp,"TEC sets to:%.2f",para);	
		thermoPara_LD.TEC_Dac = i;
		//writeDAC(D_ChTec, i);
		FTM_PWM_ChangeDuty(thermoPara_LD.ftmHW, thermoPara_LD.ftmCh, i);
	}
	else
		strcpy(tmp, "failed, Thermo is running");
	
	SendStr((unsigned char*)tmp, channel);
	
}
void rqtQDIRswitch(char channel)
{
	char dir = ReadByteFromEEprom(W_THERMO);
	if(!dir){
		strcpy(temp,"now TEC_DIR:one-way");
		SendStr((unsigned char*)temp, channel);
	}
	else{
		strcpy(temp,"now TEC_DIR:two-way");
		SendStr((unsigned char*)temp, channel);
	}
}
void rqtDIRswitch(char channel)
{
	char dir = 0;
	retRqtHead(channel);
	GPIO_ToggleBit(TEC_DIR);    
	dir = ReadByteFromEEprom(W_THERMO);
	if(dir){
		WriteByteFromEEprom(W_THERMO,0);
		strcpy(temp,"now TEC_DIR:one-way");
		SendStr((unsigned char*)temp, channel);
	}
	else{
		strcpy(temp,"now TEC_DIR:two-way");
		SendStr((unsigned char*)temp, channel);
		WriteByteFromEEprom(W_THERMO,1);
	}
//	strcpy(temp,"now TEC_DIR:");
//	SendStr((unsigned char*)temp, channel);
//	if(GPIO_ReadBit(TEC_DIR) == 1 )		//0 -> 1 by wt
//	{
//		strcpy(temp,"heating");
//		SendStr((unsigned char*)temp, channel);
//	}
//	else
//	{
//		strcpy(temp,"cooling");
//		SendStr((unsigned char*)temp, channel);
//	}	
}

void initID(char* strpara)
{
	char id;
	if(hexIn == 1)
	{
		memcpy(tmp, strpara + 2, 2);
		hexstr2ch(tmp, &id, 1);
	}
	else
	{
		memcpy(tmp,strpara, strlen(strpara));
		i = atoi(tmp);
		id = i;
	}
	//hexstr2ch(strpara, &id, 1);
	WriteByteFromEEprom(CAN_ID,id);
	retRqtHead(COMM_UART);
	sprintf(temp, "ID sets to:%s",strpara);
	SendStr((unsigned char*)temp,COMM_UART);
}
void initMAC(char* strpara)
{
	char id[4];
	long int li;
	if(strlen(strpara)==0)
	{
		strcpy(temp, "plz input value");
		SendStr((unsigned char*)temp, COMM_UART);
		return;
	}
	if(hexIn == 1)
	{
		reverse(strpara);
		hexstr2ch(strpara, id, 4);
		hex_reverse(id, 4);
	}
	else
	{
		memcpy(tmp,strpara , strlen(strpara));
		li = atoi(tmp);
		id[0] = i>>8;
		id[1] = i;
	}
	WriteIntFromEEprom(GS_MAC,li);
	retRqtHead(COMM_UART);
	sprintf(temp, "MAC sets to:%s",strpara);
	SendStr((unsigned char*)temp, COMM_UART);
}
void rqtAPSyncLD(char channel,unsigned char c)
{
	MOENLD = c;
	WriteByteFromEEprom(MOSynLD,c);
	retRqtHead(COMM_UART);
	if(c)
		sprintf(temp, "EN AP");
	else
		sprintf(temp, "DIS AP");
	SendStr((unsigned char*)temp, COMM_UART);

}
void rqtLDSwitch(char channel, unsigned char c)
{
	if(c ==1 )
	{
		LDswitch(channel,1);
	}
	else
	{
		LDswitch(channel,0);
	}		
		if(c){
			if(language)
			sprintf(temp,"b_ldswitch.txt=\"LD ON\"");
		else
			sprintf(temp,"b_ldswitch.txt=\"LD 开\"");	
			SendStr((unsigned char*)temp, HMI_UART);
			hmi_end(HMI_UART);
			sprintf(temp,"now LD EN_LD:enable");		
		}
		else{
			if(language)
			sprintf(temp,"b_ldswitch.txt=\"LD OFF\"");
		else
			sprintf(temp,"b_ldswitch.txt=\"LD 关\"");	
			SendStr((unsigned char*)temp, HMI_UART);
			hmi_end(HMI_UART);
			sprintf(temp,"now LD EN_LD:diable");
			
		}
		if(channel != HMI_UART){
			retRqtHead(channel);
			SendStr((unsigned char*)temp, channel);
		}
}

void initPreC(char channel,float para)
{
	int j;
	if(para > MaxCurrent || para <0)
	{
		paraOverRange(channel);
		return;
	}
	j = GetDACValueWithCurrent(para);
	WriteIntFromEEprom(PRE_CUR,j);
	if(channel != HMI_UART){
		retRqtHead(channel);
		sprintf(temp, "LD Current presets to %.2f %d %d %.2f", para,j,base_LDI_DAC,ConReadADC(A_RealDACRef));
	  SendStr((unsigned char*)temp, channel);
	}	
}
void initPreT(char channel,float para)
{
	int j;	
		if(para > HIGHTEMLD){
			para = HIGHTEMLD;
			paraOverRange(channel);
			return;
		}
		if(para < LOWTEMLD){
			para = LOWTEMLD;
			paraOverRange(channel);
			return;
		}
		j = GetADCWithTemp(para);
		WriteIntFromEEprom(PRE_TEM,j);
		if(channel != HMI_UART){
			retRqtHead(COMM_UART);
			sprintf(temp, "LD presets to %.2f", para);
			SendStr((unsigned char*)temp, COMM_UART);	
	}
}
void initMI(char channel,float para)
{

	WriteFloatFromEEprom(E_MI,para);
	retRqtHead(channel);
	sprintf(temp, "Max current sets to %f",para);
	SendStr((unsigned char*)temp,channel);

}
void initQMI(char channel)
{
	retRqtHead(channel);
	sprintf(temp, "Sys:%.1f Cal:%.3f User:%.3f Curr:%.3f ",SystemMaxCurr,GetCurrentWidthPower(MaxPower),ReadFloatFromEEprom(E_MI),MaxCurrent);
	SendStr((unsigned char*)temp,channel);
}
void initLI(char channel,float para)
{
	WriteFloatFromEEprom(E_LI,para);

}
void initQLI(char channel)
{
	retRqtHead(channel);
	sprintf(temp, "curr step:%f",ReadFloatFromEEprom(E_LI));
	SendStr((unsigned char*)temp,channel);
}
void initLV(char channel,float para)
{
	WriteFloatFromEEprom(MaxLDVLimit,para);
	retRqtHead(channel);
	sprintf(temp, "MaxLDVLimit sets to %f",para);
	SendStr((unsigned char*)temp,channel);
}
void initQLV(char channel)
{
	retRqtHead(channel);
	sprintf(temp, "MaxLDVLimit:%f",ReadFloatFromEEprom(MaxLDVLimit));
	SendStr((unsigned char*)temp,channel);
}
void initTM(char channel, char* strpara)
{
	retRqtHead(channel);
	SendStr((unsigned char*)cmdbuf,channel);
	WriteStrFromEEprom(TM,cmdbuf+2,20);
}
void rqtQTM(char channel)
{
	retRqtHead(channel);
	ReadStrFromEEprom(TM,temp,20);
	SendStr((unsigned char*)temp,channel);
}
void initPM(char channel, char* strpara)
{
	retRqtHead(channel);
	SendStr((unsigned char*)cmdbuf,channel);
	WriteStrFromEEprom(PM,cmdbuf+2,20);
}
void rqtQPM(char channel)
{
	retRqtHead(channel);
	ReadStrFromEEprom(PM,temp,20);
	SendStr((unsigned char*)temp,channel);
}
void initSN(char channel, char* strpara)
{
	retRqtHead(channel);
	SendStr((unsigned char*)cmdbuf,channel);
	WriteStrFromEEprom(SN,cmdbuf+2,20);
}
void rqtQSN(char channel)
{
	retRqtHead(channel);
	ReadStrFromEEprom(SN,temp,20);
	SendStr((unsigned char*)temp,channel);
}
void initCRC(char channel, char* strpara)
{
	int j;
	if(strlen(strpara)==0)
	{
		strcpy(temp, "plz input value");
		SendStr((unsigned char*)temp, channel);
		return;
	}
	memset(tmp, 0x0, 24);
	retRqtHead(channel);
	j = atoi(strpara);
	CommandCRC = j;
	sprintf(temp, "CRC sets to %d",CommandCRC);
	SendStr((unsigned char*)temp,channel);

}
void rqtQCRC(char channel)
{
	retRqtHead(channel);
	sprintf(temp, "CRC:%d",CommandCRC);
	SendStr((unsigned char*)temp,channel);
}

void initQKA(char channel)
{
	retRqtHead(channel);
	sprintf(temp, "Arga:%f",ReadFloatFromEEprom(PRE_ArgA));
	SendStr((unsigned char*)temp,channel);
}
void initQES(char channel,float para)
{
	unsigned char tec = ReadByteFromEEprom(TECStrength);
	retRqtHead(channel);
	sprintf(temp, "TEC MAX:EEPROM:%d  System:%d",tec,T_EMAX/100);
	SendStr((unsigned char*)temp,channel);

}
void initSES(char channel,float para)
{
	unsigned char tec = (unsigned char)para;
	if(tec*100>SystemTEClimit){
		retRqtHead(channel);
		sprintf(temp, "error:TEC MAX is:%d",SystemTEClimit/100);
		SendStr((unsigned char*)temp,channel);
		return ;
	}
	WriteByteFromEEprom(TECStrength,tec);	
	retRqtHead(channel);
	sprintf(temp, "TEC MAX:%d",tec);
	SendStr((unsigned char*)temp,channel);
}
void initQKB(char channel)
{
	retRqtHead(channel);
	sprintf(temp, "ArgB:%f",ReadFloatFromEEprom(PRE_ArgB));
	SendStr((unsigned char*)temp,channel);
}
void initKA(char channel,float para)
{
	WriteFloatFromEEprom(PRE_ArgA,para);
	retRqtHead(channel);
	sprintf(temp, "ArgA sets to %f",para);
	SendStr((unsigned char*)temp,channel);
}
void initKB(char channel,float para)
{
	WriteFloatFromEEprom(PRE_ArgB,para);
	retRqtHead(channel);
	sprintf(temp, "ArgB sets to %f",para);
	SendStr((unsigned char*)temp,channel);
}
void initKp(char channel,char* strpara)
{
	int j;
	if(strlen(strpara)==0)
	{
		strcpy(temp, "plz input value");
		SendStr((unsigned char*)temp, channel);
		return;
	}
	memset(tmp, 0x0, 24);
	retRqtHead(channel);
	j = atoi(strpara);
	WriteIntFromEEprom(KP,j);
//	decstr2ch(strpara, tmp);
	thermoPara_LD.T_Kp = j;
	
//	writeKp(tmp);
	sprintf(temp, "LD KP sets to %d",j);
	
	
	
	SendStr((unsigned char*)temp,channel);
}
void initKi(char channel,char* strpara)
{
	int j;
	if(strlen(strpara)==0)
	{
		strcpy(temp, "plz input value");
		SendStr((unsigned char*)temp, channel);
		return;
	}
	memset(tmp, 0x0, 24);
	retRqtHead(channel);
	j = atoi(strpara);
	WriteIntFromEEprom(KI,j);
//	decstr2ch(strpara, tmp);
	thermoPara_LD.T_Ki = j;
//	writeKi(tmp);
	sprintf(temp, "LD KI sets to %d",j);
	SendStr((unsigned char*)temp, channel);
}
void initKd(char channel,char* strpara)
{
	int j;
	if(strlen(strpara)==0)
	{
		strcpy(temp, "plz input value");
		SendStr((unsigned char*)temp, channel);
		return;
	}
	memset(tmp, 0x0, 24);
	retRqtHead(channel);
	j = atoi(strpara);
	WriteIntFromEEprom(KD,j);
//	decstr2ch(strpara, tmp);
	thermoPara_LD.T_Kd = j;
//	writeKd(tmp);
	sprintf(temp, "LD KD sets to %d",j);
	SendStr((unsigned char*)temp, channel);
}


void rqtQKp()
{
	i = ReadIntFromEEprom(KP);
	sprintf(temp, "KP:%d",i);
	SendStr((unsigned char*)temp, COMM_UART);
}
void rqtQKi()
{
	i = ReadIntFromEEprom(KI);
	sprintf(temp, "KI:%d",i);
	SendStr((unsigned char*)temp, COMM_UART);
}
void rqtQKd()
{
	i = ReadIntFromEEprom(KD);
	sprintf(temp, "KD:%d",i);
	SendStr((unsigned char*)temp, COMM_UART);
}



void initTadjA(float para)
{
	int j;
	memset(tmp, 0x0, 16);
	j = para *1000;
	tmp[0] = j;
	tmp[1] = j>>8;
	writeTemAdjA(tmp);
	retRqtHead(COMM_UART);
	sprintf(temp, "T_Param_a set to:%.2f", para);
	SendStr((unsigned char*)temp, COMM_UART);
}
void initTadjB(float para)
{
	int j;
	memset(tmp, 0x0, 16);
	j = para *1000;
	tmp[0] = j;
	tmp[1] = j>>8;
	writeTemAdjB(tmp);
	retRqtHead(COMM_UART);
	sprintf(temp, "T_Param_b set to:%.2f", para);
	SendStr((unsigned char*)temp, COMM_UART);
}

void rqtQinit(char channel)
{
	unsigned int j;
	char buff = 0;

	retRqtHead(channel);

	buff = ReadByteFromEEprom(CAN_ID);
	sprintf(temp,"ID:%d",buff);
	SendStr((unsigned char*)temp, channel);


	j = ReadIntFromEEprom(PRE_CUR);
	sprintf(tmp,"%d",j);
	strcpy(temp, "  I_PRESET:");
	strcat(temp, tmp);
	SendStr((unsigned char*)temp, channel);

	j = ReadIntFromEEprom(PRE_TEM);
	sprintf(tmp,"%d",j);
	strcpy(temp, "  T_PRESET:");
	strcat(temp, tmp);
	SendStr((unsigned char*)temp, channel);

	j = ReadIntFromEEprom(KP);
	sprintf(tmp,"%d",j);
	strcpy(temp, "  KP:");
	strcat(temp, tmp);
	SendStr((unsigned char*)temp, channel);

	j = ReadIntFromEEprom(KI);
	sprintf(tmp,"%d",j);
	strcpy(temp, "  KI:");
	strcat(temp, tmp);
	SendStr((unsigned char*)temp, channel);

	j = ReadIntFromEEprom(KD);
	sprintf(tmp,"%d",j);
	strcpy(temp, "  KD:");
	strcat(temp, tmp);
	SendStr((unsigned char*)temp, channel);

//	readTemAdjA(ch);
//	j = ch2dec(ch);
//	sprintf(tmp,"%d",j);
//	strcpy(temp, "  T_PARAM_A:");
//	strcat(temp, tmp);
//	SendStr((unsigned char*)temp, channel);
//	
//	readTemAdjB(ch);
//	j = ch2dec(ch);
//	sprintf(tmp,"%d",j);
//	strcpy(temp, "  T_PARAM_B:");
//	strcat(temp, tmp);
//	SendStr((unsigned char*)temp, channel);
	
}

void rqtTarget(char channel)
{
	float buff = 0;
	float tartemp = 0;
	f = GetTargetCurrentWithDAC();
	if(f < 0)
	{
		f = 0;
	}
	tartemp = GetTarTemp();
	if(channel == HMI_UART){
			if(LDstatus())
			{
				if(!language)
					sprintf(temp,"main.b_ldswitch.txt=\"LD 开\"");
				else
					sprintf(temp,"main.b_ldswitch.txt=\"LD ON\"");
			}
			else
			{
				if(!language)
					sprintf(temp,"main.b_ldswitch.txt=\"LD 关\"");
				else
				sprintf(temp,"main.b_ldswitch.txt=\"LD OFF\"");
			}
			SendStr((unsigned char*)temp,channel);
			hmi_end(channel);
			
			sprintf(temp,"main.tLdTarcurrent.txt=\"%.2f\"",f);
			SendStr((unsigned char*)temp,channel);
			hmi_end(HMI_UART);
			buff = GetPowerWithCurrent(f);
			sprintf(temp,"main.tLdTargetpower.txt=\"%.2f\"",buff);
			SendStr((unsigned char*)temp,channel);
			hmi_end(channel);
			sprintf(temp,"main.tLdTartemp.txt=\"%.2f\"",tartemp);
			SendStr((unsigned char*)temp,channel);
			hmi_end(HMI_UART);
			char t[20];
			ReadStrFromEEprom(PM,t,20);
			sprintf(temp,"main.mode.txt=\"%s\"",t);
			SendStr((unsigned char*)temp,channel);
			hmi_end(HMI_UART);
			sprintf(temp,"c0.val=%d",Autorun);
			SendStr((unsigned char*)temp,channel);
			hmi_end(channel);
	}
	else{
		retRqtHead(channel);
		
		sprintf(temp, " LD:%d", LDstatus());
		SendStr((unsigned char*)temp, channel);
		sprintf(temp, " TEC:%d", thermoPara_LD.en_tec);
		SendStr((unsigned char*)temp, channel);
		
		sprintf(temp, " TargetCurrent:%.2f", f);
		SendStr((unsigned char*)temp, channel);
		
		sprintf(temp, " TargetTemp:%.2f",tartemp);
		SendStr((unsigned char*)temp, channel);
	}
}
void rqtSAUTO(char channel,float para)
{
	if(para<1){
		Autorun = 0;		
		sprintf(temp, "\ndisable autorun");
	}
	else{
		Autorun = 1;
		sprintf(temp, "\nenable autorun");
	}
	WriteByteFromEEprom(AUTORUN,Autorun);
	SendStr((unsigned char*)temp, COMM_UART);
	char ss = ReadByteFromEEprom(AUTORUN);
	printf("%s\n",temp);
}
void rqtQAUTO(char channel)
{
	if(channel != HMI_UART){
		retRqtHead(channel);
		if(Autorun)
			sprintf(tmp, "autorun is on");
		else
			sprintf(tmp, "autorun is off");
		SendStr((unsigned char*)tmp, channel);
		return;
	}
	if(Autorun)
			sprintf(tmp, "autorun.pco=2016");
	else
			sprintf(tmp, "autorun.pco=33808");
  SendStr((unsigned char*)tmp, HMI_UART);
}
void rqtSave(char channel)
{
	WriteByteFromEEprom(LANGUAGE,language);
	WriteIntFromEEprom(PRE_CUR,readDAC(D_ChCur));
	WriteIntFromEEprom(PRE_TEM,thermoPara_LD.T_Target);
	if(channel != HMI_UART){
		sprintf(temp, "\nsave ok");
		SendStr((unsigned char*)temp, COMM_UART);
	}
	else if (channel == HMI_UART){
		sprintf(temp, "save.status.txt=\"save ok\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
	}
}

void SwitchLanguage(void)
{
	if(!language)	//switch chanise
	{
		sprintf(temp, "language.val=1");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lan.txt=\"简体中文\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lpower.txt=\"功率\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lcurrent.txt=\"电流\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "ltemperatre.txt=\"温度\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "save.txt=\"保存\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "load.txt=\"加载\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "autorun.txt=\"自加载\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "setp.txt=\"设置\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "setc.txt=\"设置\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "sett.txt=\"设置\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.back.txt=\"取消\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.sure.txt=\"确定\"");
		SendStr((unsigned char*)temp, HMI_UART); 
		hmi_end(HMI_UART);
		if(LDstatus())
		{
			sprintf(temp, "b_ldswitch.txt=\"LD 开\"");
			SendStr((unsigned char*)temp, HMI_UART);
			hmi_end(HMI_UART);			
		}else
		{
			sprintf(temp, "b_ldswitch.txt=\"LD 关\"");
			SendStr((unsigned char*)temp, HMI_UART);   
			hmi_end(HMI_UART);
		}		
	}
	else
	{
		sprintf(temp, "language.val=0");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lan.txt=\"English\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lpower.txt=\"Power\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lcurrent.txt=\"Current\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "ltemperatre.txt=\"Temperatre\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "save.txt=\"Save\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "load.txt=\"Load\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "autorun.txt=\"Auto\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "setp.txt=\"set\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "setc.txt=\"set\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "sett.txt=\"set\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.back.txt=\"cancel\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.sure.txt=\"ok\"");
		SendStr((unsigned char*)temp, HMI_UART); 
		hmi_end(HMI_UART);
		if(LDstatus())
		{
			sprintf(temp, "b_ldswitch.txt=\"LD ON\"");
			SendStr((unsigned char*)temp, HMI_UART);   
			hmi_end(HMI_UART);
		}else
		{
			sprintf(temp, "b_ldswitch.txt=\"LD OFF\"");
			SendStr((unsigned char*)temp, HMI_UART);   
			hmi_end(HMI_UART);
		}
	}
}
void HmiLoad(char channel)
{
	float tartemp = 0;
	float buff = 0;
	float tarcurr = 0;
	InitTarCurr = ReadIntFromEEprom(PRE_CUR);
	InitTarTemp = ReadIntFromEEprom(PRE_TEM);
	writeDAC(D_ChCur,InitTarCurr);
	thermoPara_LD.T_Target = InitTarTemp;
	language = readLanguagefromFlash();
	tarcurr = GetAimCurrWithInitCurr(InitTarCurr);
	rqtSLDI(COMM_UART,f);
	tartemp = GetAimTempWithInitTemp(InitTarTemp);
	sprintf(temp,"tLdTarcurrent.txt=\"%.2f\"",tarcurr);
	SendStr((unsigned char*)temp,channel);
	hmi_end(channel);
	buff = GetPowerWithCurrent(tarcurr);
	sprintf(temp,"tLdTargetpower.txt=\"%.2f\"",buff);
	SendStr((unsigned char*)temp,channel);
	hmi_end(channel);
	sprintf(temp,"tLdTartemp.txt=\"%.2f\"",tartemp);
	SendStr((unsigned char*)temp,channel);
	hmi_end(channel);
	sprintf(temp,"c0.val=%d",Autorun);
	SendStr((unsigned char*)temp,channel);
	hmi_end(channel);
	if(!language)
	{
		sprintf(temp, "language.val=1");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lan.txt=\"简体中文\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lpower.txt=\"功率\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lcurrent.txt=\"电流\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "ltemperatre.txt=\"温度\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "save.txt=\"保存\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "load.txt=\"加载\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "autorun.txt=\"自加载\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "setc.txt=\"设置\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "sett.txt=\"设置\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.back.txt=\"取消");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.sure.txt=\"确定\"");
		SendStr((unsigned char*)temp, HMI_UART); 
		hmi_end(HMI_UART);
		if(LDstatus())
		{
			sprintf(temp, "b_ldswitch.txt=\"LD 开\"");
			SendStr((unsigned char*)temp, HMI_UART);
			hmi_end(HMI_UART);			
		}else
		{
			sprintf(temp, "b_ldswitch.txt=\"LD 关\"");
			SendStr((unsigned char*)temp, HMI_UART);   
			hmi_end(HMI_UART);
		}		
	}
	else
	{
		sprintf(temp, "language.val=0");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lan.txt=\"English\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lpower.txt=\"Power\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "lcurrent.txt=\"Current\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "ltemperatre.txt=\"Temperatre\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "save.txt=\"Save\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "load.txt=\"Load\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "autorun.txt=\"Auto\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "setc.txt=\"set\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "sett.txt=\"set\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.back.txt=\"cancel\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.sure.txt=\"ok\"");
		SendStr((unsigned char*)temp, HMI_UART); 
		hmi_end(HMI_UART);
		if(LDstatus())
		{
			sprintf(temp, "b_ldswitch.txt=\"LD ON\"");
			SendStr((unsigned char*)temp, HMI_UART);   
			hmi_end(HMI_UART);
		}else
		{
			sprintf(temp, "b_ldswitch.txt=\"LD OFF\"");
			SendStr((unsigned char*)temp, HMI_UART);   
			hmi_end(HMI_UART);
		}
	}
	
}

void rqtSW(char channel,float para)
{
	retRqtHead(channel);
	sprintf(tmp,"Set MaxPower:%.3f",para);
	SendStr((unsigned char*)tmp,channel);	
	WriteFloatFromEEprom(MaxP,para);
}
void rqtQW(char channel)
{
	float power = ReadFloatFromEEprom(MaxP);
	retRqtHead(channel);
	sprintf(tmp," MaxPower:%.3f",power);
	SendStr((unsigned char*)tmp,channel);
}
float GetPowerWithCurrent(float current)
{
	return ((ArgA*current+ArgB)<0?0:(ArgA*current+ArgB));
}
float GetCurrentWidthPower(float Power)
{
	return ((Power-ArgB)/ArgA);
}
void rqtSPower(char channel, float para)
{
	unsigned char flag = 0;
	float current = 0;
	if(para > MaxPower){
		para = MaxPower;
		flag = 1;
	}
	if(para < 0){
		para = 0;
		flag = 1;
	}
	sprintf(tmp, "GS_P%.3f", para);
	if((CommandCRC != 0)&&(CommandCRC != CalCRC(tmp,strlen(tmp)))){
		retRqtHead(channel);
		sprintf(tmp, "CRCError");
		SendStr((unsigned char*)tmp, channel);	
		return;
	}
	if(channel != HMI_UART){
		retRqtHead(channel);
		sprintf(temp, "Power sets to %.2f", para);
		SendStr((unsigned char*)temp,channel);
		sprintf(temp, "tLdTargetpower.txt=\"%.2f\"", para);
		SendStr((unsigned char*)temp,HMI_UART);	
		hmi_end(HMI_UART);
	}
	if(channel == HMI_UART){
		if(flag){
			sprintf(temp, "page error");
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);
			return ;
		}
		else{
			sprintf(temp, "page main");
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);
			sprintf(temp, "tLdTargetpower.txt=\"%.2f\"", para);
			SendStr((unsigned char*)temp,HMI_UART);	
			hmi_end(HMI_UART);
		}
	}
	//power to current
	current = GetCurrentWidthPower(para);
	if(CommandCRC != 0){
		sprintf(temp, "GS_I%.3f", current);
		CommandCRC = CalCRC(temp,strlen(temp));
	}
	rqtSLDI(channel,current);
	
}

void rqtQPower(char channel)
{
	float Ireal = 0;
	float power = 0;
	//current to power
	Ireal = GetCurrent();
	power = GetPowerWithCurrent(Ireal);
	if(DEBUG_UART == channel){
		retRqtHead(channel);
		sprintf(tmp," Power:%.2f",power);
		SendStr((unsigned char*)tmp,channel);	
	}
	else if(COMM_UART == channel){
		retRqtHead(channel);
		sprintf(tmp," Power:%.2f",power);
		SendStr((unsigned char*)tmp,channel);	
	}
	else{
		sprintf(tmp, "tLdpower.txt=\"%.3f\"",power);
		SendStr((unsigned char*)tmp,channel);	
		hmi_end(channel);
		switch(objectS)
		{
			case 1:		sprintf(tmp, "show.s1.txt=\"%.2fW\"",power);
								SendStr((unsigned char*)tmp,channel);	
								hmi_end(channel);
								break;
			case 2:		sprintf(tmp, "show.s2.txt=\"%.2fW\"",power);
								SendStr((unsigned char*)tmp,channel);	
								hmi_end(channel);
								break;
			case 3:		sprintf(tmp, "show.s3.txt=\"%.2fW\"",power);
								SendStr((unsigned char*)tmp,channel);	
								hmi_end(channel);
								break;
			default:break;
		}
	}
}

void rqtSL(char channel,float para)
{
	if(para<1){
		language = 0;				//switch chanise
		SwitchLanguage();
	}
	else{
		language = 1;				//switch english
		SwitchLanguage();
	}
	
}
void rqtQL(char channel)
{
	SwitchLanguage();
//	if(channel != HMI_UART){
//		retRqtHead(channel);
//		if(language)
//			sprintf(tmp, "lan=English");
//		else
//			sprintf(tmp, "lan=简体中文");
//		SendStr((unsigned char*)tmp, channel);
//	}
//	if(language)
//			sprintf(tmp, "lan.txt=\"English\"");
//	else
//			sprintf(tmp, "lan.txt=\"简体中文\"");
//  SendStr((unsigned char*)tmp, HMI_UART);	
}
void initRK(char channel,char* strpara)
{
		int j;
	char t[20];
	memset(tmp, 0x0, 24);
	j = atoi(strpara);
	decstr2ch(strpara, tmp);
	if(j == 0){
		sprintf(temp, "SetUI.object.txt=\"current\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.rank.txt=\"range:0~%.1fA\"",MaxCurrent);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
	}
	else if(j == 1){
		sprintf(temp, "SetUI.object.txt=\"temperature\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.rank.txt=\"range:%.1f~%.1f`C\"",LOWTEMLD,HIGHTEMLD);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
	}
	else if(j == 2){
		ReadStrFromEEprom(PM,t,20);
		sprintf(temp, "mode.txt=\"%s\"",t);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp,"c0.val=%d",Autorun);
		SendStr((unsigned char*)temp,channel);
		hmi_end(channel);
	}
	else if(j == 3){
		sprintf(temp, "SetUI.object.txt=\"power\"");
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "SetUI.rank.txt=\"range:0~%.1fW\"",MaxPower);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
	}
	else if(j == 5){
		objectS = 1;
	}
	else if(j == 6){
		objectS = 2;
	}
	else if(j == 7){
		objectS = 3;
	}
	else if(j == 8){
		
		ReadStrFromEEprom(PM,t,20);
		sprintf(temp, "aboutc.PartType.txt=\"%s\"",t);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "aboute.PartType.txt=\"%s\"",t);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		ReadStrFromEEprom(SN,t,20);
		sprintf(temp, "aboutc.SerialNumber.txt=\"%s\"",t);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "aboute.SerialNumber.txt=\"%s\"",t);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		ReadStrFromEEprom(TM,t,20);
		sprintf(temp, "aboutc.Calibration.txt=\"%s\"",t);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
		sprintf(temp, "aboute.Calibration.txt=\"%s\"",t);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
//		sprintf(temp, "aboutc.Version.txt=\"%0x\"",Version);
//		SendStr((unsigned char*)temp, HMI_UART);
//		hmi_end(HMI_UART);
		sprintf(temp, "aboutc.Revision.txt=\"%0x\"",Reversion);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
//		sprintf(temp, "aboute.Version.txt=\"%0x\"",Version);
//		SendStr((unsigned char*)temp, HMI_UART);
//		hmi_end(HMI_UART);
		sprintf(temp, "aboute.Revision.txt=\"%0x\"",Reversion);
		SendStr((unsigned char*)temp, HMI_UART);
		hmi_end(HMI_UART);
	}
}
void APSyncLD(char lwitch)
{

}
void rqtTargetTemp()
{
	retRqtHead(COMM_UART);
	f = 15.906* thermoPara_LD.T_Target/32767.0 *3 + 12.633;
	sprintf(temp, "TargetTemp:%.2f",f);
	SendStr((unsigned char*)temp, COMM_UART);
}

void HmiCancel(char channel)
{
	writePreCurtoFlash(InitTarCurr);
	writePreTemtoFlash(InitTarTemp);
	
}
void rqtQID()
{
	unsigned char ch;
	memset(tmp, 0x0, 64);
	retRqtHead(COMM_UART);
	readDevCanID((char*)&ch);
	hex2str((char *)&ch, tmp, 1);
	strcpy((char*)temp, "ID:");
	strcat(temp, tmp);
	SendStr((unsigned char*)temp, COMM_UART);
}
void rqtQHardID()
{
	unsigned char ch = 0;
	memset(tmp, 0x0, 64);
	retRqtHead(COMM_UART);
	hex2str((char*)&ch, tmp, 0x01);
	strcpy(temp,"hardID:");
	strcat(temp, tmp);
	SendStr((unsigned char*)temp, COMM_UART);
}
void rqtQPDpwr()
{
	retRqtHead(COMM_UART);
	i = ConReadADC(A_RealPDPwr);
	//i &= 0x0fff;
	f = i/32767.0 ;  //need more calculation
	sprintf(tmp,"PD%.2f",f);
	SendStr((unsigned char*)tmp, COMM_UART);
}
void rqtQMAC()
{
	char ch[4];
	memset(ch, 0x0, 4);
	retRqtHead(COMM_UART);
	readMAC(ch);
	hex_reverse(ch,4);
	hex2str(ch, tmp, 4);
	reverse(tmp);
	sprintf(temp, "MAC:%s",tmp);
	SendStr((unsigned char*)temp, COMM_UART);
}
void rqtQPreTem()
{
	retRqtHead(COMM_UART);
	i = ReadIntFromEEprom(PRE_TEM);
	f = 15.906* i/32767.0 *3 + 12.633;//(2.612 - thermoPara_LD.T_Target/32767.0 *3)/0.0258 + LOWTEMLD;	// by wt
	sprintf(temp, "Temp preseted is:%.2f",f);
	SendStr((unsigned char*)temp, COMM_UART);
}
void rqtTECswitch(char channel,char c)
{
	TECswitch(channel,c);
	retRqtHead(channel);
	if(c == 1)
	{
		sprintf(temp, "TEC switched on");
	}
	else
		sprintf(temp, "TEC switched off");
	SendStr((unsigned char*)temp,channel);
}
void rqtTMSswitch(char c)
{
	THOswitch(c);
	retRqtHead(COMM_UART);
	if(c == 1)
	{
		sprintf(temp, "Thermostat switched on");
	}
	else
		sprintf(temp, "Thermostat switched off");
	SendStr((unsigned char*)temp,COMM_UART);
}
void rqtQparam(char channel)
{
	memset(tmp,0x0,16);

	retRqtHead(channel);
	readTemAdjA(tmp);
	i = ch2dec(tmp);
	f = i/1000.0;
	sprintf(temp, "  T_PARAM_A:%.2f",f);
	SendStr((unsigned char*)temp,channel);
	
	readTemAdjB(tmp);
	i = ch2dec(tmp);
	f = i/1000.0;
	sprintf(temp, "  T_PARAM_B:%.2f",f);
	SendStr((unsigned char*)temp,channel);
}

void rqtQLDV(char channel)
{
	retRqtHead(channel);
	i = ConReadADC(A_RealLDVol);
	if(i > 32767)
		i= 0;
	f = i/32767.0 * 3 *23 / 3.0;  //need more calculation
	if(DEBUG_UART == channel)
		sprintf(tmp,"LD vol is:%.2f     %.2fV",f,i/32767.0*3);
	else
		sprintf(tmp,"LD vol is:%.2f ",f);
	SendStr((unsigned char*)tmp,channel);
}
void rqtSBWThermo(char channel)
{
	retRqtHead(channel);
	ifBWThermo = ((ifBWThermo +1)&0x01);
	if(1 == ifBWThermo)
	{
		sprintf(tmp, " work in both way thermostat mode");
	}
	else
	{
		sprintf(tmp, " work in only cooling thermostat mode");
	}
	writeBW_Thermo(&ifBWThermo);
	SendStr((unsigned char*)tmp,channel);
}
void rqtQMonitorTem(char channel)
{
	int i_hb = 0;
	int i_dc = 0;
	float f_tmp1 = 0;
	float f_tmp2 = 0;
	float f_hb = 0;
	float f_dc = 0;
	retRqtHead(channel);
	i_hb = ConReadADC(A_RealT_HBridge);
	i_dc = ConReadADC(A_RealT_DCDC);
	/*
	f_tmp1 = i_hb/65535.0*3;
	f_hb = f_tmp1*f_tmp1*f_tmp1*3.8092;
	f_hb = f_hb - f_tmp1*f_tmp1*16.353;
	f_hb = f_hb + 52.625*f_tmp1 - 4.6161;
	f_tmp2 = i_dc/65535.0*3;
	f_dc = f_tmp2*f_tmp2*f_tmp2*3.8092;
	f_dc = f_dc- f_tmp2*f_tmp2*16.353;
  f_dc = f_dc	+ 52.625*f_tmp2 - 4.6161;
	*/
	f_tmp1 = i_hb/65535.0f*3;
	f_hb = f_tmp1*f_tmp1*f_tmp1*4.0049f;
	f_hb = f_hb - f_tmp1*f_tmp1*17.121f;
	f_hb = f_hb + 53.407f*f_tmp1 - 4.7797f;
	f_tmp2 = i_dc/65535.0f*3;
	f_dc = f_tmp2*f_tmp2*f_tmp2*4.0049f;
	f_dc = f_dc- f_tmp2*f_tmp2*17.121f;
  f_dc = f_dc	+ 53.407f*f_tmp2 - 4.7797f;
	
	if(f_hb<0)f_hb =0;
	if(f_dc<0)f_dc =0;
	
	if(DEBUG_UART == channel)
	{
		sprintf(tmp,"HBrifge TEMP:%.2f %.2fV  adc_value:%x",f_hb, f_tmp1, i_hb);
		sprintf(temp," DCDC TEMP:%.2f %.2fV  adc_value:%x",f_dc, f_tmp2, i_dc);
	}
	else
	{
		sprintf(tmp,"HBrifge TEMP:%.2f ",f_hb);
		sprintf(temp," DCDC TEMP:%.2f ",f_dc);
	}
	retRqtHead(channel);
	SendStr((unsigned char*)tmp,channel);
	SendStr((unsigned char*)temp,channel);
	
}
void rqtQFrq(char channel)
{
	retRqtHead(channel);
	sprintf(tmp, "input frequency:%dHZ", freq);
	SendStr((unsigned char*)tmp,channel);
}
void debug_info_task(uint32_t dummy)
{
	rqtQLDI(DEBUG_UART);
	rqtQTem(DEBUG_UART);
	rqtQTec(DEBUG_UART);
	rqtQLDV(DEBUG_UART);
	rqtQMonitorTem(DEBUG_UART);
//	rqtQFrq(DEBUG_UART);
}

float GetTecVol(void)
{
		return ABS((ConReadADC(A_RealTecVol)/65535.0*3)*10 );
}
