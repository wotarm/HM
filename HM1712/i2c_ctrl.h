#ifndef _I2C_H_
#define _I2C_H_
#include <mqx.h>
#include <bsp.h>

/* Global variables */
extern LWSEM_STRUCT lock;

/* Function prototypes */
//void i2c_init(void);
//void i2c_write_ADS1100_polled( MQX_FILE_PTR fd, uint32_t addr, unsigned char  cmd);
//unsigned int i2c_read_ADS1100_polled(MQX_FILE_PTR fd,uint32_t addr);

float get_i2c_tem(char channel);
void i2c_init(void);
unsigned int readADS_Data(char channel);
void writeADS_Cfg(unsigned char dat, char channel);
unsigned int get_i2c_value(char channel);
#endif
