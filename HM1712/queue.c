#include "QUEUE.h"


unsigned int NEXT(unsigned int x,unsigned int i)					//return ((x+i)%MAXLENGTH)
{
	return((x+i)%MAXLENGTH);
}

void NewQueue(Queue *q)
{
	int i;
	for(i=0;i<MAXLENGTH;i++) q->element[i]=0x00;
	q->rear=0;
	q->front=0;				//Initial the storage buffer, an empty Queue
	q->size=0;
}

unsigned char Front(Queue *q)
{
	return(q->element[q->front]);
}

unsigned char Neibour(Queue *q, unsigned int i)
{
	return(q->element[NEXT(q->front,i)]);
}

void Enqueue(Queue *q, unsigned char e)
{
	checkQ(q);
	if(q->size>=MAXLENGTH) return;
	else
	{
		q->element[q->rear]=e;
		q->rear=Next(q->rear);
		q->size++;
	}
}

void Dequeue(Queue *q)
{
	if(q->size!=0)
	{
		q->front=Next(q->front);
		q->size--;
	}
	checkQ(q);
}

char ClearElement(Queue *q, int i)
{
	if(q->size<i) return(0);
	else
	{	q->front=NEXT(q->front,i);
		q->size-=i;
		return(1);
	}
}

char EmptyQ(Queue *q)
{
	return(q->size==0);
}

char FullQ(Queue *q)
{
	return(q->size>=MAXLENGTH);
}

unsigned char SizeQ(Queue *q)
{
	return(q->size);
}

void AddElement(Queue *q, unsigned char e)
{
	checkQ(q);
	if(q->size>=MAXLENGTH) return;
	else
	{
		q->element[q->rear]=e;
		q->rear=Next(q->rear);
		q->size++;
	}
}

unsigned char Trim(Queue *q)
{
	unsigned char dat;
	dat=q->element[q->front];
	q->front=Next(q->front);
	q->size--;
	checkQ(q);
	return(dat);
}
void checkQ(Queue *q)
{
	if((q->size != q->rear - q->front) && (q->size != MAXLENGTH + q->rear - q->front))
	{
		q->rear = q->front;
		q->size = 0;
	}
}
/*
void getStr(Queue* q, char* ch, int len)
{
	int idx;
	for(idx = 0; idx < len; idx++)
	{
		ch[idx] = q->element[q->front + idx];
	}
}

int getInt(Queue* q)
{
	int i = 0;
	while(q->element[q->front + i] <= '0' || q->element[q->front + i] >= '9')
	{
		i++;
	}
	return atoi(q + q->front + i);
}

float getFloat(Queue* q)
{
	int i = 0;
	while(q->element[q->front + i] <= '0' || q->element[q->front + i] >= '9')
	{
		i++;
	}
	return atof(q + q->front + i);
}

*/
