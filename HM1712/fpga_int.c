#include "fpga_int.h"
#include "spi_ctrl.h"
#include "dac_ctrl.h"
#include "api.h"
#include "motor.h"
#include "can_ctrl.h"
#include "gpio.h"
#include "hw_ptr.h"

#define BSP_EXT_AP_INT						(GPIO_PORT_C | GPIO_PIN1)
LWSEM_STRUCT lwsem;
extern char fpga_rw_flag;
#define PROCESS_FPGAINT_TASK 11
unsigned char FPGAInt=0;
LWGPIO_STRUCT extint1;
extern LWEVENT_STRUCT event;

extern s_LD_para LD_para;;
unsigned char LaserAP=0;
unsigned int  TargetCurr=0;
extern char MOENLD;
char CtrlcurrFLAG = 0;
extern HWTIMER hwtimer1;  
/******************************************************************************
*
* Functio Name      : int_service_routine
* Comments          : The interrupt service routine triggered by gpio
*
******************************************************************************/
void int_service_routine(void *pin)
{
//	GPIO_ToggleBit(HW_GPIOA, 25);      //turn the LED pin
	lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
	if(GPIO_ReadBit(HW_GPIOC,1)){	//AP ON
		LaserAP = 1;
		CtrlcurrFLAG = 1;
		if(MOENLD)
			hwtimer_start(&hwtimer1);	
		else
			writeDAC(D_ChCur,LD_para.I_Target);
	}
	else{	
		LaserAP = 0;			//AP OFF
		CtrlcurrFLAG = 0;
		writeDAC(D_ChCur,LD_para.BaisI);
	}
	if(MOENLD)
				LDswitch(0,LaserAP);
}

/******************************************************************************
*
* Task Name    : main_task
* Comments     : The main task executes 3 steps
*
*   1) Configures BSP_BUTTON1 to trigger interrupt on falling edge if supported
*      by selected platform.
*   2) Drives BSP_LED1 based on BSP_BUTTON1 state or
*      drives BSP_LED1 automatically if BSP_BUTTON1 is not available.
*   3) Togles BSP_LED1 if BSP_BUTTON1 is not available
*
******************************************************************************/
void int_task
    (
        uint32_t initial_data
    )
{
    /* Structures holding information about specific pin */
    //LWGPIO_STRUCT extint1;
    _mqx_uint result;
		
    /* Create the lightweight semaphore */
    result = _lwsem_create(&lwsem, 0);
    while (result != MQX_OK) {
        printf("\nCreating sem failed: 0x%X", result);
        _task_block();
    }


#if defined BSP_EXTINT
/******************************************************************************
    Open the pin (BSP_BTN1) for input, initialize interrupt
    and set interrupt handler.
******************************************************************************/
    /* opening pins for input */
    if (!lwgpio_init(&extint1, BSP_EXT_AP_INT/*BSP_EXTINT*/, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing button GPIO as input failed.\n");
        _task_block();
    }

#ifdef BSP_EXTINT_MUX_IRQ
    /* Some platforms require to setup MUX to IRQ functionality,
    for the rest just set MUXto GPIO functionality */
    lwgpio_set_functionality(&extint1, BSP_BUTTON1_MUX_IRQ);
#if defined(BSP_BUTTONS_ACTIVE_HIGH)
    lwgpio_set_attribute(&extint1, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
#else
    lwgpio_set_attribute(&extint1, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* enable gpio functionality for given pin, react on falling edge */
    if (!lwgpio_int_init(&extint1, LWGPIO_INT_MODE_FALLING|LWGPIO_INT_MODE_RISING))// LWGPIO_INT_MODE_FALLING
    {
        _task_block();
    }
    /* install gpio interrupt service routine */
    _int_install_isr(lwgpio_int_get_vector(&extint1), int_service_routine, (void *) &extint1);
    /* set the interrupt level, and unmask the interrupt in interrupt controller */
    _bsp_int_init(lwgpio_int_get_vector(&extint1), 7, 0, TRUE);
    /* enable interrupt on GPIO peripheral module */
    lwgpio_int_enable(&extint1, TRUE);
#endif /* BSP_BUTTON1_MUX_IRQ */
#endif
/******************************************************************************
    Read value from input pin (BSP_BTN1)  Note that in previous phase,
    the pin was configured as an interrupt and it have to be reconfigured
    to standard GPIO.
******************************************************************************/

}
/*
bit1: ESTOP(reserved)		10
bit2: PLL unlock				100
bit4: reserved					1	0000
bit5: reserved					10	0000
bit6: pulse_width_set		100	0000
bit7: db25_freqlmt			1000	0000
bit8: db25_dinlat				1	0000 0000
bit9: db25_mo						10	0000	0000
bit10: db25_ap					100	0000	0000
*/
#define Int_ESTOP 				0x02
#define Int_PLLunlock 		0x04
#define Int_pulse_width		0x40
#define Int_Freqlmt 			0x80
#define Int_Dinlat 				0x100
#define Int_MO    				0x200
#define Int_AP    				0x400
char preStatus = 1;
void handle_Fpgaint_task(uint32_t dummy)
{
	while(1)
	{
		if(FPGAInt){		
		}
		else{
		 if (_lwevent_wait_ticks(&event, 1 << 30, FALSE, 0) != MQX_OK) {
         printf("\nEvent Wait failed");
     }
		}
		FPGAInt=0;
		if(GPIO_ReadBit(HW_GPIOA,26)){	//AP ON
			FPGAInt=1;
		}
		else{														//AP OFF
			
		}
	}
}
/* EOF */
