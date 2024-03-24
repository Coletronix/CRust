/*
 * File:        uart.c
 * Purpose:     Provide UART routines for serial IO
 *
 * Notes:        
 *
 */

#include "msp.h"
#include "uart.h"  // you need to create this file with the function prototypes
#include "Common.h"  // from Lab1 code

#define BAUD_RATE 9600      //default baud rate 
extern uint32_t SystemCoreClock;  // clock rate of MCU


void uart0_init() {
    // bit15=0,      no parity bits
    // bit14=x,      not used when parity is disabled
    // bit13=0,      LSB first
    // bit12=0,      8-bit data length
    // bit11=0,      1 stop bit
    // bits10-8=000, asynchronous UART mode
    // bits7-6=11,   clock source to SMCLK
    // bit5=0,       reject erroneous characters and do not set flag
    // bit4=0,       do not set flag for break characters
    // bit3=0,       not dormant
    // bit2=0,       transmit data, not address (not used here)
    // bit1=0,       do not transmit break (not used here)
    // bit0=1,       hold logic in reset state while configuring

    // set CTLW0 - hold logic and configure clock source to SMCLK (set bits 7-6 to 11)
    EUSCI_A0->CTLW0 |= 0xC1;

    // baud rate
    // N = clock/baud rate = clock_speed/BAUD_RATE
    // set BRW register
    EUSCI_A0->BRW = SystemCoreClock/BAUD_RATE;

     // clear first and second modulation stage bit fields
    // MCTLW register;  
    EUSCI_A0->MCTLW &= ~0xFFF0;

    // P1.3 = TxD
    // P1.2 = RxD
    // we will be using P1.2, P1.3 for RX and TX but not in I/O mode, so we set Port1 SEL1=0 and SEL0=1
    // set SEL0, SEL1 appropriately
    P1->SEL0 |= (BIT2 | BIT3);
    P1->SEL1 &= ~(BIT2 | BIT3);


    // CTLW0 register - release from reset state
    EUSCI_A0->CTLW0 &= ~(BIT0);

    // disable interrupts (transmit ready, start received, transmit empty, receive full)    
    // IE register;      
    EUSCI_A0->IE &= ~(BIT3 | BIT2 | BIT1 | BIT0);
}

BYTE uart0_getchar() {
    // Wait for data
    // IFG register
    while (!(EUSCI_A0->IFG & BIT0));

    // read character and store in inChar variable
    // RXBUF register
    //Return the 8-bit data from the receiver 
    return(EUSCI_A0->RXBUF);
}

BYTE uart0_dataAvailable() {
    return (EUSCI_A0->IFG & BIT0);
}

void uart0_putchar(char ch) {
    // Wait until transmission of previous bit is complete 
    // IFG register
    while (!(EUSCI_A0->IFG & BIT1));
    
    // send ch character to uart
    // TXBUF register
    EUSCI_A0->TXBUF = ch;
}

void uart0_put(char *ptr_str) {
    while(*ptr_str != 0)
        uart0_putchar(*ptr_str++);
}

void uart2_init() {
    // bit15=0,      no parity bits
    // bit14=x,      not used when parity is disabled
    // bit13=0,      LSB first
    // bit12=0,      8-bit data length
    // bit11=0,      1 stop bit
    // bits10-8=000, asynchronous UART mode
    // bits7-6=11,   clock source to SMCLK
    // bit5=0,       reject erroneous characters and do not set flag
    // bit4=0,       do not set flag for break characters
    // bit3=0,       not dormant
    // bit2=0,       transmit data, not address (not used here)
    // bit1=0,       do not transmit break (not used here)
    // bit0=1,       hold logic in reset state while configuring

    // set CTLW0 - hold logic and configure clock source to SMCLK (set bits 7-6 to 11)
    EUSCI_A2->CTLW0 |= 0xC1;

    // baud rate
    // N = clock/baud rate = clock_speed/BAUD_RATE
    // set BRW register
    EUSCI_A2->BRW = SystemCoreClock/BAUD_RATE;

     // clear first and second modulation stage bit fields
    // MCTLW register;  
    EUSCI_A2->MCTLW &= ~0xFFF0;

    // P3.3 = TxD
    // P3.2 = RxD
    // we will be using P3.2, P3.3 for RX and TX but not in I/O mode, so we set Port1 SEL1=0 and SEL0=1
    // set SEL0, SEL1 appropriately
    P3->SEL0 |= (BIT2 | BIT3);
    P3->SEL1 &= ~(BIT2 | BIT3);


    // CTLW0 register - release from reset state
    EUSCI_A2->CTLW0 &= ~(BIT0);

    // disable interrupts (transmit ready, start received, transmit empty, receive full)    
    // IE register;      
    EUSCI_A2->IE &= ~(BIT3 | BIT2 | BIT1 | BIT0);
}

BYTE uart2_getchar() {
    // Wait for data
    // IFG register
    while (!(EUSCI_A2->IFG & BIT0));

    // read character and store in inChar variable
    // RXBUF register
    //Return the 8-bit data from the receiver 
    return(EUSCI_A2->RXBUF);
}

BYTE uart2_dataAvailable() {
    return (EUSCI_A2->IFG & BIT0);
}

void uart2_putchar(char ch) {
    // Wait until transmission of previous bit is complete 
    // IFG register
    while (!(EUSCI_A2->IFG & BIT1));
    
    // send ch character to uart
    // TXBUF register
    EUSCI_A2->TXBUF = ch;
}

void uart2_put(char *ptr_str) {
    while(*ptr_str != 0)
        uart2_putchar(*ptr_str++);
}

void uart_init() {
    #ifdef BLE
        uart2_init();
    #else
        uart0_init();
    #endif
}

void uart_putchar(char ch) {
    #ifdef BLE
        uart2_putchar(ch);
    #else
        uart0_putchar(ch);
    #endif
}

void uart_put(char *ptr_str) {
    #ifdef BLE
        uart2_put(ptr_str);
    #else
        uart0_put(ptr_str);
    #endif
}

BYTE uart_getchar() {
    #ifdef BLE
        return uart2_getchar();
    #else
        return uart0_getchar();
    #endif
}

BYTE uart_dataAvailable() {
    #ifdef BLE
        return uart2_dataAvailable();
    #else
        return uart0_dataAvailable();
    #endif
}