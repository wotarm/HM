#include "motor.h"
#include "api.h"
#include "common_fun.h"
#include "spi_test.h"
char curstat = STOPPED;
sMotorPara scanpara;

void motorInit()
{
	//resetMotor();
	//resetSecMotor();
	//movpos(readPointXfromFlash(), readPointYfromFlash());
	//movSecpos(readPointX2fromFlash(), readPointY2fromFlash());
	//TODO: reset the motor
}

void motorStart()
{
	curstat = DEFAULT;
}


void setStartPoint(unsigned int x, unsigned int y)
{
	writeFPGA(R_SweepX_start, x);
	writeFPGA(R_SweepY_start, y);
}
void setEndPoint(unsigned int x, unsigned int y)
{

	writeFPGA(R_SweepX_stop, x);
	writeFPGA(R_SweepY_stop, y);
}
void setstep(unsigned int c)
{

	writeFPGA(R_SweepX_step, c);
}
/*
void selectMotor(char c)
{
	if(curstat == STOPPED)
	{
		para.motorNum = c;
	}
}

void selectStrategy(char c)
{
	if(curstat == STOPPED)
	{
		para.strategy = c;
	}
}
*/
void motorCtrl()
{
	unsigned int curX = 0;
	unsigned int curY = 0;
	unsigned int tarX = 0;
	unsigned int tarY = 0;
	unsigned int state = 0;

	if(curstat == STOPPED )return;
	getSecpos(&curX, &curY);
	state = getSecState();
	if(curstat == REACHEND)return;
	if(curstat == DEFAULT)
	{
		if(curX != scanpara.startX || curY != scanpara.startY)
		{
			setSecTarX(scanpara.startX);
			setSecTarY(scanpara.startY);
			writeFPGA(R_Motor2STS,0x77);
			curstat = PREPARE;
		}
	}
	if(curX == scanpara.startX && curY == scanpara.startY && curstat ==PREPARE)
		curstat = READY;
	if(curstat<READY)
		return;
	switch(scanpara.strategy)
	{
		case(XCOMBSCAN) :	if((state&0x03) == 0 )
							{
								if(curX == scanpara.startX)
								{
									if(curstat == SCANBACK )
									{
										setSecTarY(curY+scanpara.step);
										secscan();
										curstat = STEPNEXT;
									}
									else if(curstat == STEPNEXT || curstat == READY)
									{
										setSecTarX(scanpara.endX);
										secscan();
									}
									if(curstat == REACHEND)
									{
										curstat = STOPPED;
									}
									return;
								}
								else if(curX == scanpara.endX)
								{
									curstat = SCANBACK;
									if(curY == scanpara.endY)
										curstat = REACHEND;
									setSecTarX(scanpara.startX);
									secscan();
									return;
								}
							}
						break;
		case(XSNAKESCAN)	:	if((state&0x03) == 0 )
							{
								if(curX == scanpara.startX)
								{
									if(curstat == STEPNEXT || curstat == READY)
										setSecTarX(scanpara.endX);
									secscan();
									curstat = SCAN;
									return;
								}
								else if(curX == scanpara.endX)
								{
									if(curY == scanpara.endY)
									{
										curstat = REACHEND;
										return;
									}
									if(curstat == SCAN)
									{
										setSecTarY(curY+scanpara.step);
										curstat = STEPNEXT;
										secscan();
									}
									else
									{
										setSecTarX(scanpara.startX);
										secscan();
									}
									return;
								}
							}
						break;
		case(YCOMBSCAN)	:	if((state&0x03) == 0 )
							{
								if(curY == scanpara.startY)
								{
									if(curstat == SCANBACK)
									{
										setSecTarX(curX+scanpara.step);
										secscan();
										curstat = STEPNEXT;
										return;
									}
									else if(curstat == SCAN || curstat == READY)
									{
										setSecTarY(scanpara.endY);
										secscan();
									}
									if(curstat == REACHEND)
									{
										curstat = STOPPED;
									}
								}
								else if(curY == scanpara.endY)
								{
									setSecTarY(scanpara.startY);
									secscan();
									curstat = SCANBACK;
									if(curX == scanpara.endX)
										curstat = REACHEND;
									return;
								}
							}
						break;
		case(YSNAKESCAN)	:	if((state&0x03) == 0 )
							{
								if(curY == scanpara.startY)
								{
									if(curstat == STEPNEXT || curstat == READY)
										setSecTarY(scanpara.endY);
									secscan();
									curstat = SCAN;
									return;
								}
								else if(curY == scanpara.endY)
								{
									if(curX == scanpara.endX)
									{
										curstat = REACHEND;
										return;
									}
									if(curstat == SCAN)
									{
										setSecTarX(curX+scanpara.step);
										secscan();
										curstat = STEPNEXT; 
									}
									else
									{
										setSecTarY(scanpara.startY);
										secscan();
									}
									return;
								}
							}
						break;
	}
} 

