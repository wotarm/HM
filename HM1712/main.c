/*HEADER**********************************************************************
*
* Copyright 2008 Freescale Semiconductor, Inc.
* Copyright 1989-2008 ARC International
*
* This software is owned or controlled by Freescale Semiconductor.
* Use of this software is governed by the Freescale MQX RTOS License
* distributed with this Material.
* See the MQX_RTOS_LICENSE file distributed for more details.
*
* Brief License Summary:
* This software is provided in source form for you to use free of charge,
* but it is not open source software. You are allowed to use this software
* but you cannot redistribute it or derivative works of it in source form.
* The software may be used only in connection with a product containing
* a Freescale microprocessor, microcontroller, or digital signal processor.
* See license agreement file for full license terms including other
* restrictions.
*****************************************************************************
*
* Comments:
*
*   This file contains the source for the hello example program.
*
*
*END************************************************************************/

#include <mqx.h>
#include <bsp.h> 
#include <fio.h>
#include <stdlib.h>

#include <bsp.h>
#include <lwevent.h>
#include "EEPROM.h"
#include "api.h"
#include "motor.h"
#include "ftm.h"


#include "can_ctrl.h"
#include "spi_ctrl.h"
#include "adc_ctrl.h"
#include "uart_ctrl.h"
#include "hw_ptr.h"
#include "common_inc.h"
#include "ftm.h"
#include "gpio.h"
#include "dac_ctrl.h"
#include "eeprom.h"
#include "parameter.h"
#include "thermostat.h"
#include "api.h"
#include "ext_int.h"
#include "capture.h"
#include "fpga_int.h"
#include "time.h"
#include "PublicHead.h"
#include "i2c_ctrl.h"

s_HW_ptr HW_ptr;
extern s_LD_para LD_para;
#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


/* Task IDs */
#define MAIN_TASK 						5
#define UART_TASK  						6
#define TX_TASK 							7
#define RX_STATE_TASK 				8
#define RX_REG_TASK 					9
#define THERMOSTAT_TASK 			10
#define HANDLE_EXTINT_TASK 		11
#define UARTPROCESS_TASK 			12
#define CTRL_TASK 						13
#define CURCTRL_TASK 					14
#define UART_COMM_REC_TASK 		15
#define DEBUG_INFO_TASK 			16
#define PROCESS_EXTINT_TASK 	17
#define UART_HMI_REC_TASK			18
#define CANDATAHANDLE_TASK 		19
#define PROCESS_FPGAINT_TASK 	20
#define PROCESS_LDV_TASK			21

extern void main_task(uint32_t);
unsigned int base_LDI_ADC;
unsigned int base_LDI_DAC;
extern char EN_Logic;
extern char work_mode;
extern char ifBWThermo;
extern unsigned int pulse_count;
extern int InitTarCurr ;
extern int InitTarTemp ;
extern char language;
extern char Autorun;
extern float MaxPower;
extern float ArgA;
extern float ArgB;
extern float MaxCurrent;	//o.1A
int CalCurrBase = 0;
float BaseCurrent = 0;
char MOENLD = 0;
float MaxLDV  = 20;
float CurrentStep = 0;
unsigned int CurrentStepDAC = 0;
unsigned int BaseStepDAC = 0;
float DACREF = 1.2;
extern sThermoPara thermoPara_LD;

const TASK_TEMPLATE_STRUCT  MQX_template_list[] = 
{ 
    /* Task Index,   Function,   Stack,  Priority, Name,     Attributes,          Param, Time Slice */
    {MAIN_TASK,   main_task, 1500,   7,        "main",  MQX_AUTO_START_TASK, 0, 0},
		{UART_TASK, uart_task, 500, 				15,				"uart_task",	0, 0,		0 },
		{RX_STATE_TASK, Rx_state_Task, 1500, 8, "Rx_state_Task", 0, 0, 0},
		{RX_REG_TASK, Rx_reg_Task, 1500, 10, "Rx_reg_Task", 0, 0, 0},
		{TX_TASK, Tx_Task, 1500, 11, "Tx_Task", 0, 0, 0},
		{UARTPROCESS_TASK, ProcessUARTMsg, 1500, 3,		"ProcessUARTMsg", 0, 0, 0},
		{THERMOSTAT_TASK, thermostat_task, 500, 6,		"thermostat_task", 0, 0, 0},
		{CURCTRL_TASK, CurCtrl_Task, 1500, 12,		"CurCtrl_Task", 0, 0, 0},
		{UART_COMM_REC_TASK, uart_comm_rec_task, 500, 14, "comm_uart_rec_task",0,0,0},
		{UART_HMI_REC_TASK, uart_hmi_rec_task, 500, 		15,				"hmi_uart_rec_task",	0, 0,		0 },
		{CANDATAHANDLE_TASK, CanHandle_Task, 1500, 4,		"CanHandle_Task", 0, 0, 0},
		{PROCESS_LDV_TASK, CheckLDV_Task, 1500, 16,		"PROCESS_LDV_Task", 0, 0, 0},
    { 0 }
};

