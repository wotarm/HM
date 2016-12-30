#include "fpga.h"

#include "spi.h"
#include "gpio.h"

extern char clock_16;
extern char ifRead;
void writeFPGA(unsigned char addr, unsigned int dat)
{
	SPI_ReadWriteByte(HW_SPI0 , HW_CTAR0, (uint8_t)(WFPGA|addr), HW_SPI_CS0, kSPI_PCS_KeepAsserted);
	SPI_ReadWriteByte(HW_SPI0 , HW_CTAR0, (uint8_t)(dat&0xFF00)>>8, HW_SPI_CS0, kSPI_PCS_KeepAsserted);
	SPI_ReadWriteByte(HW_SPI0 , HW_CTAR0, (uint8_t)(dat&0x00FF), HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	//SPI_MCUWriteByte(WFPGA|addr);
	//SPI_MCUWriteByte(dat);
	//FPGA_CS2 = 1;
}


unsigned int readFPGA(unsigned char addr)
{
	unsigned int dat;
	
	SPI_ReadWriteByte(HW_SPI0 , HW_CTAR0, (uint8_t)(RFPGA|addr), HW_SPI_CS0, kSPI_PCS_KeepAsserted);
	dat = SPI_ReadWriteByte(HW_SPI0 , HW_CTAR0, (uint8_t)(dat&0xFF00)>>8, HW_SPI_CS0, kSPI_PCS_KeepAsserted);
	dat +=SPI_ReadWriteByte(HW_SPI0 , HW_CTAR0, (uint8_t)(dat&0x00FF), HW_SPI_CS0, kSPI_PCS_ReturnInactive);
	//SPI_MCUWriteByte(RFPGA|addr);
	//dat = SPI_MCUReadByte();
	//FPGA_CS2 =1;
	return dat;
}
void FPGA_initial()
{
	SIM->SCGC6 |= SIM_SCGC6_SPI0_MASK;
    PORT_PinMuxConfig(HW_GPIOC, 4, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOC, 5, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOC, 6, kPinAlt2); 
    PORT_PinMuxConfig(HW_GPIOC, 7, kPinAlt2);
    GPIO_QuickInit(HW_GPIOE, 28, kGPIO_Mode_OPP);
	SPI_QuickInit(SPI0_SCK_PC05_SOUT_PC06_SIN_PC07, kSPI_CPOL0_CPHA0, 1*1000*1000);
	SPI_CTARConfig(HW_SPI0,HW_CTAR0,kSPI_CPOL0_CPHA0,8, kSPI_MSB,1*1000*1000);
	FPGA_reset();
}
void FPGA_reset()
{
    GPIO_WriteBit(HW_GPIOE, 28, 1);
    GPIO_WriteBit(HW_GPIOE, 28, 0);
    GPIO_WriteBit(HW_GPIOE, 28, 1);
}