int ifmotorIdel()
{
	return (getState()&0x03) ==0;
}
void movpos(unsigned int x, unsigned int y)
{
	unsigned int buf;
	while(ifmotorIdel() != 1);
	setTarX(x);
	setTarY(y);
	buf = readFPGA(R_MotorSTS);
	buf &= 0xFFFE;
	buf |= 0x06;
	writeFPGA(R_MotorSTS, buf);
}
void movSecpos(unsigned int x, unsigned int y)
{
	unsigned int buf;
	//while(ifmotorIdel() != 1);
	setSecTarX(x);
	setSecTarY(y);
	buf = readFPGA(R_Motor2STS);
	buf &= 0xFFFE;
	buf |= 0x06;
	writeFPGA(R_Motor2STS, buf);
}
void scan(unsigned int sx, unsigned int sy, unsigned int ex, unsigned int ey,unsigned int step,  char strategy)
{
	setStartPoint(sx, sy);
	setEndPoint(ex, ey);
	setstep(step);
	if(strategy == YSNAKESCAN)
	{
		writeFPGA(R_ScanSTS, 0x02);
	}
	if(strategy == YCOMBSCAN)
	{
		writeFPGA(R_ScanSTS, 0x03);
	}
}
void motorStop()
{
	if (ifSweep() == 1)
	{
		stopSweep();
	}
	if(ifScan_move() == 1)
	{
		stopScan_move();
	}
}
void secMotorStop()
{
	writeFPGA(R_Motor2STS, 0x0);
}

char ifSweep()
{
	return mask2read(R_ScanSTS, 2);
}
void stopSweep()
{
	writeFPGA(R_ScanSTS, 0x0);
}
char ifScan_move()
{
	return mask2read(R_MotorSTS, 1);
}
void stopScan_move()
{
	unsigned int buf;
	buf = readFPGA(R_MotorSTS);
	writeFPGA(R_MotorSTS, buf & 0xFC);
}

void getpos(unsigned int* x, unsigned int* y)
{
	*x = readFPGA(R_MotorX_pos);
	*y = readFPGA(R_MotorY_pos);
}
void getSecpos(unsigned int* x, unsigned int* y)
{
	*x = readFPGA(R_MotorX2_pos);
	*y = readFPGA(R_MotorY2_pos);
}

char ifModeLockDataCanRead()
{
	if(ifModeLockValid()==1)
		return 1;
	else
		return 0;
}
void getModeLockdata(unsigned int* x, unsigned int* y)
{
	modeLockPreRead();
	getModeLockPos(x, y);
}

void getModeLockPos(unsigned int* x, unsigned int* y)
{
	*x = readFPGA(R_ModeLockPosX);
	*y = readFPGA(R_ModeLockPosY);
}

void sendCmd(unsigned int cmd)
{
	writeFPGA(R_MotorSTS,cmd);
}

void secscan()
{
	unsigned int j = readFPGA(R_Motor2STS);
	j &= 0xFDFF;
	j |= 0x0700;
	writeFPGA(R_Motor2STS,j);
}


unsigned int getState()
{
	return readFPGA(R_MotorSTS);
}
unsigned int getSecState()
{
	return readFPGA(R_Motor2STS);
}

