#ifndef FPGA_INT__H
#define FPGA_INT__H
#include <mqx.h>
#include <bsp.h>
extern void int_task(uint32_t);
extern void handle_Fpgaint_task(uint32_t dummy);

#endif
