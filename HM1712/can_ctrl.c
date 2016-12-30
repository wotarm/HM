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
*   This file contains the source for the FlexCAN example program.
*
*   NOTE: This is a two node test. It requires the software to be loaded 
*   onto two boards, one board programmed as NODE 1 and the other programmed 
*   as NODE 2.  A properly terminated CAN cable (120 ohms on either end) is 
*   used to connect both boards together.  When running, the boards will 
*   exchange a CAN message once per second, and the following output
*   should be repetitively displayed (where <x> and <y> increment):
*
*       Data transmit: <x>
*       FLEXCAN tx update message. result: 0x0
*       Received data: <y>
*       ID is: 0x321
*       DLC is: 0x1
*
*
*END************************************************************************/


#include <mqx.h>
#include <bsp.h>
#include <lwevent.h>
#include "EEPROM.h"
#include "can_ctrl.h"
#include "api.h"
#include "motor.h"
#include "adc_ctrl.h"
#include "thermostat.h"
#include "ftm.h"
/* Global variables */
uint32_t data_len_code;
uint32_t bit_rate;
uint32_t format;
uint32_t interrupt;
uint32_t TX_mailbox_num;
uint32_t RX_mailbox_num;
uint32_t TX_remote_mailbox_num;
uint32_t RX_remote_mailbox_num;
uint32_t bit_timing0;
uint32_t bit_timing1;
uint32_t frequency;
uint32_t flexcan_mode;
uint32_t flexcan_error_interrupt;
char canID;
extern sThermoPara thermoPara_LD;
extern s_LD_para LD_para;
extern unsigned int base_LDI_ADC;
extern unsigned int base_LDI_DAC;
float can_cur;
extern unsigned char LaserAP;
extern unsigned int  TargetCurr;


#ifndef PSP_MQX_CPU_IS_KINETIS
#include <flexcan.h>
#endif


#if ! BSPCFG_ENABLE_IO_SUBSYSTEM
#error This application requires BSPCFG_ENABLE_IO_SUBSYSTEM defined non-zero in user_config.h. Please recompile BSP with this option.
#endif


#ifndef BSP_DEFAULT_IO_CHANNEL_DEFINED
#error This application requires BSP_DEFAULT_IO_CHANNEL to be not NULL. Please set corresponding BSPCFG_ENABLE_TTYx to non-zero in user_config.h and recompile BSP with this option.
#endif


LWEVENT_STRUCT event;

