#ifndef __FPGA_H__
#define __FPGA_H__

#include "spi_test.h"
#define WFPGA		0x80	//Write command and Function command
#define RFPGA		0x00	//Read command
void FPGA_initial(void);
void writeFPGA(unsigned char addr, unsigned int dat);
unsigned int readFPGA(unsigned char addr);
void FPGA_reset(void);


#endif