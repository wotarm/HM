#include "seed.h"
#include "common_fun.h"

unsigned char getLD_enb()
{
	return readFPGA(R_ld_enb);
}
void setLD_enb(unsigned char c)
{
	writeFPGA(R_ld_enb, c);
}
unsigned char getsd_perd_div()
{
	return readFPGA(R_sd_perd_div);
}
void setsd_perd_div(unsigned char c)
{
	writeFPGA(R_sd_perd_div, c);
} 
unsigned int getsd_wid_coarse()
{
	return readFPGA(R_sd_wid_coarse);
}
void setsd_wid_coarse(unsigned int c)
{
	writeFPGA(R_sd_wid_coarse, c);
}
/*
unsigned char getsd_wid_fine()
{
	return readFPGA(R_sd_wid_fine);
}
void setsd_wid_fine(unsigned char c)
{
	writeFPGA(R_sd_wid_fine, c);
}
*/
unsigned char getsd_pol()
{
	return readFPGA(R_sd_polarization);
}
void setsd_pol(unsigned char c)
{
	writeFPGA(R_sd_polarization, c);
}
unsigned int getsd_cntr()
{
	return readFPGA(R_sd_cntr);
}
unsigned char getdcc_ini()
{
	return readFPGA(R_dcc_ini);
}
void setdcc_ini(unsigned char c)
{
	writeFPGA(R_dcc_ini, c);
}
unsigned char getsd_wid_fine_step()
{
	return readFPGA(R_sd_wid_fine_step);
}
void setsd_wid_fine_step(unsigned char c)
{
	writeFPGA(R_sd_wid_fine_step, c);
}
unsigned char geteom_freq()
{
	return mask2read(R_eom_type, 0);
}
unsigned char geteom_type()
{
	return mask2read(R_eom_type, 1);
}
void seteom_type(unsigned char c)
{
	mask2write(R_eom_type, 1, c);
}
unsigned int geteom_cntr()
{
	return readFPGA(R_eom_cntr);
}
unsigned int geteom_divider()
{
	return readFPGA(R_eom_divider);
}
void seteom_divider(unsigned int divider)
{
	writeFPGA(R_eom_divider, divider);
}
unsigned int geteom_phase()
{
	return readFPGA(R_eom_phase);
}
void seteom_phase(unsigned int phase)
{
	writeFPGA(R_eom_phase, phase);
}
unsigned char geteom_phase_pos()
{
	return readFPGA(R_eom_phase_pos);
}
void seteom_phase_pos(unsigned char phase)
{
	writeFPGA(R_eom_phase_pos, phase);
}
unsigned char geteom_phase_neg()
{
	return readFPGA(R_eom_phase_neg);
}
void seteom_phase_neg(unsigned char phase)
{
	writeFPGA(R_eom_phase_neg, phase);
}

char ifseedPulseReady()
{
	return (mask2read(R_hardware_status, 0) == 0x0);
}
void setdb25frqlim(unsigned int lim)
{
	writeFPGA(R_db25_frq_limit, lim);
}
unsigned int getdb25frqlim()
{
	return readFPGA(R_db25_frq_limit);
}

unsigned int getdb25lowlim()
{
	return readFPGA(R_db25_frq_low_lim);
}

void setap_delay(unsigned int delay)
{
	writeFPGA(R_ap_delay, delay);
}
unsigned int getap_delay()
{
	return readFPGA(R_ap_delay);
}
void disLD()
{
	mask2write(R_ld_enb, 0, 0);
}
void enLD()
{
	mask2write(R_ld_enb, 0, 1);
}
unsigned char getdb25enable()
{
	return mask2read(R_db25_mode, 1);
}
void setdb25mode(unsigned char c)
{
	writeFPGA(R_db25_mode, c);
}
void setDB25enable(unsigned char c)
{
	mask2write(R_db25_mode, 0, c);
}

unsigned char getLatchInt()
{
	return mask2read(R_db25_mode, 1);
}
void setDB25LatchInt(unsigned char c)
{
	mask2write(R_db25_mode, 1, c);
}
unsigned char getLatchPulseLen()
{
	return mask2read(R_db25_mode, 2);
}
void setLatchPulseLen(unsigned char c)
{
	mask2write(R_db25_mode, 2, c);
}
unsigned char getDB25LatchDat()
{
	return readFPGA(R_db25_bufin)&0x00FF;
}
unsigned char getdb25freValLow()
{
	return mask2read(R_db25_frq_limit, 0);
}
unsigned char getdb25freValUp()
{
	return mask2read(R_db25_frq_limit, 1);
}
unsigned char getdb25temAlarm()
{
	return mask2read(R_db25_alarm, 0);
}
void setdb25temAlarm(unsigned char c)
{
	mask2write(R_db25_alarm, 0, c);
}
unsigned char getdb25laserTurnOffAlarm()
{
	return mask2read(R_db25_alarm, 1);
}
void setdb25laserTurnOffAlarm(unsigned char c)
{
	mask2write(R_db25_alarm, 1, c);
}
unsigned char getdb25MOAlarm()
{
	return mask2read(R_db25_alarm, 2);
}
void setdb25MOAlarm(unsigned char c)
{
	mask2write(R_db25_alarm, 2, c);
}
void setEmrgStopInt(unsigned char c)
{
	mask2write(R_int_enb, 0, c);
}
void setSDPulseReadyInt(unsigned char c)
{
	mask2write(R_int_enb, 1, c);
}
void setdb25dataLatchInt(unsigned char c)
{
	mask2write(R_int_enb, 8, c);
}
void setdb25MOswiInt(unsigned char c)
{
	mask2write(R_int_enb, 9, c);
}
void setdb25APswiInt(unsigned char c)
{
	mask2write(R_int_enb, 10, c);
}
void setIntEnb(unsigned int i)
{
	writeFPGA(R_int_enb, i);
}
void resetFPGA()
{
	mask2write(R_resetcmd, 1, 1);
}


void mask2write(unsigned char addr, unsigned char bitnum, unsigned char value)
{
	unsigned int mask = readFPGA(addr);
	if(value == 1)
	{
		if (bitnum == 0)
		{
			mask = mask | 0x01;
		}
		else
		{
			mask = mask | (pow2(bitnum));
		}
	}
	if(value == 0)
	{
		if (bitnum == 0)
		{
			mask = mask & 0xFE;
		}
		else
		{
			mask = mask & 0xFFFF - (pow2(bitnum));
		}
	}
	writeFPGA(addr, mask);
}
unsigned char mask2read(unsigned char addr,unsigned char bitnum)
{
	if (bitnum == 0)
	{
		return readFPGA(addr) & 0x01;
	}
	else
	{
		return (readFPGA(addr) & (pow2(bitnum)))== pow2(bitnum);
	}
}