/* Task template list */
/*
TASK_TEMPLATE_STRUCT MQX_template_list[] = {
   { MAIN_TASK, Main_Task, 1000L, 8L, "Main task", MQX_AUTO_START_TASK},
   { TX_TASK, Tx_Task, 1000L, 7L, "TX task", 0, 0, 0},
   { RX_TASK, Rx_Task, 1000L, 7L, "RX task", 0, 0, 0},
   { 0L, 0L, 0L, 0L, 0L, 0L }
};
*/
#if PSP_MQX_CPU_IS_KINETIS
void MY_FLEXCAN_ISR
(
   /* [IN] FlexCAN base address */
   void   *can_ptr
)
{
   volatile CAN_MemMapPtr        can_reg_ptr;
   volatile uint32_t                               tmp_reg;
   volatile uint32_t                               temp;
      
   can_reg_ptr = (CAN_MemMapPtr)can_ptr;

   /* get the interrupt flag */
   tmp_reg = (can_reg_ptr->IFLAG1 & CAN_IMASK1_BUFLM_MASK);
   // check Tx/Rx interrupt flag and clear the interrupt
   if(tmp_reg){
      /* clear the interrupt and unlock message buffer */
      /* Start CR# 1751 */
      _lwevent_set(&event, tmp_reg);
      can_reg_ptr->IFLAG1 |= tmp_reg;
      /* End CR# 1751 */
      temp = can_reg_ptr->TIMER;
   }/* Endif */

   // Clear all other interrupts in ERRSTAT register (Error, Busoff, Wakeup)
   tmp_reg = can_reg_ptr->ESR1;
   if(tmp_reg & FLEXCAN_ALL_INT){
      /* Start CR# 1751 */
      can_reg_ptr->ESR1 |= (tmp_reg & FLEXCAN_ALL_INT);
      /* End CR# 1751 */
   } /* Endif */
      
   return;
}
#else
void MY_FLEXCAN_ISR
(
   /* [IN] FlexCAN base address */
   void   *can_ptr
)
{
   volatile FLEXCAN_REG_STRUCT_PTR        can_reg_ptr;
   volatile uint32_t                               tmp_reg;
   volatile uint32_t                               temp;
      
   can_reg_ptr = (FLEXCAN_REG_STRUCT_PTR)can_ptr;

   /* get the interrupt flag */
   tmp_reg = (can_reg_ptr->IFLAG & FLEXCAN_IMASK_VALUE);
   // check Tx/Rx interrupt flag and clear the interrupt
   if(tmp_reg){
      /* clear the interrupt and unlock message buffer */
      /* Start CR# 1751 */
      _lwevent_set(&event, tmp_reg);
      can_reg_ptr->IFLAG |= tmp_reg;
      /* End CR# 1751 */
      temp = can_reg_ptr->TIMER;
   }/* Endif */

   // Clear all other interrupts in ERRSTAT register (Error, Busoff, Wakeup)
   tmp_reg = can_reg_ptr->ERRSTAT;
   if(tmp_reg & FLEXCAN_ALL_INT){
      /* Start CR# 1751 */
      can_reg_ptr->ERRSTAT |= (tmp_reg & FLEXCAN_ALL_INT);
      /* End CR# 1751 */
   } /* Endif */
      
   return;
}
#endif


char getCanID()
{  
    return readIDfromFlash();
}



/*TASK*-----------------------------------------------------------
*
* Task Name : Main_Task
* Comments :
* 
*
*END*-----------------------------------------------------------*/
void canInit_Task(uint32_t parameter)
{ /* Body */
   uint32_t result;

   _int_install_unexpected_isr();
   
   if (_bsp_flexcan_io_init(CAN_DEVICE) != 0)
   {
      printf ("\nError initializing pins for FlexCAN device %d!\n", CAN_DEVICE);
      _task_block();
   }

   printf("\n*********FLEXCAN TEST PROGRAM.*********");
   printf("\n   Message format: Standard (11 bit id)");
   printf("\n   Message buffer 0 used for Tx and Rx.");
   printf("\n   Interrupt Mode: Enabled");
   printf("\n   Operation Mode: TX and RX --> Normal");
   printf("\n***************************************\n");
    canID = getCanID();
   frequency = 1000;
   printf("\nselected frequency (Kbps) is: %d", frequency);

   data_len_code = 1;
   printf("\nData length: %d", data_len_code);
  
   /* Select message format */
   format = FLEXCAN_STANDARD;
   
   /* Select mailbox number */
   RX_mailbox_num = 0;
   TX_mailbox_num = 1;
   RX_remote_mailbox_num = 2;
   TX_remote_mailbox_num = 3;
 
   /* We use default settings */
   bit_timing0 = bit_timing1 = 0;
   
   /* Select operation mode */
   flexcan_mode = FLEXCAN_NORMAL_MODE;

   /* Enable interrupt */
   interrupt = FLEXCAN_ENABLE;
   
   /* Enable error interrupt */
   flexcan_error_interrupt = 1;
   
   /* Reset FLEXCAN device */
   result = FLEXCAN_Softreset ( CAN_DEVICE);
   printf("\nFLEXCAN reset. result: 0x%lx", result);

   /* Initialize FLEXCAN device */
   result = FLEXCAN_Initialize ( CAN_DEVICE, bit_timing0, bit_timing1, frequency, FLEXCAN_IPBUS_CLK);
   printf("\nFLEXCAN initilization. result: 0x%lx", result);

    /* Select mode */
   result = FLEXCAN_Select_mode( CAN_DEVICE, flexcan_mode);
   printf("\nFLEXCAN mode selected. result: 0x%lx", result);

   result = FLEXCAN_Set_global_stdmask (CAN_DEVICE, 0x7fe );
   printf("\nFLEXCAN global mask. result: 0x%lx", result);

   /* Enable error interrupts */
   if(flexcan_error_interrupt == 1)
   {
      result = FLEXCAN_Install_isr_err_int( CAN_DEVICE, MY_FLEXCAN_ISR );
      printf("\nFLEXCAN Error ISR install, result: 0x%lx", result);

      result = FLEXCAN_Install_isr_boff_int( CAN_DEVICE, MY_FLEXCAN_ISR  );
      printf("\nFLEXCAN Bus off ISR install, result: 0x%lx", result);

      result = FLEXCAN_Error_int_enable(CAN_DEVICE);
      printf("\nFLEXCAN error interrupt enable. result: 0x%lx", result);
   }

  /* Set up an event group */
    result = _lwevent_create(&event, LWEVENT_AUTO_CLEAR);
    if (result != MQX_OK) {
        printf("\nCannot create lwevent");
    }   



   /* Start FLEXCAN */
   result = FLEXCAN_Start(CAN_DEVICE);
   printf("\nFLEXCAN started. result: 0x%lx", result);

   // start the ping pong
   //while(1){}

} /* EndBody */

