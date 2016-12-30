/*--------------------------------------------------------------------------
QUEUE.H

Header file for definition and function of the customized Circular Queue applied in Project GRAPE microcontroller.
Copyright (c) 2011 GS Lasers Technology (Shanghai) Co., Ltd
All rights reserved.
--------------------------------------------------------------------------*/
#ifndef __QUEUE_H__
#define __QUEUE_H__

#include <stdlib.h>
/*------------------------------------------------
	Definition of Structure Queue and relative Operation
	Used for Storage buffer of UART-received data and Protocol process
------------------------------------------------*/
/*Define structure of Circular Queue*/
#define MAXLENGTH 100		//store 20 bytes at most extern
#define Next(x) (x+1)%MAXLENGTH
struct CQueue				//Use a circular array as Queue
{	unsigned int front;
	unsigned int rear;
	unsigned int size;
	unsigned char element[MAXLENGTH];
};
typedef struct CQueue Queue;

/*------------------------------------------------
	General operation on CQueue	
------------------------------------------------*/	
void NewQueue(Queue *q);						//New and initialize a queue
unsigned char Front(Queue *q);					//Return the first element e of Queue Q, and remain it
unsigned char Trim(Queue *q);					//Return the first element e of Queue Q, and remove it then
void Enqueue(Queue *q, unsigned char e);		//Add element e to the trail of Queue Q
void AddElement(Queue *q, unsigned char e);		//Add element e to the trail of Queue Q
void Dequeue(Queue *q);							//Delete the first element e of Queue Q
unsigned int NEXT(unsigned int x,unsigned int i);	//return ((x+i)%MAXLENGTH)
char ClearElement(Queue *q, int i);
char EmptyQ(Queue *q);
char FullQ(Queue *q);
unsigned char SizeQ(Queue *q);
unsigned char Neibour(Queue *q, unsigned int i);
void checkQ(Queue *q);

#endif
