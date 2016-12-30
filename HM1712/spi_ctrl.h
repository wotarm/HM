#ifndef _SPI_TEST_H_
#define _SPI_TEST_H_
#define SPI_TASK	5

#include <mqx.h>
#include <bsp.h>

extern void spi_task(uint32_t);
void writeFPGA(unsigned char addr, unsigned int dat);
unsigned int readFPGA(unsigned char addr);
void FPGA_reset(void);
void FPGA_init(void);

#endif
