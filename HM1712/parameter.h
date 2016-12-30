/*--------------------------------------------------------------------------
PARAMETER.H

Header file for all parameters applied in Project Kiwi microcontroller.
Copyright (c) 2011 GS Lasers Technology (Shanghai) Co., Ltd
All rights reserved.
Version: EBAKWI-0030A1
Modified Date: July 22, 2014
--------------------------------------------------------------------------*/

#ifndef __PARAMETER_H__
#define __PARAMETER_H__

#include "PublicHead.h"

#if VersionHPower
#define Version		0x29
#else
#define Version		0x1e
#endif
#define Reversion	0x1712

/*------------------------------------------------
	MCU scan strategy
------------------------------------------------*/

#define SNAKE_STRATEGY		1
#define COMB_STRATEGY		2

/*------------------------------------------------
	HW type
------------------------------------------------*/

#define LD0			0x00
#define LD1			0x01
#define PAD			0x02

/*------------------------------------------------
	MCU Special Rigsters Address
------------------------------------------------*/


#define R_ld_enb			3
#define R_sd_perd_div		4
#define R_sd_wid_coarse		5
#define R_sd_wid_fine		5
#define R_sd_polarization	6
#define R_sd_cntr			7
#define R_dcc_ini			8
#define R_sd_wid_fine_step	9
	
#define R_eom_type			16
#define R_eom_divider		17
#define R_eom_phase			18
#define R_eom_phase_pos		18
#define R_eom_phase_neg		18
#define R_eom_cntr			19

#define R_db25_mode			32
#define R_ap_delay			33
#define R_db25_frq_limit	34
#define R_db25_frq_low_lim	35
#define R_db25_alarm		36
#define R_db25_bufin		37
#define R_db25_frq_valid	38


#define R_int_reg			40
#define R_int_enb			41
#define R_hardware_status	42

#define R_resetcmd		126
//#define R_Hand			128	// 8 bits,r/w	handshaking
#define R_MCUVer			129	// 8 bits,r.o	mcu firmware verion
#define R_MCURevL			130	// 8 bits,r/w	mcu firwware revision_low
#define R_MCURevH			131	// 8 bits,r/w	
#define R_enLD				134
#define R_enTEC				135
#define R_moduleSwitch		136

#define R_TargCur			150
#define R_TargTEC			151
#define R_EN_LED			153
#define R_FPGA_INT		154
#define R_HardID			155
#define R_RealPDPwr		156
#define R_RealLDTem			157
#define R_RealTECCur	158
#define R_RealLDCur		159
#define R_TargTem			162

#define R_CANID				180
#define R_PreCur			181
#define R_PreTem			182
#define R_KP				183
#define R_KI				184
#define R_KD				185
#define R_VOLADJ_A			186
#define R_VOLADJ_B			187
#define R_TEMADJ_A			188
#define R_TEMADJ_B			189
#define R_GS_MAC			190
#define R_EOMTYPE			192
#define R_EOMDIV			193
#define R_EOMPHPOS			194
#define R_EOMPHNEG			195
#define R_SDPERDIV			196
#define R_SDWIDCOA			197
#define R_SDWIDFINE			198
#define R_SDPOLAR			199
#define R_INTENB			200
#define R_FPGARST			210
#define R_BWTHERMO		212
#define R_LDADCCODE		222
#define R_CHKCODE			243


#define R_EN_ARM_IN		222
#define R_EN_ARM_OUT	223
#define R_STATUS			224
#define R_STATUSNP1		225
#define R_WORKMODE		221
#define R_EN_LOGIC		230


#define R_FPGARST			210
#define R_WORKSIG			211
#define R_MOTORRESET	219

#define R_SECMOTORSCAN	212
#define R_SCANXSTART	213
#define R_SCANYSTART	214
#define R_SCANXEND		215
#define R_SCANYEND		216
#define R_SCANSTATEGY	217
#define R_SCANSTEP		218

#define R_MO					220

#define R_testAlarm		231
#define R_AlarmL			232	// 8 bits,r/w   report alarm signal
#define R_AlarmH			233	// 5 bits,r/w

