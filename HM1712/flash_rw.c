#include "flash_rw.h"
#include "hw_ptr.h"
#include "ano_MK64F12.h"
#include "common_fun.h"
#include <mqx.h>
#include <bsp.h>
#include <fio.h>
/* flash commands */
#define RD1BLK    0x00  /* read 1 block */
#define RD1SEC    0x01  /* read 1 section */
#define PGMCHK    0x02  /* program check */
#define RDRSRC    0x03  /* read resource */
#define PGM4      0x06  /* program phase program 4 byte */
#define PGM8      0x07  /* program phase program 8 byte */
#define ERSBLK    0x08  /* erase flash block */
#define ERSSCR    0x09  /* erase flash sector */
#define PGMSEC    0x0B  /* program section */
#define RD1ALL    0x40  /* read 1s all block */
#define RDONCE    0x41  /* read once */
#define PGMONCE   0x43  /* program once */
#define ERSALL    0x44  /* erase all blocks */
#define VFYKEY    0x45  /* verift backdoor key */
#define PGMPART   0x80  /* program paritition */
#define SETRAM    0x81  /* set flexram function */
#define NORMAL_LEVEL 0x0

/* function return type */
#define FLASH_OK                    0x00
#define FLASH_OVERFLOW              0x01
#define FLASH_BUSY                  0x02
#define FLASH_ERROR                 0x04
#define FLASH_TIMEOUT               0x08
#define FLASH_NOT_ERASED            0x10
#define FLASH_CONTENTERR            0x11


#define FLEXNVM_PART_CODE_NOPART 0x0F
#define SET_EE_DATA_SIZE_CODE	0x26
#define SET_FLEXNVM_PART_CODE 0x03
#define EEPROM_ADDR		0x14000000

/* disable interrupt before lunch command */
#define CCIF    (1<<7)
#define ACCERR  (1<<5)
#define FPVIOL  (1<<4)
#define MGSTAT0 (1<<0)


#define FTF    FTFE
#define SECTOR_SIZE     (4096)
#define PROGRAM_CMD      PGM8

static uint8_t _CommandLaunch(void)
{
	int tmp = 0;
    /* Clear command result flags */
    FTF->FSTAT = ACCERR | FPVIOL;

    /* Launch Command */
    FTF->FSTAT = CCIF;
		for(;tmp<1024;tmp++);
    /* wait command end */
    while(!(FTF->FSTAT & CCIF));

    /*check for errors*/
    if(FTF->FSTAT & (ACCERR | FPVIOL | MGSTAT0)) return FLASH_ERROR;

    /*No errors retur OK*/
    return FLASH_OK;

}

void FLASH_Init(void)
{
    /* Clear status */
    FTF->FSTAT = ACCERR | FPVIOL;
}
static inline uint8_t FlashCmdStart(void)
{
    /* clear command result flags */
    FTF->FSTAT = ACCERR | FPVIOL;
    FTF->FSTAT = CCIF;
    while(!(FTF->FSTAT & CCIF));
    if(FTF->FSTAT & (ACCERR | FPVIOL | MGSTAT0)) return FLASH_ERROR;
    return FLASH_OK;
}

uint8_t FLASH_EraseSector(uint32_t addr)
{
	int ret;
    union
    {
        uint32_t  word;
        uint8_t   byte[4];
    } dest;
    dest.word = (uint32_t)addr;

    /* set cmd */
    FTF->FCCOB0 = ERSSCR; 
    FTF->FCCOB1 = dest.byte[2];
    FTF->FCCOB2 = dest.byte[1];
    FTF->FCCOB3 = dest.byte[0];
        
//    if(FLASH_OK == _CommandLaunch())
//    {
//        return FLASH_OK;
//    }
//    else
//    {
//        return FLASH_ERROR;
//    }
			 __disable_irq();
    ret = FlashCmdStart();
    __enable_irq();
    
    return ret;
}



void WriteFlash(uint32_t addr,char *src, unsigned numbytes, char non)  
{  
  
   Flash_write(addr/SECTOR_SIZE, addr%SECTOR_SIZE, numbytes, (unsigned char*)src); 
}  
  
