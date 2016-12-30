#ifndef _UART_TEST_H_
#define _UART_TEST_H_


#include <mqx.h>
#include <bsp.h> 
#include <fio.h>
extern void uart_task(uint32_t dummy);
void uart_comm_send(char* str);
extern void uart_comm_rec_task(uint32_t dummy);
void uart_hmi_send(char* str);
extern void uart_hmi_rec_task(uint32_t dummy);
void uart_init(void);
#endif
