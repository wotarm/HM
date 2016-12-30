#include "time.h"
#include <mqx.h>
#include <bsp.h>
#include <lwsem.h>
#include "gpio.h"
#include "parameter.h"
#include "ftm.h"
#include "gs_protocol.h"
#include "hw_ptr.h"
#include "dac_ctrl.h"

extern unsigned int CurrentStepDAC;
extern char CtrlcurrFLAG;
extern s_LD_para LD_para;
#define HWTIMER1_FREQUENCY  10          //frequency set to hwtimer 1
HWTIMER hwtimer1;  
static void hwtimer1_callback(void *p);
void InitTime(void )
{
	 if (MQX_OK != hwtimer_init(&hwtimer1, &BSP_HWTIMER1_DEV, BSP_HWTIMER1_ID, (BSP_DEFAULT_MQX_HARDWARE_INTERRUPT_LEVEL_MAX + 1)))
    {
        printf(" FAILED!\n");
    }
    else
    {
        printf(" OK\n");
    }
//    printf("Try to set frequency %d Hz to hwtimer1\n",HWTIMER1_FREQUENCY);
//    hwtimer_set_freq(&hwtimer1, BSP_HWTIMER1_SOURCE_CLK, HWTIMER1_FREQUENCY);
//    printf("Read frequency from hwtimer1 : %d Hz\n", hwtimer_get_freq(&hwtimer1));
//    printf("Read period from hwtimer1    : %d us\n", hwtimer_get_period(&hwtimer1));
//    printf("Read modulo from hwtimer1    : %d\n", hwtimer_get_modulo(&hwtimer1));
//    /* Register isr for hwtimer1 */
//    printf("Register callback for hwtimer1\n");
    hwtimer_callback_reg(&hwtimer1,(HWTIMER_CALLBACK_FPTR)hwtimer1_callback, NULL);
//		 hwtimer_start(&hwtimer1);
		hwtimer_set_freq(&hwtimer1, BSP_HWTIMER1_SOURCE_CLK, 1000);	//1KHZ
		//hwtimer_start(&hwtimer1);	
}

static void hwtimer1_callback(void *p)
{
		
	if(CtrlcurrFLAG)
	{
		int cur = readDAC(D_ChCur);
		if(cur < LD_para.I_Target)
		{
			cur += CurrentStepDAC;
			if(cur >= LD_para.I_Target)
			{
				cur = LD_para.I_Target;
				CtrlcurrFLAG = 0;
				printf("\nAP Stop!");
				hwtimer_stop(&hwtimer1);
			}
			writeDAC(D_ChCur, cur);
		}
	}
	else{
		printf("\nAP Stop!");
		hwtimer_stop(&hwtimer1);
	}
}
