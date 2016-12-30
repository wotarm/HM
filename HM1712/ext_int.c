
#include "ext_int.h"
#include "spi_ctrl.h"
#include "dac_ctrl.h"
#include "adc_ctrl.h"
#include "api.h"
#include "motor.h"
#include "can_ctrl.h"
#include "common_fun.h"
#include "gpio.h"
LWSEM_STRUCT lwsem;

LWGPIO_STRUCT extint1;
extern unsigned int base_LDI_ADC;
extern s_LD_para LD_para;
extern unsigned int curLDI;
#define PROCESS_EXTINT_TASK 17
/******************************************************************************
*
* Functio Name      : int_service_routine
* Comments          : The interrupt service routine triggered by gpio
*
******************************************************************************/
void int_service_routine(void *pin)
{
  lwgpio_int_clear_flag((LWGPIO_STRUCT_PTR) pin);
    //_lwsem_post(&lwsem);
	 _task_id     created_task;
	 created_task =  _task_create(0, PROCESS_EXTINT_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nthermo task: task creation failed.");
   }

    _lwsem_post(&lwsem);
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
    _mqx_uint result;
		
    /* Create the lightweight semaphore */
    result = _lwsem_create(&lwsem, 0);
    if (result != MQX_OK) {
        printf("\nCreating sem failed: 0x%X", result);
        _task_block();
    }


#if defined BSP_EXTINT
/******************************************************************************
    Open the pin (BSP_BTN1) for input, initialize interrupt
    and set interrupt handler.
******************************************************************************/
    /* opening pins for input */
    if (!lwgpio_init(&extint1, BSP_EXTINT1, LWGPIO_DIR_INPUT, LWGPIO_VALUE_NOCHANGE))
    {
        printf("Initializing button GPIO as input failed.\n");
        _task_block();
    }

#ifdef BSP_EXTINT_MUX_IRQ
    /* Some platforms require to setup MUX to IRQ functionality,
    for the rest just set MUXto GPIO functionality */
    lwgpio_set_functionality(&extint1, BSP_EXTINT_MUX_IRQ);
#if defined(BSP_BUTTONS_ACTIVE_HIGH)
    lwgpio_set_attribute(&extint1, LWGPIO_ATTR_PULL_DOWN, LWGPIO_AVAL_ENABLE);
#else
    lwgpio_set_attribute(&extint1, LWGPIO_ATTR_PULL_UP, LWGPIO_AVAL_ENABLE);
#endif

    /* enable gpio functionality for given pin, react on falling edge */
    if (!lwgpio_int_init(&extint1, LWGPIO_INT_MODE_RISING))// LWGPIO_INT_MODE_FALLING
    {
        printf("Initializing button GPIO for interrupt failed.\n");
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

char preStatus = 1;
void handle_Extint_task(uint32_t dummy)
{
	GPIO_WriteBit(EN_LD, 0);
	LD_para.enLD = 0;
	Delay(10);
	
	if( (( (curLDI - base_LDI_ADC)/32767.0*3)*10) <0.1)
	{
		printf("over voltage; no load");
	}
	else
	{
		printf("over voltage; current out of range");
	}
	
	LD_para.I_Target = 0;
	writeDAC(D_ChCur,0);
	//GPIO_WriteBit(EN_LD, 1);
	
}
/* EOF */
