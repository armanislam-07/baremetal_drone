#ifndef IO_H
#define IO_H
#include "registers.h"
void gpio_config(GPIO_T *port, unsigned pin, unsigned mode, unsigned cnf);
void io_init(void);
#endif