#if	VersionHPower
void gpio_init()
{	
  GPIO_QuickInit(HW_GPIOA, 26, kGPIO_Mode_OPP);   //STATUS N 
  GPIO_QuickInit(HW_GPIOA, 27, kGPIO_Mode_IFT);   //EN ARM IN 
  GPIO_QuickInit(HW_GPIOB, 9, kGPIO_Mode_OPP);   //EN ARM OUT 
  GPIO_QuickInit(HW_GPIOB, 8, kGPIO_Mode_IFT);   //STATUS NP1 
  GPIO_QuickInit(HW_GPIOE, 10, kGPIO_Mode_OPP);  //LD EN
  GPIO_QuickInit(HW_GPIOA, 10, kGPIO_Mode_OPP);  //TEC EN
  GPIO_QuickInit(HW_GPIOE, 8, kGPIO_Mode_OPP);   //TECDIR
	GPIO_WriteBit(HW_GPIOE, 10, 0);								 //set en_ld to 0
  GPIO_QuickInit(HW_GPIOA, 25, kGPIO_Mode_OPP);  //LED0
  GPIO_QuickInit(HW_GPIOA, 24, kGPIO_Mode_OPP);  //LED1
  GPIO_QuickInit(HW_GPIOB, 1, kGPIO_Mode_OPP);  //LED_0 run
  GPIO_QuickInit(HW_GPIOB, 2, kGPIO_Mode_OPP);  //LED_1	las
	PORT_PinMuxConfig(HW_GPIOB, 20, kPinAlt6);
	PORT_PinMuxConfig(HW_GPIOC, 5, kPinAlt6);
	FTM_PWM_QuickInit(FTM1_CH1_PA09, kPWM_EdgeAligned, 50000);	//T1PWM
  FTM_PWM_ChangeDuty(HW_FTM1, HW_FTM_CH1, 0);	
}
#else
void gpio_init()
{	
  GPIO_QuickInit(HW_GPIOC, 4, kGPIO_Mode_OPP);   //SPI CS 
  GPIO_QuickInit(HW_GPIOB, 20, kGPIO_Mode_OPP);   //LCD SPI CS 
  GPIO_QuickInit(HW_GPIOD, 9, kGPIO_Mode_OPP);  //GPIO0
	GPIO_WriteBit(HW_GPIOD, 9, 0);
  GPIO_QuickInit(HW_GPIOD, 8, kGPIO_Mode_OPP);  //GPIO1
	GPIO_WriteBit(HW_GPIOD, 8, 0);
  GPIO_QuickInit(HW_GPIOE, 9, kGPIO_Mode_OPP);   //LD1EN
	GPIO_WriteBit(HW_GPIOE, 9, 0);
  GPIO_QuickInit(HW_GPIOE, 7, kGPIO_Mode_OPP);   //TEC0DIR
	GPIO_WriteBit(HW_GPIOE, 7, 0);
  GPIO_QuickInit(HW_GPIOE, 8, kGPIO_Mode_OPP);   //TEC1DIR
	GPIO_WriteBit(HW_GPIOE, 8, 0);
  GPIO_QuickInit(TEC_DIR, kGPIO_Mode_OPP);  //TEC_DIR
  GPIO_QuickInit(LED0, kGPIO_Mode_OPP);  //LED0
  GPIO_QuickInit(LED1, kGPIO_Mode_OPP);  //LED1
  GPIO_QuickInit(HW_GPIOD, 15, kGPIO_Mode_OPP);  //LASER_ON_LED
	GPIO_WriteBit(HW_GPIOA, 24, 0);
	GPIO_WriteBit(HW_GPIOA, 25, 1);
	GPIO_WriteBit(HW_GPIOD, 15, 1);
  GPIO_QuickInit(HW_GPIOD, 2, kGPIO_Mode_OPP);  //I2C_SCL
	GPIO_WriteBit(HW_GPIOD, 2, 0);
  GPIO_QuickInit(HW_GPIOD, 3, kGPIO_Mode_OOD);  //I2C_SDA
	GPIO_WriteBit(HW_GPIOD, 3, 1);
}
#endif
void functional_init()
{ 
	VREF_BASE_PTR->SC = 0x81;
	VREF_BASE_PTR->TRM |= 0x40;
	uart_init();
	adc_init();
	gpio_init();
	dac_init();
	#if VersionHPower
	hscmp_init();
	#else
	i2c_init();
	#endif
	#if FalshEEprom
	FLASH_Init();
	FlexMemInit();
	EEPROM_init();
	#else
	FlashEEprom_Init();
	#endif
	canInit_Task(0);
	Rx_Task(0);
	InitTherm();
	//hscmp_init();
	init_LD_para();
	#if VersionHPower
	base_LDI_ADC  = Getbase_LDI_ADC();	
	base_LDI_DAC	= Getbase_LDI_DAC();
	#endif
	//Max Power Limit
	T_EMAX = ReadByteFromEEprom(TECStrength)*100;
	if(T_EMAX>SystemTEClimit)
		T_EMAX = SystemTEClimit;
	if(T_EMAX<2000)
		T_EMAX = 2000;
	LD_para.I_Target = 0;
	LD_para.BaisI = 0;
	InitTarCurr = ReadIntFromEEprom(PRE_CUR);
	InitTarTemp = ReadIntFromEEprom(PRE_TEM);
	language = ReadByteFromEEprom(LANGUAGE);
	ArgA = ReadFloatFromEEprom(PRE_ArgA);
	ArgB = ReadFloatFromEEprom(PRE_ArgB);
	MaxPower = ReadFloatFromEEprom(MaxP);
	if(MaxPower>SystemMaxP)
		MaxPower=SystemMaxP;
	MaxCurrent = GetCurrentWidthPower(MaxPower);
	if(MaxCurrent>SystemMaxCurr)
		MaxCurrent=SystemMaxCurr;
	if(MaxCurrent>ReadFloatFromEEprom(E_MI)){
		MaxCurrent = ReadFloatFromEEprom(E_MI);
	}
	if(MaxCurrent<=0)
		MaxCurrent = 0;
	SwitchLanguage();	
	#if VersionHPower
	DACREF = GetDACRef();
	CurrCalibration();
	#endif
	Autorun = ReadByteFromEEprom(AUTORUN);
	StartLCD();
	rqtTarget(HMI_UART);
	MOENLD = ReadByteFromEEprom(MOSynLD);
	MaxLDV = ReadFloatFromEEprom(MaxLDVLimit);
	CurrentStep = ReadFloatFromEEprom(E_LI);
	CurrentStepDAC = GetDACValueWithCurrent(CurrentStep);
	CurrentStepDAC -= LD_para.BaisI;
//	char temp[20];	
//	retRqtHead(COMM_UART);
//	sprintf(temp, "curr step:%d-%d-%f-%d-%d-",CurrentStepDAC,LD_para.BaisI,DACREF,base_LDI_DAC,base_LDI_ADC);
//	SendStr((unsigned char*)temp,COMM_UART);
	if(Autorun){
			HmiLoad(HMI_UART);
	}
	InitTime();
	
}


