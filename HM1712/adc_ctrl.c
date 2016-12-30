
#include <mqx.h>
#include <bsp.h> 
#include <fio.h>
#include "adc_ctrl.h"
#include "hw_ptr.h"
#include "PublicHead.h"
extern s_HW_ptr HW_ptr;

#if MQX_USE_LWEVENTS
#include <lwevent.h>
#endif

#define ADC0 "adc0:" /* must be #1 as the inputs are wired to ADC 1 */
#define MY_TRIGGER ADC_PDB_TRIGGER
#if !BSPCFG_ENABLE_ADC0
    #error This application requires BSPCFG_ENABLE_ADC1 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif /* BSPCFG_ENABLE_ADCx */

#define ADC1 "adc1:" /* must be #1 as the inputs are wired to ADC 1 */
#define MY_TRIGGER ADC_PDB_TRIGGER
#if !BSPCFG_ENABLE_ADC1
    #error This application requires BSPCFG_ENABLE_ADC1 defined non-zero in user_config.h. Please recompile BSP with this option.
#endif /* BSPCFG_ENABLE_ADCx */

		
#ifndef BSP_DEFAULT_LWADC0_MODULE
#error This application requires BSP_DEFAULT_LWADC_MODULE to be not defined in the BSP. Please recompile BSP with this option.
#endif

#ifndef BSP_DEFAULT_LWADC1_MODULE
#error This application requires BSP_DEFAULT_LWADC_MODULE to be not defined in the BSP. Please recompile BSP with this option.
#endif

extern const LWADC_INIT_STRUCT BSP_DEFAULT_LWADC0_MODULE;
extern const LWADC_INIT_STRUCT BSP_DEFAULT_LWADC1_MODULE;

#if MQX_USE_LWEVENTS
//static LWEVENT_STRUCT evn;
#endif

const ADC_INIT_STRUCT s_adc_init = {
    ADC_RESOLUTION_DEFAULT,     /* resolution */
};


//static void print_lwadc_attribute( LWADC_STRUCT_PTR lwadc_ptr, LWADC_ATTRIBUTE attribute, const char * name)
//{
//    uint32_t value;
//    
//    if (_lwadc_get_attribute(lwadc_ptr,attribute,&value)) {
//        printf("%-30s = 0x%08x (%d)\n", name,value,value);
//    } else {
//        printf("%-30s not supported\n", name);
//    }
//}

#define PRINT_LWADC_ATTRIBUTE(p,x) print_lwadc_attribute(p,x,#x)
//000
//static void print_all_lwadc_attributes( LWADC_STRUCT_PTR lwadc_ptr)
//{
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_RESOLUTION);
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_REFERENCE);
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_FREQUENCY);
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_DIVIDER);
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_DIFFERENTIAL);
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_POWER_DOWN);
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_NUMERATOR);
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_DENOMINATOR);
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_FORMAT);
//    PRINT_LWADC_ATTRIBUTE(lwadc_ptr, LWADC_INPUT_CONVERSION_ENABLE);
//}

void adc_init()
{
		_lwadc_init(&BSP_DEFAULT_LWADC0_MODULE);
		_lwadc_init(&BSP_DEFAULT_LWADC1_MODULE);

		#if VersionHPower
		_lwadc_init_input(&A_RealLDTem,ADC1_SOURCE_ADPM0);
		//_lwadc_init_input(&A_RealLDTem,ADC1_SOURCE_AD10);
		_lwadc_init_input(&A_RealLDVol,ADC0_SOURCE_ADPM1);
		_lwadc_init_input(&A_RealPDPwr,ADC0_SOURCE_ADPM0);
		_lwadc_init_input(&A_RealLDCur,ADC1_SOURCE_ADPM1);
		_lwadc_init_input(&A_RealTecCur,ADC0_SOURCE_AD16);
		_lwadc_init_input(&A_RealTecVol,ADC1_SOURCE_AD16);
		_lwadc_init_input(&A_RealDACRef,ADC0_SOURCE_AD13);
		_lwadc_init_input(&A_RealLDIRef,ADC1_SOURCE_AD10);
		_lwadc_init_input(&A_RealT_HBridge,ADC1_SOURCE_AD11);
		_lwadc_init_input(&A_RealT_DCDC,ADC1_SOURCE_AD12);
		#else
			_lwadc_init_input(&A_RealLDVol,ADC0_SOURCE_AD16);        //36pin
		_lwadc_init_input(&A_RealTem2,ADC1_SOURCE_ADPM0);      
		_lwadc_init_input(&A_RealPDPwr,ADC0_SOURCE_ADPM0); 
		_lwadc_init_input(&A_RealLDCur,ADC1_SOURCE_AD16);         //35pin
		_lwadc_init_input(&A_MCU_VREF,ADC0_SOURCE_AD17);     //PTE24
		_lwadc_init_input(&A_PREF,ADC0_SOURCE_AD18);     //PTE25
		_lwadc_init_input(&A_RealTecCur, ADC0_SOURCE_AD6B);		//PTD5 ADC0_SE6b
		_lwadc_init_input(&A_RealTecVol, ADC0_SOURCE_AD7B);		//PTD6 ADC0_SE7b
		#endif
}


uint32_t ConReadADC(LWADC_STRUCT s)
{
	LWADC_VALUE ret;
	if(_lwadc_wait_next(&s))
	{
		if(_lwadc_read_raw(&s,&ret))
		{
			return ret;
		}
		printf("can not read raw.");
	}
	printf("can not convert value.");
	return 0;
}
