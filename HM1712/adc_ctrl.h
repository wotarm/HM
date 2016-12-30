#ifndef _ADC_CTRL_H_
#define _ADC_CTRL_H_

#include <mqx.h>
#include <bsp.h> 
#include "hw_ptr.h"

#define A_RealLDCur	HW_ptr.adcLDCur
#define A_RealLDTem	HW_ptr.adcLDTem
#define A_RealTecVol	HW_ptr.adcTecVol
#define A_RealPDPwr 	HW_ptr.adcPDPwr
#define A_RealTecCur	HW_ptr.adcTecCur
#define A_RealLDVol		HW_ptr.adcLDVol
#define A_RealDACRef		HW_ptr.dac_ref
#define A_RealLDIRef		HW_ptr.ldi_ref
#define A_RealT_HBridge	HW_ptr.adcTem_HBridge
#define A_RealT_DCDC		HW_ptr.adcTem_DCDC
#define A_RealTem2	HW_ptr.adcTem2
#define A_PREF				HW_ptr.adcP_Ref
#define A_MCU_VREF		HW_ptr.adcMCU_Vref

void adc_init(void);
uint32_t ConReadADC(LWADC_STRUCT s);

#endif