char * ReadFlash(char *dest, uint32_t addr, unsigned numbytes, char non)  
{  
   Flash_read(addr/SECTOR_SIZE, addr%SECTOR_SIZE, numbytes, (unsigned char*)dest);  
   return dest;  
}  

uint8_t ReadFlashByte(uint32_t addr, char non)
{
	char ret[4];
	ReadFlash(ret, addr, 1, 0);
	return *ret;
}
uint8_t Flash_read(uint16_t sectNo,uint16_t offset,uint16_t cnt,uint8_t* bBuf)
{
    uint32_t wAddr = 0;
    wAddr = sectNo * 4096 + offset;
    while (cnt--)
        *bBuf++=*(uint8_t*)wAddr++;
   return true;
}


uint8_t Flash_write(uint16_t sectNo,uint16_t offset,uint16_t cnt,uint8_t buf[])
{
    uint32_t size;
    uint32_t destaddr;
    char step = 0;
    union
    {
        uint32_t   word;
        uint8_t  byte[4];
    } dest;
    
    if(offset%4 != 0)
        return 1;   
    
    FTF->FCCOB0 = PROGRAM_CMD;
		switch(PROGRAM_CMD)
    {
        case PGM4:
            step = 4;
            break;
        case PGM8:
            step = 8;
            break;
        default:
            printf("FLASH: no program cmd found!\r\n");
            step = 4;
            break;
    }
		
    destaddr = (uint32_t)(sectNo*(1<<12) + offset);
    dest.word = destaddr;
    for(size=0; size < cnt;size+=step)
    {
        FTF->FCCOB1 = dest.byte[2];
        FTF->FCCOB2 = dest.byte[1];
        FTF->FCCOB3 = dest.byte[0];

        FTF->FCCOB4 = buf[3];
        FTF->FCCOB5 = buf[2];
        FTF->FCCOB6 = buf[1];
        FTF->FCCOB7 = buf[0];
        if(step == 8)
				{
					FTF->FCCOB8 = buf[7];
					FTF->FCCOB9 = buf[6];
					FTF->FCCOBA = buf[5];
					FTF->FCCOBB = buf[4];
				}
				dest.word += step;
				buf += step;
        if(1 == _CommandLaunch())
            return 2;  
    }
    
    return 0;  
}

int partition_flash(int eeprom_size, int dflash_size)
{
      /* Test to make sure the device is not already partitioned. If it
       * is already partitioned, then return with no action performed.
       */
      if ((SIM_FCFG1 & SIM_FCFG1_DEPART(0xF)) != 0x00000F00)
      {
          printf("\nDevice is already partitioned.\n");
          return 0;
      }
      /* Write the FCCOB registers */
			
			__disable_interrupt();
      FTF->FCCOB0 = FTFE_FCCOB0_CCOBn(0x80); // Selects the PGMPART command
      FTF->FCCOB1 = 0x00;
      FTF->FCCOB2 = 0x00;
      FTF->FCCOB3 = 0x00;

      /* FCCOB4 is written with the code for the subsystem sizes (eeprom_size define) */
      FTF->FCCOB4 = eeprom_size;

      /* FFCOB5 is written with the code for the Dflash size (dflash_size define) */
      FTF->FCCOB5 = dflash_size;


      /* All required FCCOBx registers are written, so launch the command */
      FTF->FSTAT = FTFE_FSTAT_CCIF_MASK;

      /* Wait for the command to complete */
      while(!(FTF->FSTAT & FTFE_FSTAT_CCIF_MASK))
			{
				Delay(1);
			}
			__enable_interrupt();

      return 1;
}

