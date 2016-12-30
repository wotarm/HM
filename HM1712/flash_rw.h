#ifndef _FLASH_RW_H_
#define _FLASH_RW_H_

#include <mqx.h>
#include <bsp.h> 
#include <fio.h>

#define FalshEEprom			0
#define FLASHSctorNo    127
#define FPGAProgramSctorNo	63
#define SECTOR_SIZE     (4096)

uint8_t FLASH_EraseSector(uint32_t addr);
uint8_t Flash_read(uint16_t sectNo,uint16_t offset,uint16_t cnt,uint8_t* bBuf);
uint8_t Flash_write(uint16_t sectNo,uint16_t offset,uint16_t cnt,uint8_t buf[]);
uint8_t FLASH_WriteSector(uint32_t addr, const uint8_t *buf, uint32_t len);

void FLASH_Init(void);
uint8_t ReadFlashByte(uint32_t addr, char non);
void WriteFlash(uint32_t addr,char *src, unsigned numbytes,  char non);
char * ReadFlash(char *dest, uint32_t addr, unsigned numbytes, char non);
void FlexMemInit(void);

void WriteFlexmem(uint32_t addr,char *src, unsigned numbytes);

char * ReadFlexmem(char *dest, uint32_t addr, unsigned numbytes);

void FlashTest(void);
#endif