/*TASK*-----------------------------------------------------------
*
* Task Name : Tx_Task
* Comments :
* 
*
*END*-----------------------------------------------------------*/
void Tx_Task(uint32_t parameter)
{/* Body */   
   uint32_t result;
	 float f;
   unsigned char str[] = 
   {
     0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0, 0x0
   };

	 char* cptr;
	 //int i;

   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 2, canID|0x080,
                                        2, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN tx alarm mailbox initialization. result: 0x%lx", result);

   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 4, canID|0x180,
                                        2, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN tx state mailbox initialization. result: 0x%lx", result);
   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 6, canID|0x280,
                                        8, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN tx cur mailbox initialization. result: 0x%lx", result);
   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 8, canID|0x380,
                                        8, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN tx tem mailbox initialization. result: 0x%lx", result);
   
   /* Initialize mailbox */
   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 10, canID|0x480,
                                        8, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN tx reg initialization. result: 0x%lx", result);
   /* Initialize mailbox */
   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 12, canID|0x580,
                                        8, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN tx file initialization. result: 0x%lx", result);
   /* Initialize mailbox */
   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 14, canID|0x700,
                                        2, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN tx heartBeat initialization. result: 0x%lx", result);
  
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 2, FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE);
   printf("\nFLEXCAN tx alarm mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 4, FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE);
   printf("\nFLEXCAN tx state mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 6, FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE);
   printf("\nFLEXCAN tx cur mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 8, FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE);
   printf("\nFLEXCAN tx tem mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 10, FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE);
   printf("\nFLEXCAN tx reg mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 12, FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE);
   printf("\nFLEXCAN tx file mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 14, FLEXCAN_TX_MSG_BUFFER_NOT_ACTIVE);
   printf("\nFLEXCAN tx heartBeat mailbox activation. result: 0x%lx", result);
      
   while(1)
   {    
      /* Let Rx Task receive message */
      _time_delay(1000);
/*
      result = FLEXCAN_Tx_mailbox(CAN_DEVICE, 14, &data);
      if(result != FLEXCAN_OK)
         printf("\nTransmit error. Error Code: 0x%lx", result);
			data = 0x0;
			data = (thermoPara_LD.en_tec<<1)|LDstatus(LD0);
      result = FLEXCAN_Tx_mailbox(CAN_DEVICE, 4, &data);
      if(result != FLEXCAN_OK)
         printf("\nTransmit error. Error Code: 0x%lx", result);
*/			
		  float qq = GetCurrent();
			cptr = (char*)&qq;
			str[0] = cptr[0];
			str[1] = cptr[1];
			str[2] = cptr[2];
			str[3] = cptr[3];	
			f = GetTemp();			
		//	f = LOWTEMLD + f*(HIGHTEMLD-LOWTEMLD);
			cptr = (char*)&f;
			str[4] = cptr[0];
			str[5] = cptr[1];
			str[6] = cptr[2];
			str[7] = cptr[3];
      result = FLEXCAN_Tx_mailbox(CAN_DEVICE, 8, &str);
      if(result != FLEXCAN_OK)
         printf("\nTransmit error. Error Code: 0x%lx", result);
      
			
		
      str[0] = (LDstatus()) | (thermoPara_LD.en_tec<<1) ;

      result = FLEXCAN_Update_message(CAN_DEVICE, str, 2, format, 4);

			
   }
} /* EndBody */

/*TASK*-----------------------------------------------------------
*
* Task Name : Rx_Task
* Comments :
* 
*
*END*-----------------------------------------------------------*/
void Rx_Task(uint32_t parameter)
{/* Body */   
   //unsigned char   dptr[8];
   uint32_t result;
   //uint32_t DLC = 0;
   //uint32_t ID = 0;


   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 1, canID,
                                        8, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN rx remote mailbox initialization. result: 0x%lx", result);

   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 3, canID|0x100,
                                        2, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN rx remote mailbox initialization. result: 0x%lx", result);

   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 5, canID|0x200,
                                        2, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN rx remote mailbox initialization. result: 0x%lx", result);

   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 7, canID|0x300,
                                        4, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN rx remote mailbox initialization. result: 0x%lx", result);

   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 9, canID|0x400,
                                        4, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN rx remote mailbox initialization. result: 0x%lx", result);

   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 11, canID|0x500,
                                        8, FLEXCAN_TX, format,
                                        interrupt);
   printf("\nFLEXCAN rx remote mailbox initialization. result: 0x%lx", result);

   /* Initialize mailbox */
   result = FLEXCAN_Initialize_mailbox( CAN_DEVICE, 13, canID|0x600,
                                        8, FLEXCAN_RX, format,
                                        interrupt);
   printf("\nFLEXCAN rx mailbox initialization. result: 0x%lx", result);
   
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 1, FLEXCAN_RX_MSG_BUFFER_EMPTY);
   printf("\nFLEXCAN receive mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 3, FLEXCAN_RX_MSG_BUFFER_EMPTY);
   printf("\nFLEXCAN receive mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 5, FLEXCAN_RX_MSG_BUFFER_EMPTY);
   printf("\nFLEXCAN receive state mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 7, FLEXCAN_RX_MSG_BUFFER_EMPTY);
   printf("\nFLEXCAN receive  cur mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 9, FLEXCAN_RX_MSG_BUFFER_EMPTY);
   printf("\nFLEXCAN receive tem mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 11, FLEXCAN_RX_MSG_BUFFER_EMPTY);
   printf("\nFLEXCAN receive reg mailbox activation. result: 0x%lx", result);
   result = FLEXCAN_Activate_mailbox(CAN_DEVICE, 13, FLEXCAN_RX_MSG_BUFFER_EMPTY);
   printf("\nFLEXCAN receive file mailbox activation. result: 0x%lx", result);


   /* Install ISR */
   if(interrupt == FLEXCAN_ENABLE)
   {
      result = FLEXCAN_Install_isr( CAN_DEVICE, 1, MY_FLEXCAN_ISR );
      printf("\nFLEXCAN RX ISR install. result: 0x%lx", result);
		 
      result = FLEXCAN_Install_isr( CAN_DEVICE, 3, MY_FLEXCAN_ISR );
      printf("\nFLEXCAN RX ISR install. result: 0x%lx", result);
		 
      result = FLEXCAN_Install_isr( CAN_DEVICE, 5, MY_FLEXCAN_ISR );
      printf("\nFLEXCAN RX ISR install. result: 0x%lx", result);
		 
      result = FLEXCAN_Install_isr( CAN_DEVICE, 7, MY_FLEXCAN_ISR );
      printf("\nFLEXCAN RX ISR install. result: 0x%lx", result);
		 
      result = FLEXCAN_Install_isr( CAN_DEVICE, 9, MY_FLEXCAN_ISR );
      printf("\nFLEXCAN RX ISR install. result: 0x%lx", result);

      result = FLEXCAN_Install_isr( CAN_DEVICE, 11, MY_FLEXCAN_ISR  );
      printf("\nFLEXCAN RX remote ISR install. result: 0x%lx", result);
		 
      result = FLEXCAN_Install_isr( CAN_DEVICE, 13, MY_FLEXCAN_ISR  );
      printf("\nFLEXCAN RX remote ISR install. result: 0x%lx", result);
   }

/*
  if (_lwevent_wait_ticks(&event, 1 << RX_mailbox_num, FALSE, 0) != MQX_OK) {
     printf("\nEvent Wait failed");
  }

  result = FLEXCAN_Lock_mailbox (CAN_DEVICE, RX_mailbox_num);
  if(result != FLEXCAN_OK) 
  {
     printf("\nLock mailbox failed. Error Code: 0x%lx", result);
  }

  result = FLEXCAN_Rx_message(CAN_DEVICE, RX_mailbox_num, &ID, format, 
                             &DLC, &dptr, interrupt);
  if(result != FLEXCAN_OK)
     printf("\nReceived error. Error Code: 0x%lx", result);
  else
  {
     printf("\nReceived data: ");
     for (result = 0; result < DLC; result++) printf ("0x%x ", dptr[result]);
     printf("\nID is: 0x%x", ID);
     printf("\nDLC is: 0x%x\n", DLC);
  }

  result = FLEXCAN_Unlock_mailbox (CAN_DEVICE);
  if(result != FLEXCAN_OK) 
  {
     printf("\nUnlock mailbox failed. Error Code: 0x%lx", result);
  }*/

   
  /* while(1)
   {            
   
      if (_lwevent_wait_ticks(&event, 1 << RX_mailbox_num, FALSE, 0) != MQX_OK) {
         printf("\nEvent Wait failed");
      }
   
      result = FLEXCAN_Lock_mailbox (CAN_DEVICE, RX_mailbox_num);
      if(result != FLEXCAN_OK) 
      {
         printf("\nLock mailbox failed. Error Code: 0x%lx", result);
      }
     
      result = FLEXCAN_Rx_message(CAN_DEVICE, RX_mailbox_num, &ID, format, 
                             &DLC, &dptr, interrupt);
      if(result != FLEXCAN_OK)
         printf("\nReceived error. Error Code: 0x%lx", result);
      else
      {
         printf("\nReceived data: ");
         for (result = 0; result < DLC; result++) printf ("0x%x ", dptr[result]);
         printf("\nID is: 0x%x", ID);
         printf("\nDLC is: 0x%x\n", DLC);
      }

      result = FLEXCAN_Unlock_mailbox (CAN_DEVICE);
      if(result != FLEXCAN_OK) 
      {
         printf("\nUnlock mailbox failed. Error Code: 0x%lx", result);
      }
      
      //FLEXCAN_Request_message (CAN_DEVICE, RX_remote_mailbox_num, format);
     
   }*/
} /* EndBody */

void Rx_state_Task(uint32_t parameter)
{/* Body */   
   unsigned char   dptr[8];
   uint32_t result;
   uint32_t DLC = 0;
   uint32_t ID = canID|0x200;
	while(1)
   {            
   
      if (_lwevent_wait_ticks(&event, 1 << 5, FALSE, 0) != MQX_OK) {
         printf("\nEvent Wait failed");
      }
   
      result = FLEXCAN_Lock_mailbox (CAN_DEVICE, 5);
      if(result != FLEXCAN_OK) 
      {
         printf("\nLock mailbox failed. Error Code: 0x%lx", result);
      }
     
      result = FLEXCAN_Rx_message(CAN_DEVICE, 5, &ID, format, 
                             &DLC, &dptr, interrupt);
      if(result != FLEXCAN_OK)
         printf("\nReceived error. Error Code: 0x%lx", result);
      else
      {
				/*
         printf("\nReceived data: ");
         for (result = 0; result < DLC; result++) printf ("0x%x ", dptr[result]);
         printf("\nID is: 0x%x", ID);
         printf("\nDLC is: 0x%x\n", DLC);
				*/
				
				 LDswitch(LD0,dptr[0]&0x01);
				 TECswitch(LD0,(dptr[0]>>1)&0x01);
				 TECswitch(PAD,(dptr[0]>>3)&0x01);
				 if( ((dptr[0]>>4)&0x01) == 0x01 )
				 {
					 LDswitch(LD1,(dptr[0]>>4)&0x01);
				 }
				 TECswitch(LD1,(dptr[0]>>5)&0x01);
      }

      result = FLEXCAN_Unlock_mailbox (CAN_DEVICE);
      if(result != FLEXCAN_OK) 
      {
         printf("\nUnlock mailbox failed. Error Code: 0x%lx", result);
      }
      _time_delay(500);
     
   }
}
//void Rx_reg_Task(uint32_t parameter)
//{/* Body */   
//   unsigned char   dptr[8];
//   uint32_t result;
//   uint32_t DLC = 0;
//   uint32_t ID = 0;
//	while(1)
//   {            
//   
//      if (_lwevent_wait_ticks(&event, 1 << 11, FALSE, 0) != MQX_OK) {
//         printf("\nEvent Wait failed");
//      }
//   
//      result = FLEXCAN_Lock_mailbox (CAN_DEVICE, 11);
//      if(result != FLEXCAN_OK) 
//      {
//         printf("\nLock mailbox failed. Error Code: 0x%lx", result);
//      }
//     
//      result = FLEXCAN_Rx_message(CAN_DEVICE, 11, &ID, format, 
//                             &DLC, &dptr, interrupt);
//      if(result != FLEXCAN_OK)
//         printf("\nReceived error. Error Code: 0x%lx", result);
//      else
//      {
//				/*
//         printf("\nReceived data: ");
//         for (result = 0; result < DLC; result++) printf ("0x%x ", dptr[result]);
//         printf("\nID is: 0x%x", ID);
//         printf("\nDLC is: 0x%x\n", DLC);
//				*/
//				
//				unsigned int k;
//				 if(dptr[0]>0)
//				 {
//					 if(dptr[2] != 0x0)
//					 {
//						 CAN_LongRead(dptr);
//					 }
//					 else
//					 {
//						CAN_LongWrite(dptr);
//					 }
//				 }
//					else if(dptr[2] != 0x0)
//					{
//						int dat = SingleRead(dptr[1]);
//						char data[8] = {dptr[0],dptr[1],0xFF,0x0,dat,dat>>8,0x0,0x0};
//						result = FLEXCAN_Tx_mailbox(CAN_DEVICE, 10, &data);
//						if(result != FLEXCAN_OK)
//							printf("\nTransmit error. Error Code: 0x%lx", result);
//					}
//					else
//					{
//						k = dptr[4] + (dptr[5]<<8);
//						SingleWrite(dptr[1], k);
//					}
//				 
//       }

//      result = FLEXCAN_Unlock_mailbox (CAN_DEVICE);
//      if(result != FLEXCAN_OK) 
//      {
//         printf("\nUnlock mailbox failed. Error Code: 0x%lx", result);
//      }
//      _time_delay(20);
//      
//     
//   }
//}
#define MaxBuff	20
unsigned char can_buff_index=0;
unsigned char   can_buff[MaxBuff][8];
void Rx_reg_Task(uint32_t parameter)
{/* Body */   
   unsigned char   dptr[8];
	
   uint32_t result;
   uint32_t DLC = 0;
   uint32_t ID = 0;
	while(1)
   {            
   
      if (_lwevent_wait_ticks(&event, 1 << 11, FALSE, 0) != MQX_OK) {
         printf("\nEvent Wait failed");
      }
   
      result = FLEXCAN_Lock_mailbox (CAN_DEVICE, 11);
      if(result != FLEXCAN_OK) 
      {
         printf("\nLock mailbox failed. Error Code: 0x%lx", result);
      }
     
      result = FLEXCAN_Rx_message(CAN_DEVICE, 11, &ID, format, 
                             &DLC, &dptr, interrupt);
      if(result != FLEXCAN_OK){
         printf("\nReceived error. Error Code: 0x%lx", result);		
			}
      else
      {
				can_buff_index = can_buff_index%MaxBuff;
				 for (result = 0; result < DLC; result++) 
						can_buff[can_buff_index][result] = dptr[result];
				can_buff_index++;//if 20 
				_lwevent_set(&event, 1<<31);
			
       }
      result = FLEXCAN_Unlock_mailbox (CAN_DEVICE);
      if(result != FLEXCAN_OK) 
      {
         printf("\nUnlock mailbox failed. Error Code: 0x%lx", result);
      }
     
   }
}
void Rx_file_Task(uint32_t parameter)
{/* Body */   
   unsigned char   dptr[8];
   uint32_t result;
   uint32_t DLC = 0;
   uint32_t ID = 0;
	while(1)
   {            
			_time_delay(1);
      if (_lwevent_wait_ticks(&event, 1 << RX_mailbox_num, FALSE, 0) != MQX_OK) {
         printf("\nEvent Wait failed");
      }
   
      result = FLEXCAN_Lock_mailbox (CAN_DEVICE, RX_mailbox_num);
      if(result != FLEXCAN_OK) 
      {
         printf("\nLock mailbox failed. Error Code: 0x%lx", result);
      }
     
      result = FLEXCAN_Rx_message(CAN_DEVICE, 13, &ID, format, 
                             &DLC, &dptr, interrupt);
      if(result != FLEXCAN_OK)
         printf("\nReceived error. Error Code: 0x%lx", result);
      else
      {
      }

      result = FLEXCAN_Unlock_mailbox (CAN_DEVICE);
      if(result != FLEXCAN_OK) 
      {
         printf("\nUnlock mailbox failed. Error Code: 0x%lx", result);
      }
      
     
   }
}

void CAN_LongRead(unsigned char* dptr)
{
	char c[4];
	float f;
	char* pf;
	if(dptr[0] == 1)
	{
		if(dptr[1] == 0)
		{
			c[0] = (thermoPara_LD.en_tec <<1 ) | (LDstatus() & 0x01);
		}
		pf = c;
	}
	if(dptr[0] == 2)
	{
		switch(dptr[1])
		{
			case 0:f = readKpfromFlash()/1000.0;break;
			case 2:f = readKifromFlash()/1000.0;break;
			case 4:f = readKdfromFlash()/1000.0;break;
		}
		pf = (char*)&f;
	}
	if(dptr[0] == 3)
	{
		if(dptr[1] == 0)
		{
			//f = ((readDAC(D_ChCur)/4095.0 * 1.2)-0.4085)*20;
			f = GetTargetCurrentWithDAC();
			if(f < 0)
			{
				f = 0;
			}
		}
		if(dptr[1] == 2)
		{
			f = GetTarTemp();;
		}
		pf = (char*)&f;
	}
	if(dptr[0] == 4)
	{
		if(dptr[1] == 0)
		{
			f = GetTemp();;
		}
		if(dptr[1] == 2)
		{
		//	f = ((ConReadADC(A_RealLDCur)- base_LDI_ADC)/32767.0*3 )*10;	//error yuejie
			f = GetCurrent();//GetLDI();
		}
		if(dptr[1] == 4)
		{
			f = ((LD_para.BaisI 	- base_LDI_ADC)/32767.0*3 )*10;	//error yuejie
			if(f < 0 || f>32767)
			{
				f=0;
			}
		}
		pf = (char*)&f;
	}
	char data[8] = {dptr[0],dptr[1],0xFF,0x0,pf[0],pf[1],pf[2],pf[3]};
	uint32_t result = FLEXCAN_Tx_mailbox(CAN_DEVICE, 10, &data);
	if(result != FLEXCAN_OK)
		printf("\nTransmit error. Error Code: 0x%lx", result);
}
void CAN_LongWrite(unsigned char* dptr)
{
	int i;
	float* pf;
	if(dptr[0] == 1)
	{
		if(dptr[1] == 0)
		{
			LDswitch(LD0, dptr[4]&0x01);
			thermoPara_LD.en_tec = ((dptr[4]>>1)&0x01);
		}
	}
	if(dptr[0] == 2)
	{
		pf = (float*)(dptr+4);
		switch(dptr[1])
		{
			case 0:thermoPara_LD.T_Kp = (*pf) *1000;
			case 2:thermoPara_LD.T_Ki = (*pf) *1000;
			case 4:thermoPara_LD.T_Kd = (*pf) *1000;
		}
	}
	if(dptr[0] == 3)
	{
		pf = (float*)(dptr+4);
		if(dptr[1] == 0)
		{	
			i = GetDACValueWithCurrent(*pf);
			//	i = (*pf*0.05+0.4085)/1.2 *4095;
		
			if(dptr[3] == 0xFF)
			{
				LD_para.I_Target = i;
				LD_para.curctrlflag = 1;
			}
			if(dptr[3] == 0x0)	 
			{
				LD_para.I_Target = i;
				writeDAC(D_ChCur,i);
			}
			if(dptr[3] == 0xaa)	//add 1 ge 
			{
				if(LaserAP){			//AP ON 
					LD_para.I_Target = i;
					writeDAC(D_ChCur,i);
				}
				else{
					LD_para.I_Target = i;
				}
			}
			if(dptr[3] == 0x55)	//add 1 ge 
			{
					LD_para.BaisI = i;				
			}
		}
		if(dptr[1] == 2)
		{		
			thermoPara_LD.T_Target = GetADCWithTemp(*pf);//(*pf - 12.633)*32767.0/3/15.906;
		}
		if(dptr[1] == 4)
		{		
			if(*pf   > 20)
			{
				i = 4095;
			}
			else
			{
				i = GetDACValueWithCurrent(*pf);
			//	i = (*pf*0.05+0.4085)/1.2 *4095;
			}
			LD_para.BaisI = i;		
		}
	}
	if(dptr[0] == 4)
	{
	}

}


unsigned char handleindex=0;
void CanHandle_Task(uint32_t dummy)
{
	unsigned index = 0;
	uint32_t result;
	while(1)
	{
		if(handleindex<can_buff_index){
			index = can_buff_index;	//1-200
		}
		else if(handleindex>can_buff_index){
			index = MaxBuff;	//1-200
		}
		else{
		 if (_lwevent_wait_ticks(&event, 1 << 31, FALSE, 0) != MQX_OK) {
         printf("\nEvent Wait failed");
     }
		 index = can_buff_index;	//1-200
	 }	
	//	index = can_buff_index;	//1-200
		 while(handleindex<index){//18		//error index=20
			
			 
				unsigned int k;
			 handleindex = handleindex%MaxBuff;	//0-7
			 /*
			 printf("@-");
			 for(i=0;i<8;i++){
				 printf("%0x-",can_buff[handleindex][i]);
			 }
			 printf("\n");
			 */
				 if(can_buff[handleindex][0]>0)
				 {
					 if(can_buff[handleindex][2] != 0x0)
					 { 
						 CAN_LongRead(can_buff[handleindex]);						
					 }
					 else
					 {
						CAN_LongWrite(can_buff[handleindex]);
					 }
				 }
					else if(can_buff[handleindex][2] != 0x0)
					{
						int dat = SingleRead(can_buff[handleindex][1]);
						char data[8] = {can_buff[handleindex][0],can_buff[handleindex][1],0xFF,0x0,dat,dat>>8,0x0,0x0};
						result = FLEXCAN_Tx_mailbox(CAN_DEVICE, 10, &data);
						if(result != FLEXCAN_OK)
							printf("\nTransmit error. Error Code: 0x%lx", result);
					}
					else
					{
						k = can_buff[handleindex][4] + (can_buff[handleindex][5]<<8);
						SingleWrite(can_buff[handleindex][1], k);
					}
					
			 handleindex++;
			 if(handleindex == MaxBuff){
				 handleindex=0;
					break;
			 }
			}
		 if(can_buff_index == MaxBuff)
			 can_buff_index = 0;
	
	}
}


/* EOF */
