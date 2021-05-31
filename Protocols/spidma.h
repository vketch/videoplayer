/*===================================================================*/
/*                                                                   */
/*  spidma.h : SPI DMA function header                               */
/*                                                                   */
/*  2016/1/20  Racoon                                                */
/*                                                                   */
/*===================================================================*/

#ifndef SPIDMA_H_INCLUDED
#define SPIDMA_H_INCLUDED

#include "stm32f4xx_hal_spi.h"

void spi_init();
void spi_write(uint8_t data);
void spi_writew(uint16_t data);
void spi_write(uint8_t* pData, uint16_t size);

extern SPI_HandleTypeDef SpiHandle;

#endif

