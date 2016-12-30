#ifndef _MOTOR_H_
#define _MOTOR_H_

#include "parameter.h"
//#include "FPGA.h"
#include "EEPROM.h"

struct motorPara
{
	unsigned int startX;
	unsigned int startY;
	unsigned int endX;
	unsigned int endY;
	unsigned int step;
	char motorNum;
	char strategy;    //  x comb scan, x S-shape scan, y comb scan, y S-shape scan
};
typedef struct motorPara sMotorPara;



#define XCOMBSCAN	0
#define XSNAKESCAN	1
#define YCOMBSCAN	2
#define YSNAKESCAN	3

#define DEFAULT		0
#define PREPARE		1
#define READY			2
#define SCAN			3
#define SCANBACK	4
#define REACHEND	5
#define STEPNEXT	6
#define STOPPED		7



void motorInit(void);

void motorStart(void);

void setStartPoint(unsigned int x, unsigned int y);
void setEndPoint(unsigned int x, unsigned int y);
void setstep(unsigned int c);
void movpos(unsigned int x, unsigned int y);
void movSecpos(unsigned int x, unsigned int y);

void scan(unsigned int sx, unsigned int sy, unsigned int ex, unsigned int ey,unsigned int step,  char strategy);
void motorStop(void);
void secMotorStop(void);
char ifSweep(void);
void stopSweep(void);
char ifScan_move(void);

void motorCtrl(void);

void stopScan_move(void);

char ifModeLockDataCanRead(void);
void getModeLockdata(unsigned int* x, unsigned int* y);
void getModeLockPos(unsigned int* x, unsigned int* y);

void getpos(unsigned int* x, unsigned int* y);

void sendCmd(unsigned int cmd);

unsigned int getState(void);

void resetMotor(void);
void resetSecMotor(void);

void setTarPos(unsigned int x, unsigned int y);

void setTarX(unsigned int x);

void setTarY(unsigned int y);

void setSecTarX(unsigned int x);
void setSecTarY(unsigned int y);
void getSecpos(unsigned int* x, unsigned int* y);

char ifModeLock(void);

char ifHealthy(void);

char ifModeLockSwitch(void);

char ifMoterInt(void);


char ifMoterInt(void);
void clearModeLockInt(void);
void clearMotorInt(void);
char ifEnModeLockInt(void);
void enModeLockInt(void);
void disModeLockInt(void);
char ifEnMotorInt(void);
void enMotorInt(void);
void disMotorInt(void);
void modeLockPreRead(void);
char ifModeLockValid(void);
char ifMLdataoverflow(void);


void secscan(void);
unsigned int getSecState(void);

void clearModeLockInt(void);
void clearMotorInt(void);

unsigned int getPD_cntr(void);
unsigned int getPD_divider(void);
void setPD_divider(unsigned int divider);

unsigned int getPD_phase(void);
void setPD_phase(unsigned int phase);

unsigned char getFstPwidth(void);
void setFstPwidth(unsigned char width);
unsigned char getFstPgap(void);
void setFstPgap(unsigned char width);
unsigned char getScdPwidth(void);
void setScdPwidth(unsigned char width);
unsigned char getScdPgap(void);
void setScdPgap(unsigned char width);
unsigned char getThdPwidth(void);
void setThdPwidth(unsigned char width);


unsigned int getTarX(void);
unsigned int getTarY(void);
unsigned int getSecTarX(void);
unsigned int getSecTarY(void);

void modeLockPreRead(void);

void setMotorType(char c);
void setMotorDriver(char c);
void setEOMfreq(char c);
void setEOMtype(char c);
void mask2write(unsigned char addr, unsigned char bitnum, unsigned char value);
unsigned char mask2read(unsigned char addr,unsigned char bitnum);

extern sMotorPara scanpara;

#endif 
