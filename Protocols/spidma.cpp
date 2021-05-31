/*===================================================================*/
/*                                                                   */
/*  spidma.cpp : SPI DMA function                                    */
/*                                                                   */
/*  2016/1/20  Racoon                                                */
/*                                                                   */
/*===================================================================*/

#include "mbed.h"
#include "stm32f4xx_hal.h"

SPI_HandleTypeDef  SpiHandle;
HAL_StatusTypeDef st;

/*-------------------------------------------------------------------*/
/*  callback                                                         */
/*-------------------------------------------------------------------*/
extern "C" {
    void HAL_SPI_MspInit(__SPI_HandleTypeDef *hspi)
    {
        static DMA_HandleTypeDef  hdma_tx;

        GPIO_InitTypeDef GPIO_InitStruct;
      
        __HAL_RCC_GPIOA_CLK_ENABLE();
        __HAL_RCC_SPI1_CLK_ENABLE();
        __HAL_RCC_DMA2_CLK_ENABLE();

        GPIO_InitStruct.Pin       = GPIO_PIN_5;
        GPIO_InitStruct.Mode      = GPIO_MODE_AF_PP;
        GPIO_InitStruct.Pull      = GPIO_PULLUP;
        GPIO_InitStruct.Speed     = GPIO_SPEED_FREQ_HIGH;
        GPIO_InitStruct.Alternate = GPIO_AF5_SPI1;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_6;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        GPIO_InitStruct.Pin = GPIO_PIN_7;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

        // hdma_tx.Instance                 = DMA2_Stream3;
        // hdma_tx.Init.Channel             = DMA_CHANNEL_3;
        // hdma_tx.Init.Direction           = DMA_MEMORY_TO_PERIPH;
        // hdma_tx.Init.PeriphInc           = DMA_PINC_DISABLE;
        // hdma_tx.Init.MemInc              = DMA_MINC_ENABLE;
        // hdma_tx.Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
        // hdma_tx.Init.MemDataAlignment    = DMA_MDATAALIGN_BYTE;
        // hdma_tx.Init.Mode                = DMA_NORMAL;
        // hdma_tx.Init.Priority            = DMA_PRIORITY_HIGH;
        // hdma_tx.Init.FIFOMode            = DMA_FIFOMODE_DISABLE;
        // hdma_tx.Init.FIFOThreshold       = DMA_FIFO_THRESHOLD_FULL;
        // hdma_tx.Init.MemBurst            = DMA_MBURST_SINGLE;
        // hdma_tx.Init.PeriphBurst         = DMA_MBURST_SINGLE;

        // st = HAL_DMA_Init(&hdma_tx);

        // __HAL_LINKDMA(&SpiHandle, hdmatx, hdma_tx); // SpiHandle->hdmatx = hdma_tx

        // HAL_NVIC_SetPriority(DMA2_Stream3_IRQn, 0, 1);
        // HAL_NVIC_EnableIRQ(DMA2_Stream3_IRQn);
        // debug("My HAL_SPI_MspInit st=%d\n", st);
    }

    void DMA2_Stream3_IRQHandler(void)
    {
        HAL_DMA_IRQHandler(SpiHandle.hdmatx);
    }
} // extern "C"

/*-------------------------------------------------------------------*/
/*  Write a byte data                                                */
/*-------------------------------------------------------------------*/
void spi_write(uint8_t data)
{
    HAL_SPI_Transmit(&SpiHandle, &data, 1, 100);
}

/*-------------------------------------------------------------------*/
/*  Write a word data                                                */
/*-------------------------------------------------------------------*/
void spi_writew(uint16_t data)
{
    HAL_SPI_Transmit(&SpiHandle, (uint8_t *)&data, 2, 100);
}

/*-------------------------------------------------------------------*/
/*  Write a byte data                                                */
/*-------------------------------------------------------------------*/
void spi_write(uint8_t* pData, uint16_t size)
{
    HAL_SPI_Transmit(&SpiHandle, pData, size, 100);
}


/*-------------------------------------------------------------------*/
/*  Initialize SPI DMA                                               */
/*-------------------------------------------------------------------*/
void spi_init()
{
    SpiHandle.Instance               = SPI1;
    SpiHandle.Init.Mode              = SPI_MODE_MASTER;
    SpiHandle.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
    SpiHandle.Init.Direction         = SPI_DIRECTION_2LINES;
    SpiHandle.Init.CLKPhase          = SPI_PHASE_1EDGE;
    SpiHandle.Init.CLKPolarity       = SPI_POLARITY_HIGH;
    SpiHandle.Init.CRCCalculation    = SPI_CRCCALCULATION_DISABLE;
    SpiHandle.Init.CRCPolynomial     = 7;
	//!ToDo correct here to avoid manipulate with bytes when send 16 bit RGB value for the Pixel
    SpiHandle.Init.DataSize          = SPI_DATASIZE_8BIT;
//    SpiHandle.Init.DataSize          = SPI_DATASIZE_16BIT;
    SpiHandle.Init.FirstBit          = SPI_FIRSTBIT_MSB;
    SpiHandle.Init.NSS               = SPI_NSS_SOFT;
    SpiHandle.Init.TIMode            = SPI_TIMODE_DISABLE;

    if (HAL_SPI_Init(&SpiHandle) != HAL_OK)
    {
        while(1){
            debug("Can't init HAL SPI\n");
            ThisThread::sleep_for(3000ms);
        }
    }
    else{
        debug("HAL SPI initilised!\n");
    }
}

