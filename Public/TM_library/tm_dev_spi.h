/**
  ************************************* Copyright ******************************   
  *                 (C) Copyright 2021,Wenkic,China, GCU.
  *                            All Rights Reserved
  *                              
  *                     By(Wenkic)
  *                    https://blog.csdn.net/Wekic
  *      
  * FileName     : tm_dev_spi.h   
  * Version      : v1.0     
  * Author       : Wenkic           
  * Date         : 2021-01-15         
  * Description  :    
  * Function List:  
  ******************************************************************************
 */ 
 /********************************End of Head************************************/
#ifndef _tm_dev_spi_H
#define _tm_dev_spi_H

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f10x.h"
#include "stdio.h"

enum en_spi_mode {
	SPI_MODE_0,		// CPOL=0, CPHA=0
	SPI_MODE_1,		// CPOL=0, CPHA=1
	SPI_MODE_2,		// CPOL=1, CPHA=0
	SPI_MODE_3		// CPOL=1, CPHA=1
};

//端口
enum en_spi_port {
	DEV_SPI1_GPIOA,
	DEV_SPI1_GPIOB,
	DEV_SPI2
};

#define DECLARE_SPI_CHANNEL(NAME, OWNER, CHANNEL) \
	struct dev_spi NAME = { \
		.channel = CHANNEL, \
	}

struct dev_spi {
	SPI_TypeDef  * 	spi ;
	GPIO_TypeDef *  port;
	uint16_t		miso;
	uint16_t		mosi;
	uint16_t		sck ;
	uint16_t		nss ;
	DMA_Channel_TypeDef * dma_tx_ch;
	uint32_t		dma_tx_flags;
	uint32_t		dma_tx_iqrn;
	DMA_Channel_TypeDef * dma_rx_ch;
	uint32_t		dma_rx_flags;
	uint32_t		dma_rx_iqrn;
	DMA_InitTypeDef dma_struct;
	SPI_InitTypeDef spi_struct;
};

/* Init functions */
void* dev_spi_init(struct dev_spi * spi, enum en_spi_port port, enum en_spi_mode mode, uint16_t spi_baudrate_prescaller);
void dev_spi_remove(struct dev_spi * spi);

/* Control functions */
void dev_spi_start(struct dev_spi * spi);
void dev_spi_stop(struct dev_spi * spi);
void dev_spi_wait(struct dev_spi * spi);
void dev_spi_set8(struct dev_spi * spi);
void dev_spi_set16(struct dev_spi * spi);

/* 8-bit send/receive functions */
void dev_spi_send8(struct dev_spi * spi, uint8_t * data, size_t data_len);
void dev_spi_sendCircular8(struct dev_spi * spi, uint8_t * data, size_t data_len);
void dev_spi_recv8(struct dev_spi * spi, uint8_t * data, size_t data_len);
void dev_spi_recvCircular8(struct dev_spi * spi, uint8_t * data, size_t data_len);

/* 16-bit functions */
void dev_spi_send16(struct dev_spi * spi, uint16_t *data, size_t data_len);
void dev_spi_sendCircular16(struct dev_spi * spi, uint16_t *data, size_t data_len);
void dev_spi_recv16(struct dev_spi * spi, uint16_t * data, size_t data_len);
void dev_spi_recvCircular16(struct dev_spi * spi, uint8_t * data, size_t data_len);



/* C++ detection */
#ifdef __cplusplus
}
#endif


#endif
/********************************End of File************************************/

