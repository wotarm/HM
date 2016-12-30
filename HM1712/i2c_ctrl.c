#include <mqx.h>
#include "i2c_ctrl.h"
#include "hw_ptr.h"
#include "common_fun.h"
#include "gpio.h"
#include "parameter.h"
#include "EEPROM.h"

#if ! BSPCFG_ENABLE_I2C0
#error This application requires BSPCFG_ENABLE_I2C0 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif

LWSEM_STRUCT        lock;
char LDADC_code=0;
extern s_HW_ptr HW_ptr;

//void i2c_init()
//{
//	
//   _mqx_int                     param;
//   I2C_STATISTICS_STRUCT        stats;
//	_lwsem_create (&lock, 1);
//   


//   
//   printf ("\n\n-------------- Polled I2C master example --------------\n\n");

//   /* Open the I2C driver */         
//	 HW_ptr.i2cfp = fopen ("i2c0:", NULL);
//   if (HW_ptr.i2cfp == NULL) 
//   {
//      printf ("ERROR opening the I2C driver!\n");
//      _task_block ();
//   }

//   /* Test ioctl commands */
//   param = 100000;
//   printf ("Set current baud rate to %d ... ", param);
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_SET_BAUD, &param))
//   {
//      printf ("OK\n");
//   } else {
//      printf ("ERROR\n");
//   }

//   printf ("Get current baud rate ... ");
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_GET_BAUD, &param))
//   {
//      printf ("%d\n", param);
//   } else {
//      printf ("ERROR\n");
//   }


//	 
//   printf ("Set master mode ... ");
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_SET_MASTER_MODE, NULL))
//   {
//      printf ("OK\n");
//   } else {
//      printf ("ERROR\n");
//   }

//   printf ("Get current mode ... ");
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_GET_MODE, &param))
//   {
//      printf ("0x%02x\n", param);
//   } else {
//      printf ("ERROR\n");
//   }
//	 
//   param = 0x00;
//   printf ("Get station address ... ");
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_GET_STATION_ADDRESS, &param))
//   {
//      printf ("0x%02x\n", param);
//   } else {
//      printf ("ERROR\n");
//   }

//   printf ("Clear statistics ... ");
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_CLEAR_STATISTICS, NULL))
//   {
//      printf ("OK\n");
//   } else {
//      printf ("ERROR\n");
//   }
//   
//   printf ("Get statistics ... ");
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_GET_STATISTICS, (void *)&stats))
//   {
//      printf ("OK\n  Interrupts:  %d\n", stats.INTERRUPTS);
//      printf ("  Rx packets:  %d\n", stats.RX_PACKETS);
//      printf ("  Tx packets:  %d\n", stats.TX_PACKETS);
//      printf ("  Tx lost arb: %d\n", stats.TX_LOST_ARBITRATIONS);
//      printf ("  Tx as slave: %d\n", stats.TX_ADDRESSED_AS_SLAVE);
//      printf ("  Tx naks:     %d\n", stats.TX_NAKS);
//   } else {
//      printf ("ERROR\n");
//   }

//   printf ("Get current state ... ");
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_GET_STATE, &param))
//   {
//      printf ("0x%02x\n", param);
//   } else {
//      printf ("ERROR\n");
//   }
//   
//   param = 0x91;
//   printf ("Set destination address to 0x%02x ... ", param);
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
//   {
//      printf ("OK\n");
//   } else {
//      printf ("ERROR\n");
//   }
//   
//   param = 0x00;
//   printf ("Get destination address ... ");
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_GET_DESTINATION_ADDRESS, &param))
//   {
//      printf ("0x%02x\n", param);
//   } else {
//      printf ("ERROR\n");
//   }
//	 i2c_read_ADS1100_polled(HW_ptr.i2cfp, 0x91);
//	 i2c_write_ADS1100_polled(HW_ptr.i2cfp, 0x90, 0x8c);
//}

///*FUNCTION****************************************************************
//* 
//* Function Name    : i2c_write_eeprom_polled
//* Returned Value   : void
//* Comments         : 
//*   Writes the provided data buffer at address in I2C EEPROMs
//*
//*END*********************************************************************/

//void i2c_write_ADS1100_polled
//   (
//      /* [IN] The file pointer for the I2C channel */
//      MQX_FILE_PTR fd,

//      /* [IN] The address in EEPROM to write to */
//      uint32_t    addr,

//      /* [IN] The array of characters are to be written in EEPROM */
//      unsigned char  cmd

//   )
//{ /* Body */

//   uint32_t       param;
//   _mqx_int    result;
//	 char i2cbuf[8];

