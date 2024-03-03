#ifndef _UART_H
#define _UART_H

#include "Common.h"

void uart0_init();

BYTE uart0_getchar();

BYTE uart0_dataAvailable();

void uart0_putchar(char ch);

void uart0_put(char *ptr_str);

void uart2_init();

BYTE uart2_getchar();

BYTE uart2_dataAvailable();

void uart2_putchar(char ch);

void uart2_put(char *ptr_str);

BYTE uart_getchar();

BYTE uart_dataAvailable();

#endif