/*TASK*-----------------------------------------------------
* 
* Task Name    : main_task
* Comments     :
*    This task prints " Hello World "
*
*END*-----------------------------------------------------*/
void main_task(uint32_t initial_data)
{
  (void)initial_data; /* disable 'unused variable' warning */
  _task_id     created_task;	 
	functional_init();
	 _lwsem_create(&HW_ptr.uart_sem, 0);
	 //_lwsem_create(&HW_ptr.can_sem, 0);
	
	 created_task = _task_create(0, THERMOSTAT_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nthermo task: task creation failed.");
   }
	 
	 created_task = _task_create(0, UART_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nthermo task: task creation failed.");
   }
	 created_task = _task_create(0, UARTPROCESS_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nuart process task: task creation failed.");
   }
	 created_task = _task_create(0, TX_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\ncan tx task: task creation failed.");
   }
	 created_task = _task_create(0, RX_STATE_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\ncan state rx task: task creation failed.");
   }
	 created_task = _task_create(0, RX_REG_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\ncan reg rx task: task creation failed.");
   }
	 created_task = _task_create(0, CURCTRL_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nLD cur ctrl task: task creation failed.");
   }
	 created_task = _task_create(0, UART_COMM_REC_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nuart comm_rec task: task creation failed.");
   }
	 created_task = _task_create(0, UART_HMI_REC_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nuart hmi_rec task: task creation failed.");
   }
	  created_task = _task_create(0, CANDATAHANDLE_TASK, 0);
   if (created_task == MQX_NULL_TASK_ID) 
   {
      printf("\nCANDATAHANDLE_TASK: task creation failed.");
   } 
	 created_task = _task_create(0, PROCESS_LDV_TASK, 0);
//   if (created_task == MQX_NULL_TASK_ID) 
//   {
//      printf("\nPROCESS_LDV_TASK: task creation failed.");
//   }
	int_task(0);// add by wt 
	while(1)
	{
		#if VersionHPower
		GPIO_ToggleBit(HW_GPIOA, 25);      //turn the LED pin
		GPIO_ToggleBit(HW_GPIOB, 1);      //turn the LED pin
		#else
		GPIO_ToggleBit(HW_GPIOA, 24);      //turn the LED pin
		#endif
    _time_delay(1000);
	}
   //_task_block();
}

