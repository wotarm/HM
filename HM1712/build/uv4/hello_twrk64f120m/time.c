#include "time.h"
#include <mqx.h>
#include <bsp.h>
#include <lwsem.h>
#include "gpio.h"
#include "parameter.h"
#include "ftm.h"

#define HWTIMER1_FREQUENCY  10          //frequency set to hwtimer 1
HWTIMER hwtimer1;  
static void hwtimer1_callback(void *p);
extern unsigned int APTime;					//60s
extern unsigned int APCount;
unsigned int DCount = 0;
unsigned int count = 0;
extern unsigned  int APDuty ;
extern unsigned int MoDelay;
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
}

static void hwtimer1_callback(void *p)
{
	count++;
	DCount++;
	if(DCount<=(APDuty/1000)){
		GPIO_WriteBit(DB25_PIN19_AP,1);
	}
	else{
		GPIO_WriteBit(DB25_PIN19_AP,0);
	}
	if(DCount>=10)
		DCount = 0;
	if(count >= APCount){
		GPIO_WriteBit(DB25_PIN19_AP,0);
		printf("\nAP Stop!");
		hwtimer_stop(&hwtimer1);
		DelayUs(MoDelay*1000);
		GPIO_WriteBit(DB25_PIN18_MO, 0);	//enable mo
		count = 0;		
		DCount = 0;
	}
}