//   /* Protect I2C transaction in multitask environment */
//   _lwsem_wait (&lock);

//   printf ("Writing %d bytes to address 0x%08x ...\n", 1, addr);
//      /* I2C bus address also contains memory block index */
//      //param = 0x90;
//			param = 0x48;
//      printf ("  Set I2C bus address to 0x%02x ... ", param);
//      if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
//      {
//         printf ("OK\n");
//      } else {
//         printf ("ERROR\n");
//      }


//      /* Initiate start and send I2C bus address */
//      printf ("  I2C start, send address and get ack ... ");
//      fwrite (i2cbuf, 1, 0, HW_ptr.i2cfp);

//      /* Check ack (device exists) */
//      if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_FLUSH_OUTPUT, &param))
//      {
//         printf ("OK ... ack == %d\n", param);
//         if (param) 
//         {
//            /* Stop I2C transfer */
//            printf ("  Stop transfer ... ");
//            if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_STOP, NULL))
//            {
//               printf ("OK\n");
//            } else {
//               printf ("ERROR\n");
//            }
//            return;
//         }
//      } else {
//         printf ("ERROR\n");
//      }

//			Delay(1);
//      result = fwrite (&cmd, 1, 1, HW_ptr.i2cfp);
//      if (result == 1)
//      {
//         printf ("OK\n");
//      } else {
//         printf ("ERROR\n");
//      }
//      
//      /* Wait for completion */
//      printf ("  Flush ... ");
//      result = fflush (HW_ptr.i2cfp);
//      if (MQX_OK == result)
//      {
//         printf ("OK\n");
//      } else {
//         printf ("ERROR\n");
//      }





//   /* Release the transaction lock */
//   _lwsem_post (&lock);
//} /* Endbody */

//   
///*FUNCTION****************************************************************
//* 
//* Function Name    : i2c_read_eeprom_polled
//* Returned Value   : void
//* Comments         : 
//*   Reads into the provided data buffer from address in I2C EEPROM
//*
//*END*********************************************************************/

//unsigned int i2c_read_ADS1100_polled
//   (
//      /* [IN] The file pointer for the I2C channel */
//      MQX_FILE_PTR fd,

//      /* [IN] The address in EEPROM to read from */
//      uint32_t    addr
//  
//   )
//{ /* Body */
//   _mqx_int    param;
//   _mqx_int    result;
//   char i2cbuf[8];



//   /* Protect I2C transaction in multitask environment */
//   _lwsem_wait (&lock);
//   
//   /* I2C bus address also contains memory block index */
//   //param = 0x91;
//	 param = 0x4A;
//   printf ("  Set I2C bus address to 0x%02x ... ", param);
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_SET_DESTINATION_ADDRESS, &param))
//   {
//      printf ("OK\n");
//   } else {
//      printf ("ERROR\n");
//   }

//   /* Initiate start and send I2C bus address */
//   printf ("  I2C start, send address and get ack ... ");
//   fwrite (i2cbuf, 1, 0, HW_ptr.i2cfp);

//   /* Check ack (device exists) */
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_FLUSH_OUTPUT, &param))
//   {
//      printf ("OK ... ack == %d\n", param);
//      if (param) 
//      {
//         /* Stop I2C transfer */
//         printf ("  Stop transfer ... ");
//         if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_STOP, NULL))
//         {
//            printf ("OK\n");
//         } else {
//            printf ("ERROR\n");
//         }
//         
//         /* Release the transaction lock */
//         _lwsem_post (&lock);
//         
//         return 0xFFFF;
//      }
//   } else {
//      printf ("ERROR\n");
//   }




//	 

//   /* Restart I2C transfer for reading */
//   //printf ("  Initiate repeated start ... ");
//   //if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_REPEATED_START, NULL))
//   //{
//    //  printf ("OK\n");
//   //} else {
//   //   printf ("ERROR\n");
//   //}

//   /* Set read request */
//   param = 2;
//   printf ("  Set number of bytes requested to %d ... ", param);
//   if (I2C_OK == ioctl (HW_ptr.i2cfp, IO_IOCTL_I2C_SET_RX_REQUEST, &param))
//   {
//      printf ("OK\n");
//   } else {
//      printf ("ERROR\n");
//   }

//   /* Read all data */
//   result = fread (i2cbuf, 1, 2, HW_ptr.i2cfp);
//   if (result == 2)
//   {
//      printf ("OK\n");
//   } else {
//      printf ("ERROR\n");
//   }
//      
//   /* Release the transaction lock */
//   _lwsem_post (&lock);

