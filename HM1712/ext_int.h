#ifndef FPGA_INT_H
#define FPGA_INT_H
#include <mqx.h>
#include <bsp.h>
extern void int_task(uint32_t);
extern void handle_Extint_task(uint32_t dummy);

#endif
