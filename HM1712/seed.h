#ifndef _SEED_H_
#define _SEED_H_

#include "parameter.h"
#include "spi_ctrl.h"

#define EMRGINT			1
#define SDPREADYINT		2
#define DB25LATCHINT	3
#define DB25MOSWIINT	4
#define DB25APSWIINT	5

unsigned char getLD_enb(void);

void setLD_enb(unsigned char c);

unsigned char getsd_perd_div(void);

void setsd_perd_div(unsigned char c);

unsigned int getsd_wid_coarse(void);

void setsd_wid_coarse(unsigned int c);

unsigned char getsd_wid_fine(void);

void setsd_wid_fine(unsigned char c);

unsigned char getsd_pol(void);

void setsd_pol(unsigned char c);

unsigned int getsd_cntr(void);

unsigned char getdcc_ini(void);

void setdcc_ini(unsigned char c);

unsigned char getsd_wid_fine_step(void);

void setsd_wid_fine_step(unsigned char c);

unsigned char geteom_freq(void);

unsigned char geteom_type(void);

void seteom_type(unsigned char c);

unsigned int geteom_cntr(void);

unsigned int geteom_divider(void);

void seteom_divider(unsigned int divider);

unsigned int geteom_phase(void);

void seteom_phase(unsigned int phase);

unsigned char geteom_phase_pos(void);

void seteom_phase_pos(unsigned char phase);

unsigned char geteom_phase_neg(void);

void seteom_phase_neg(unsigned char phase);

char ifseedPulseReady(void);

void disLD(void);

void enLD(void);

void setdb25frqlim(unsigned int lim);
unsigned int getdb25frqlim(void);
unsigned int getdb25lowlim(void);
void setap_delay(unsigned int delay);
unsigned int getap_delay(void);
unsigned char getDB25enable(void);
void setdb25enable(unsigned char c);
void setdb25mode(unsigned char c);

unsigned char getLatchInt(void);
void setDB25LatchInt(unsigned char c);
unsigned char getLatchPulseLen(void);
void setLatchPulseLen(unsigned char c);
unsigned char getDB25LatchDat(void);
unsigned char getdb25freValLow(void);
unsigned char getdb25freValUp(void);
unsigned char getdb25temAlarm(void);
void setdb25temAlarm(unsigned char c);
unsigned char getdb25laserTurnOffAlarm(void);
void setdb25laserTurnOffAlarm(unsigned char c);
unsigned char getdb25MOAlarm(void);
void setdb25MOAlarm(unsigned char c);
void setEmrgStopInt(unsigned char c);
void setSDPulseReadyInt(unsigned char c);
void setdb25dataLatchInt(unsigned char c);
void setdb25MOswiInt(unsigned char c);
void setdb25APswiInt(unsigned char c);

void setIntEnb(unsigned int i);
void resetFPGA(void);

void mask2write(unsigned char addr, unsigned char bitnum, unsigned char value);
unsigned char mask2read(unsigned char addr,unsigned char bitnum);

#endif