void resetMotor()
{
	motorStop();
	writeFPGA(R_MotorSTS,0x05);
}
void resetSecMotor()
{
	secMotorStop();
	writeFPGA(R_MotorSTS,0x05);
}
/*
void setTarPos(unsigned int x, unsigned int y)
{
	setTarX(x);
	setTarY(y);
}
*/
void setTarX(unsigned int x)
{
	writeFPGA(R_MotorX_targ ,x);
}

void setTarY(unsigned int y)
{
	writeFPGA(R_MotorY_targ ,y);
}

unsigned int getTarX()
{
	return readFPGA(R_MotorX_targ);
}
unsigned int getTarY()
{
	return readFPGA(R_MotorY_targ);
}

void setSecTarX(unsigned int x)
{
	if(x>scanpara.endX)x = scanpara.endX;
	writeFPGA(R_MotorX2_targ, x);
}
void setSecTarY(unsigned int y)
{
	if(y>scanpara.endY)y = scanpara.endY;
	writeFPGA(R_MotorY2_targ, y);
}

unsigned int getSecTarX()
{
	return readFPGA(R_MotorX2_targ);
}
unsigned int getSecTarY()
{
	return readFPGA(R_MotorY2_targ);
}

char ifModeLock()
{
	return (mask2read(R_hardware_status, 2) == 0x1);
}
char ifModeLockSwitch()
{
	return mask2read(R_int_reg, 0);
}
char ifMoterInt()
{
	return mask2read(R_int_reg, 1);
}
void clearModeLockInt()
{
	mask2write(R_int_reg, 0, 1);
}
void clearMotorInt()
{
	mask2write(R_int_reg, 1, 1);
}
char ifEnModeLockInt()
{
	return mask2read(R_int_en, 0);
}
void enModeLockInt()
{
	mask2write(R_int_en, 0, 1);
}
void disModeLockInt()
{
	mask2write(R_int_en, 0, 0);
}
char ifEnMotorInt()
{
	return mask2read(R_int_en, 1);
}
void enMotorInt()
{
	mask2write(R_int_en, 1, 1);
}
void disMotorInt()
{
	mask2write(R_int_en, 1, 0);
}
void modeLockPreRead()
{
	mask2write(R_MotorSTS, 5, 1);
}
char ifModeLockValid()
{
	return mask2read(R_MotorSTS, 5);
}
char ifMLdataoverflow()
{
	return mask2read(R_MotorSTS, 6);
}

unsigned int getPD_cntr()
{
	return readFPGA(R_pd_cntr);
}
unsigned int getPD_divider()
{
	return readFPGA(R_pd_divider);
}
void setPD_divider(unsigned int divider)
{
	writeFPGA(R_pd_divider, divider);
}
unsigned int getPD_phase()
{
	return readFPGA(R_pd_phase);
}
void setPD_phase(unsigned int phase)
{
	writeFPGA(R_pd_phase, phase);
}
unsigned char getFstPwidth()
{
	return readFPGA(R_pd_phase);
}

void setFstPwidth(unsigned char width)
{
	writeFPGA(R_pwidth1, width);
}
unsigned char getFstPgap()
{
	return readFPGA(R_pwidth1);
}
void setFstPgap(unsigned char width)
{
	writeFPGA(R_pgap1, width);
}
unsigned char getScdPwidth()
{
	return readFPGA(R_pgap1);
}
void setScdPwidth(unsigned char width)
{
	writeFPGA(R_pwidth2, width);
}
unsigned char getScdPgap()
{
	return readFPGA(R_pgap2);
}
void setScdPgap(unsigned char width)
{
	writeFPGA(R_pgap2, width);
}
unsigned char getThdPwidth()
{
	return readFPGA(R_pwidth3);
}
void setThdPwidth(unsigned char width)
{
	writeFPGA(R_pwidth3,width);
}


void setMotorType(char c)
{
	mask2write(R_HwType, 0, c);
}
void setMotorDriver(char c)
{
	mask2write(R_HwType, 1, c);
}
void setEOMfreq(char c)
{
	mask2write(R_HwType, 2, c);
}
void setEOMtype(char c)
{
	mask2write(R_HwType, 3, c);
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