//	 return i2cbuf[0] + (i2cbuf[1]<<8);
//   
//} /* Endbody */

void I2CStart(void) 
{
	GPIO_WriteBit(I2C0_SDA,1);
	GPIO_WriteBit(I2C0_SCL,1);
	Delay(3);
	GPIO_WriteBit(I2C0_SDA,0);
	Delay(3);
	GPIO_WriteBit(I2C0_SCL,0);
}

void I2CStop(void)
{
	GPIO_WriteBit(I2C0_SDA,0);
	GPIO_WriteBit(I2C0_SCL,1);
	Delay(3);
	GPIO_WriteBit(I2C0_SDA,1);
	Delay(3);
	GPIO_WriteBit(I2C0_SCL,0);
}
char I2CWaitAck(void) 
{  
	unsigned char cErrTime=64; 
	GPIO_WriteBit(I2C0_SDA,1);
	Delay(1);
	GPIO_WriteBit(I2C0_SCL,1);
	while(1 == GPIO_ReadBitIn(I2C0_SDA))
	{
		cErrTime--;   
		if (cErrTime==0)
		{
			I2CStop();
			//printf("wait ark timeout");
			return 1;
		}
	}
	GPIO_WriteBit(I2C0_SCL,0);
	return 0;
}
void I2CSendAck(void)
{
	GPIO_WriteBit(I2C0_SDA,0);
	Delay(1);
	GPIO_WriteBit(I2C0_SCL,1);
	Delay(2);
	GPIO_WriteBit(I2C0_SCL,0);
}

void I2CSendNotAck(void)
{
	GPIO_WriteBit(I2C0_SDA,1);
	Delay(1);
	GPIO_WriteBit(I2C0_SCL,1);
	Delay(2);
	GPIO_WriteBit(I2C0_SCL,0);
}

void I2CSendByte(unsigned char c)
{
	char i = 8;
	char sendBit = 0;
	while(i--)
	{
		GPIO_WriteBit(I2C0_SCL, 0);
		sendBit = ((c&0x80)>>7);
		c<<=1;
		GPIO_WriteBit(I2C0_SDA, sendBit);
		Delay(1);
		GPIO_WriteBit(I2C0_SCL, 1);
		Delay(1);
	}
	GPIO_WriteBit(I2C0_SCL, 0);
}

unsigned char I2CReceiveByte(void)
{
	char i = 8;
	unsigned char c = 0;
	unsigned char tc = 0;
	GPIO_WriteBit(I2C0_SDA,1);
	while(i--)
	{
		c<<=1;
		GPIO_WriteBit(I2C0_SCL, 0);
		Delay(3);
		GPIO_WriteBit(I2C0_SCL, 1);
		Delay(3);
		tc = GPIO_ReadBitIn(I2C0_SDA);
		c|=tc;
	}
	GPIO_WriteBit(I2C0_SCL, 0);
	return c;
}
void writeADS_Cfg(unsigned char c, char channel)
{
	I2CStart();
	Delay(2);
	if(0 == channel)
	{
		I2CSendByte(0xD0 + (LDADC_code<<1));
	}
	Delay(2);
	I2CWaitAck();
	Delay(2);
	I2CSendByte(0x12);
	Delay(2);
	I2CWaitAck();
	Delay(2);
	I2CStop();
}

unsigned int readADS_Data(char channel)
{
	unsigned int ret;
	unsigned char c_h,c_l;
	I2CStart();
	Delay(2);
	if(0 == channel)
	{
		I2CSendByte(0xD1 +(LDADC_code<<1));
	}
	Delay(2);
	//GPIO_WriteBit(I2C0_SDA, 0);
	//ctl_reg = GPIO_ReadBit(I2C0_SDA);
	I2CWaitAck();
	Delay(2);
	c_h = I2CReceiveByte();
	Delay(2);
	I2CSendAck();
	Delay(2);
	c_l = I2CReceiveByte();
	Delay(2);
	I2CSendAck();
	Delay(2);
	I2CReceiveByte();
	Delay(2);
	I2CSendNotAck();
	Delay(2);
	I2CStop();
	ret = (c_h<<8) + c_l;
	return ret;
}
float get_i2c_tem(char channel)
{
	short i =0;
	i	= readADS_Data(channel);
	return 24.705 - 0.0104*i;
}
unsigned int get_i2c_value(char channel)
{
	int i = readADS_Data(channel);
	return i;
}
void i2c_init()
{ 
	LDADC_code = ReadByteFromEEprom(E_ADCCode);;
	writeADS_Cfg(0, 0);
	writeADS_Cfg(0, 1);
}
