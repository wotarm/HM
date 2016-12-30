
#include <mqx.h>
#include <bsp.h>
#include "capture.h"
#include "ftm.h"
#include "common_inc.h"

uint32_t freq = 0;
unsigned int pulse_count=0;
static void FTM0_ISR()
{
	uint32_t clock;
	uint32_t InputCaptureValue;
  InputCaptureValue = FTM_GetChlCounter(HW_FTM0, HW_FTM_CH0);
  CLOCK_GetClockFrequency(kBusClock, &clock);
  FTM_SetMoudleCounter(HW_FTM0, 0); 
  freq = (clock/128/InputCaptureValue);
}
void capture_init()
{

	_int_install_isr(INT_FTM0 ,FTM0_IRQHandler,0);
  FTM_IC_QuickInit(FTM0_CH0_PC01, kFTM_ClockDiv128);
  FTM_IC_SetTriggerMode(HW_FTM0, HW_FTM_CH0, kFTM_IC_FallingEdge);
  FTM_CallbackInstall(HW_FTM0, FTM0_ISR);
  FTM_ITDMAConfig(HW_FTM0, kFTM_IT_CH0, true);
}
void cmp0_isr(void* dummy)
{
    //If rising edge
    if ((CMP0_SCR & CMP_SCR_CFR_MASK)==CMP_SCR_CFR_MASK)
    {
        CMP0_SCR |= CMP_SCR_CFR_MASK;  // Clear the flag
        //printf("Rising edge on HSCMP0\n");
			pulse_count++;
    }
    
    //If falling edge
    if ((CMP0_SCR & CMP_SCR_CFF_MASK)==CMP_SCR_CFF_MASK)
    {
        CMP0_SCR |= CMP_SCR_CFF_MASK;  // Clear the flag
        //printf("Falling edge on HSCMP0\n");
    }
}
void hscmp_init(void)
{
    //Set up HSCMP0 in NVIC. ISR put into vector table in isr.h file
		_int_install_isr(INT_CMP0, cmp0_isr, 0);
		_bsp_int_init(INT_CMP0, 7, 0, TRUE);
    //Enable HSCMP module clock
    SIM_SCGC4 |= (SIM_SCGC4_CMP_MASK );

    // Configure CMP.
    CMP_CR0_REG(CMP0_BASE_PTR)   = 0x00;  // Filter and digital hysteresis disabled
    CMP_CR1_REG(CMP0_BASE_PTR)   = 0x17;  // Continuous mode, high-speed compare, unfiltered output, output pin enabled

    CMP_FPR_REG(CMP0_BASE_PTR)   = 0x00;  // Filter disabled
    CMP_SCR_REG(CMP0_BASE_PTR)   = 0x1E;  // Enable rising and falling edge interrupts, clear flags
    CMP_DACCR_REG(CMP0_BASE_PTR) = 0xC7;  // 6-bit reference DAC enabled, output set to vdd/64*(42+1)= 0.80v, (Vin selected is vin1=Vdd)
    CMP_MUXCR_REG(CMP0_BASE_PTR) = 0x27;  // P-input as external channel 4. M-input as 6b DAC = 0.80v, it is adjustable with CMP_DACCR_REG
    CMP_CR1_REG(CMP0_BASE_PTR)  |= CMP_CR1_OPE_MASK; // Enable output pin for HSCMP0.
}
