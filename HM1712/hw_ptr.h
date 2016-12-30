#ifndef _HW_PTR_
#define _HW_PTR_
#include <mqx.h>
#include <bsp.h> 

typedef struct s_HW_PTR
{
	MQX_FILE_PTR rs232_dev;
	MQX_FILE_PTR comm_dev;
	MQX_FILE_PTR hmi_dev;
	//LWADC_STRUCT adc0;
	//LWADC_STRUCT adc1;
	LWADC_STRUCT adcLDCur;
	LWADC_STRUCT adcLDTem;
	LWADC_STRUCT adcPDPwr;
	LWADC_STRUCT adcTecCur;
	LWADC_STRUCT adcTecVol;
	LWADC_STRUCT adcLDVol;
	LWADC_STRUCT dac_ref;
	LWADC_STRUCT ldi_ref;
	LWADC_STRUCT adcTem_HBridge;
	LWADC_STRUCT adcTem_DCDC;
	MQX_FILE_PTR spifp;
	MQX_FILE_PTR flex_file;
	LWSEM_STRUCT uart_sem;
	LWSEM_STRUCT can_sem;
	LWSEM_STRUCT ctrl_sem;
	LWADC_STRUCT adcTem2;
	LWADC_STRUCT adcP_Ref;
	LWADC_STRUCT adcMCU_Vref;
}s_HW_ptr;

extern s_HW_ptr HW_ptr;

typedef struct s_LD_ctrl
{
	char enLD;
	char curctrlflag;
	int I_Target;
	int BaisI;
}s_LD_para;
extern s_LD_para LD_ctrl;
#endif