#define WORK_DEFAULT	0
#define WORK_AUTO			1


#define CURRENTSTEP 100

#define SAMPLE_DELAY 2500              // Delay in ms before displaying sample

#define ANALOG_INPUTS 4                // Number of AIN pins to measure
#define WindowWidth   50
#define CAN_ID_DEFAULT	0x7F
#if VersionHPower
#define T_DEFAULT     10000
#define T_KPDEFAULT   2000
#define T_KIDEFAULT   4
#define T_KDDEFAULT   0 
#else
#define T_DEFAULT     0x01C4
#define T_KPDEFAULT   1000
#define T_KIDEFAULT   10
#define T_KDDEFAULT   0 
#endif
#define C_DEFAULT     0
#define T_PADEFAULT		1000
#define T_PBDEFAULT		0
#define EOMTYPE_DEFAULT	0
#define EOMDIV_DEFAULT	0
#define EOMPHPOS_DEFAULT	0
#define EOMPHNEG_DEFAULT	0
#define SDPERDIV_DEFAULT	0
#define SDWIDCOA_DEFAULT	0
#define SDWIDFINE_DEFAULT	0
#define SDPOLAR_DEFAULT		0

#define GS_GETSWITCH	3
#define GS_SETSWITCH	4
#define GS_GETI			5
#define GS_SETI			6
#define GS_GETTEC		7
#define GS_SETTEC		8
#define GS_GETTECPARA	9
#define GS_OPERATEREG	10
#define GS_GETFILE		11
#define GS_RECFILE		12

#define GS_MAC_DEFAULT  0xFFFFFFFF
//module status
#define GS_INIT				0
#define GS_CAN_CONNECTED	1
#define GS_CAN_STOP			2
#define GS_CAN_RESET		3
#define GS_CAN_RESETCOM		4

#define TEM_DIR_LIM			250
#define TEM_DIR_FINE_LIM	100

#define TIMER_PRESCALER    250        // Based on Timer CKCON settings
#define PID_INTERVAL       10        // LED toggle rate in milliseconds
 
#define LOWTEM	 	21.38
#define HIGHTEM 	47.04

#define EN_TEC			HW_GPIOA,10
#define EN_LD				HW_GPIOE,10
#define EN_ARM_OUT	HW_GPIOB,9
#define EN_ARM_IN		HW_GPIOA,27
#define STATUS_N		HW_GPIOA,26
#define STATUS_Np1	HW_GPIOB,8
#define LED0			HW_GPIOA, 25
#define LED1			HW_GPIOA, 24

#define LASER_ON_LED	HW_GPIOB,2
#if VersionHPower
#define TEC_DIR		HW_GPIOE,8
#else
#define TEC_DIR		HW_GPIOD,7
#endif

#define WORK_DEFAULT	0
#define WORK_AUTO			1

#define I2C0_SDA	HW_GPIOD, 3
#define I2C0_SCL	HW_GPIOD, 2

/*------------------------------------------------
	Waring/Status Flag byte definition
------------------------------------------------*/
extern unsigned int Alarm;					//alarm signal to upload
//#define	P0TOver		0x0002	//Seed LD is overheat
//#define	P1TOver		0x0004	//Pump1 LD is overheat
//#define	P2TOver		0x0008	//Pump2 LD is overheat
//#define	P3TOver		0x0010	//Pump3 LD is overheat
//#define	CTOver		0x0040	//seed source container happens to overcurrent
//#define FanErr		0x0100
//#define	Vol0Alarm	0x0400	//Report abnormal voltage supply: status 0
//#define	Vol1Alarm	0x0800	//Report abnormal voltage supply: status 1
//#define L24VErr		0x1000	//D24V is beyond range
//#define VRefErr		0x2000	//DAC VRef is beyond range
#define PCMsgErr	0x4000	//'PC signal is incorrect, ALREADY DEFINED IN UART.H
#define CmdBufFull	0x8000	//RxD Queue empty or full
//#define QueueErr	0x0001

extern char TransFlag;


#endif
