
#include <mqx.h>
#include <bsp.h> 
#include <fio.h>
#include "gs_protocol.h"
#include "uart_ctrl.h"
#include "queue.h"
#include "hw_ptr.h"
extern s_HW_ptr HW_ptr;
extern char if_process;
extern char if_comm_process;
extern char if_hmi_process;
Queue RxD;
Queue Comm_RxD;
Queue Hmi_RxD;
void uart_task(uint32_t dummy)
{
	char Byte;
	if(HW_ptr.rs232_dev == NULL)
	{
		uart_init();
	}
	while(1)
	{
		if(0 != read(HW_ptr.rs232_dev,&Byte,1))
		{
			Enqueue(&RxD,Byte);
			if(RxD.element[(RxD.rear +100 -1 )%100] == 0x0a || RxD.element[(RxD.rear +100 -1 )%100] == 0x0d)
			{
				RxD.element[(RxD.rear+100-1)%100] = 0x0d;
				if_process = 1;
				_lwsem_post(&HW_ptr.uart_sem);
				//ProcessMsg(&RxD);
			}
		}
	}
}
void uart_init()
{
	HW_ptr.rs232_dev = fopen("ittya:",NULL);
	if(NULL == HW_ptr.rs232_dev)
	{
		printf ("Error opening uart driver!\n");
    _time_delay (200L);
    _task_block ();
	}
	uint32_t param = 115200;
  ioctl(HW_ptr.rs232_dev ,IO_IOCTL_SERIAL_SET_BAUD, &param);
	param = IO_SERIAL_PARITY_NONE;
  ioctl(HW_ptr.rs232_dev ,IO_IOCTL_SERIAL_SET_PARITY, &param); 
	//param = 8;
  //ioctl(HW_ptr.rs232_dev ,IO_IOCTL_SERIAL_SET_DATA_BITS, &param);
	param = IO_SERIAL_STOP_BITS_1;
  ioctl(HW_ptr.rs232_dev ,IO_IOCTL_SERIAL_SET_STOP_BITS, &param);
	param = IO_SERIAL_PARITY_NONE;
  ioctl(HW_ptr.rs232_dev ,IO_IOCTL_SERIAL_SET_PARITY, &param);
	
	
	
	HW_ptr.comm_dev = fopen("ittye:",NULL);
	if(NULL == HW_ptr.comm_dev)
	{
		printf ("Error opening uart driver!\n");
    _time_delay (200L);
    _task_block ();
	}
	param = 115200;
  ioctl(HW_ptr.comm_dev ,IO_IOCTL_SERIAL_SET_BAUD, &param);
	param = IO_SERIAL_PARITY_NONE;
  ioctl(HW_ptr.comm_dev ,IO_IOCTL_SERIAL_SET_PARITY, &param); 
	//param = 8;
  //ioctl(HW_ptr.rs232_dev ,IO_IOCTL_SERIAL_SET_DATA_BITS, &param);
	param = IO_SERIAL_STOP_BITS_1;
  ioctl(HW_ptr.comm_dev ,IO_IOCTL_SERIAL_SET_STOP_BITS, &param);
	param = IO_SERIAL_PARITY_NONE;
  ioctl(HW_ptr.comm_dev ,IO_IOCTL_SERIAL_SET_PARITY, &param);
	
	
	HW_ptr.hmi_dev = fopen("ittyd:",NULL);
	if(NULL == HW_ptr.hmi_dev)
	{
		printf ("Error opening uart driver!\n");
    _time_delay (200L);
    _task_block ();
	}
	param = 115200;
  ioctl(HW_ptr.hmi_dev ,IO_IOCTL_SERIAL_SET_BAUD, &param);
	param = IO_SERIAL_PARITY_NONE;
  ioctl(HW_ptr.hmi_dev ,IO_IOCTL_SERIAL_SET_PARITY, &param); 
	param = IO_SERIAL_STOP_BITS_1;
  ioctl(HW_ptr.hmi_dev ,IO_IOCTL_SERIAL_SET_STOP_BITS, &param);
}
void uart_comm_send(char* str)
{
	write(HW_ptr.comm_dev, str, strlen(str));
	//fflush(HW_ptr.comm_dev);
}
void uart_comm_rec_task(uint32_t dummy)
{
	char Byte;
	if(HW_ptr.comm_dev == NULL)
	{
		uart_init();
	}
	while(1)
	{
		if(0 != read(HW_ptr.comm_dev,&Byte,1))
		{
			Enqueue(&Comm_RxD,Byte);
			if(Comm_RxD.element[(Comm_RxD.rear +100 -1 )%100] == 0x0a || Comm_RxD.element[(Comm_RxD.rear +100 -1 )%100] == 0x0d)
			{
				Comm_RxD.element[(Comm_RxD.rear+100-1)%100] = 0x0d;
				if_comm_process = 1;
				_lwsem_post(&HW_ptr.uart_sem);
			}
		}
	}
}

void uart_hmi_send(char* str)
{
	write(HW_ptr.hmi_dev, str, strlen(str));
//	fflush(HW_ptr.hmi_dev);
}
void uart_hmi_rec_task(uint32_t dummy)
{
	char Byte;
	if(HW_ptr.hmi_dev == NULL)
	{
		uart_init();
	}
	while(1)
	{
		if(0 != read(HW_ptr.hmi_dev,&Byte,1))
		{
			Enqueue(&Hmi_RxD,Byte);
			if(Hmi_RxD.element[(Hmi_RxD.rear +100 -1 )%100] == 0x0a || Hmi_RxD.element[(Hmi_RxD.rear +100 -1 )%100] == 0x0d)
			{
				Hmi_RxD.element[(Hmi_RxD.rear+100-1)%100] = 0x0d;
				if_hmi_process = 1;
				_lwsem_post(&HW_ptr.uart_sem);
			}
		}
	}
}