void FlexMemInit()
{
	
		SCB_SHCSR|=SCB_SHCSR_BUSFAULTENA_MASK|SCB_SHCSR_MEMFAULTENA_MASK|SCB_SHCSR_USGFAULTENA_MASK;
  	printf("\nRunning FlexMem demo!!\n");
        /* Partition the memory to enable FlexMem mode */
    partition_flash( 0x26, 0x03);
}
/*
uint8_t flexmem_read(uint16_t offset,uint16_t cnt,uint8_t buf[])
{
  uint32_t wAddr = 0;
  wAddr = EEPROM_ADDR + offset;
  while (cnt--)
    *buf++=*(uint8_t*)wAddr++;
  return true;
}
uint8_t flexmem_write(uint16_t offset,uint16_t cnt,uint8_t buf[])
{
	uint32_t wAddr = 0;
	wAddr = EEPROM_ADDR + offset;
	while(cnt--)
		*(uint8_t*)wAddr++ = *buf++;
	return true;
}
*/
void WriteFlexmem(uint32_t addr,char *src, unsigned numbytes)  
{  
	uint32_t wAddr = 0;
	wAddr = EEPROM_ADDR + addr;
	while(numbytes--)
	{
		*(uint8_t*)wAddr++ = *src++;
    while(!(FTF->FCNFG & FTFE_FCNFG_EEERDY_MASK));
		
    //_io_ioctl(HW_ptr.flex_file, FLEXNVM_IOCTL_WAIT_EERDY, NULL); 
	}
}  
  
char * ReadFlexmem(char *dest, uint32_t addr, unsigned numbytes)  
{  
  uint32_t wAddr = 0;
  wAddr = EEPROM_ADDR + addr;
  while (numbytes--)
    *dest++=*(uint8_t*)wAddr++;
  return dest; 
}  




/**********************FLASH Program Test********************/


uint32_t FLASH_GetSectorSize(void)
{
    return SECTOR_SIZE;
}


 /**
 * @brief  flash write sector
 * @note   len must = sector size
 * @param  addr: start addr
 * @param  buf : buffer pointer
 * @param  len : len
 * @retval Flash return code
 */
uint8_t FLASH_WriteSector(uint32_t addr, const uint8_t *buf, uint32_t len)
{
  uint16_t step, ret, i;
	union
	{
		uint32_t  word;
		uint8_t   byte[4];
	} dest;
	dest.word = (uint32_t)addr;
	FTF->FCCOB0 = PROGRAM_CMD;   
  switch(PROGRAM_CMD)
  {
    case PGM4:
        step = 4;
        break;
    case PGM8:
        step = 8;
        break;
    default:
        printf("FLASH: no program cmd found!\r\n");
        step = 4;
        break;
    }
	for(i=0; i<len; i+=step)
	{
        /* set address */
		FTF->FCCOB1 = dest.byte[2];
		FTF->FCCOB2 = dest.byte[1];
		FTF->FCCOB3 = dest.byte[0];

		FTF->FCCOB4 = buf[3];
		FTF->FCCOB5 = buf[2];
		FTF->FCCOB6 = buf[1];
		FTF->FCCOB7 = buf[0];      
    if(step == 8)
    {
			FTF->FCCOB8 = buf[7];
      FTF->FCCOB9 = buf[6];
      FTF->FCCOBA = buf[5];
      FTF->FCCOBB = buf[4];
    }
		dest.word += step; buf += step;
    __disable_irq();
    ret = FlashCmdStart();
    __enable_irq();     
		if(FLASH_OK != ret) 
    {
			return FLASH_ERROR;
    }
  }
  return FLASH_OK;
}

#define DEV_1024K_SIZE            			(0x100000)
#define DEV_512K_SIZE            				(0x80000)
void FlashTest(void)
{
    int addr, i, err, secNo, sector_size;
    uint8_t *p;
    static uint8_t buf[4096];
      
    sector_size = FLASH_GetSectorSize();
    printf("flash sector size:%d\r\n", sector_size);
    
    for(i=0;i<sector_size;i++)
    {
        buf[i] = i % 0xFF;
    }
    
    for (secNo = 109; secNo < (DEV_512K_SIZE/sector_size); secNo++)
    {
        addr = secNo*sector_size;
        printf("program addr:0x%X ", addr);
        err = 0;
        err += FLASH_EraseSector(addr);
        err += FLASH_WriteSector(addr, buf, sector_size);
        if(err)
        {
            printf("issue command failed %d\r\n", err);
            while(1);
        }
        p = (uint8_t*)(addr);
        
        for(i=0;i<sector_size;i++)
        {
            if(*p++ != (i%0xFF))
            {
                err++;
                printf("[%d]:0x%02X ", i, *p);
            }
        }
        if(!err)
        {
            printf("verify OK\r\n");
        }
        else
        {
            printf("verify ERR\r\n");
        }
    }
}
