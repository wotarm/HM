#ifndef _PUBLICHEAD_H__
#define _PUBLICHEAD_H__

//define 1:30W 0:1W
#define VersionHPower 1

#if	VersionHPower
#define SystemMaxCurr	20.0f
#define SystemMaxP		60.0f
#define LOWTEMLD			20.00f
#define HIGHTEMLD			70.00f
#define E_UMI					10.0f
#define E_ES					45
#define E_LV					15.0f
#else
#define SystemMaxCurr	1.2f
#define SystemMaxP		1.0f
#define LOWTEMLD			13.93f
#define HIGHTEMLD			38.76f
#define E_UMI					1.2f
#define E_ES					80
#define E_LV					10.f
#endif
#define	E_KA					1
#define E_KB					0
#define E_AUTO				0
#define E_TEM					1
#define E_LAN					1
#endif
