#ifndef SPI_H
#define SPI_H

#include <stdint.h>

void SPI_Init();
void DAC_Write(unsigned int data);
void SPI_transfer(int d);

#endif