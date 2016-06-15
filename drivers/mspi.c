#include "bsp.h"

#include "freertos.h"
#include "task.h"
#include "semphr.h"

#include "types.h"
#include "io.h"
#include "irqprio.h"
#include "mspi.h"

#define SPIx							SPI3
#define SPIx_DR_Base					(&(SPI3->DR))
#define SPIx_CLK						RCC_APB1Periph_SPI3
#define SPIx_CLK_CMD(a,b)				RCC_APB1PeriphClockCmd(a,b)
#define SPIx_IRQHandler					SPI3_IRQHandler
#define SPIx_IRQn						SPI3_IRQn
#define SPIx_IRQ_PRIO					SPI3_IRQ_PRIO

#define SPI_MASTER_DMA					DMA2
#define SPIx_DMA_CLK					RCC_AHBPeriph_DMA2

#define SPIx_Rx_DMA_Channel				DMA2_Channel1
#define SPIx_Rx_DMA_IRQn				DMA2_Channel1_IRQn
#define SPIx_Rx_DMA_IRQHandler			DMA2_Channel1_IRQHandler
#define SPIx_Rx_DMA_Flag				DMA2_FLAG_TC1
#define SPIx_Rx_DMA_CLR_Pendings		DMA2_IT_GL1
#define SPIx_Rx_DMA_IRQ_PRIO			DMA2_Channel1_IRQ_PRIO

#define SPIx_Tx_DMA_Channel				DMA2_Channel2
#define SPIx_Tx_DMA_IRQn				DMA2_Channel2_IRQn
#define SPIx_Tx_DMA_IRQHandler			DMA2_Channel2_IRQHandler
#define SPIx_Tx_DMA_Flag				DMA2_FLAG_TC2
#define SPIx_Tx_DMA_CLR_Pendings		DMA2_IT_GL2
#define SPIx_Tx_DMA_IRQ_PRIO			DMA2_Channel2_IRQ_PRIO

#define sema_clear(a)					while (xSemaphoreTake(a,0))

static const SPI_InitTypeDef SPI_InitStructure[2] = {
	{ // LCD
		.SPI_Direction = SPI_Direction_2Lines_FullDuplex,
		.SPI_Mode = SPI_Mode_Master,
		.SPI_DataSize = SPI_DataSize_8b,
		.SPI_CPOL = SPI_CPOL_High,
		.SPI_CPHA = SPI_CPHA_2Edge,
		.SPI_NSS = SPI_NSS_Soft,
		.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32,//era 32
		.SPI_FirstBit = SPI_FirstBit_MSB,
		.SPI_CRCPolynomial = 7
	},
	{ // DF
		.SPI_Direction = SPI_Direction_2Lines_FullDuplex,
		.SPI_Mode = SPI_Mode_Master,
		.SPI_DataSize = SPI_DataSize_8b,
		.SPI_CPOL = SPI_CPOL_Low,
		.SPI_CPHA = SPI_CPHA_1Edge,
		.SPI_NSS = SPI_NSS_Soft,
		.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2,
		.SPI_FirstBit = SPI_FirstBit_MSB,
		.SPI_CRCPolynomial = 7
	}
};

static uint8_t spisetup;
static volatile uint32_t dummy;
static DMA_InitTypeDef DMA_InitStructure;
static volatile uint8_t spi_rxdata;
static xSemaphoreHandle xSemaSpi,xMutexSpi;

/**
  * @brief  This function handles DMA1 Channel 6 interrupt request.
  * @param  None
  * @retval None
  */
void SPIx_Rx_DMA_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Test on Rx DMA Channel Transfer Complete interrupt */
	if(DMA_GetITStatus(SPIx_Rx_DMA_Flag))
	{
		xSemaphoreGiveFromISR(xSemaSpi,&xHigherPriorityTaskWoken);
		/* Clear DMA Channel Half Transfer, Transfer Complete and Global interrupt pending bits */
		DMA_ClearITPendingBit(SPIx_Rx_DMA_CLR_Pendings);
		DMA_ITConfig(SPIx_Rx_DMA_Channel, DMA_IT_TC, DISABLE);
		SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, DISABLE);
		DMA_Cmd(SPIx_Rx_DMA_Channel, DISABLE);
	}
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void SPIx_Tx_DMA_IRQHandler(void)
{
	/* Test on Tx DMA Channel Transfer Complete interrupt */
	if (DMA_GetITStatus(SPIx_Tx_DMA_Flag))
	{
		/* Clear DMA Channel Half Transfer, Transfer Complete and Global interrupt pending bits */
		DMA_ClearITPendingBit(SPIx_Tx_DMA_CLR_Pendings);
		DMA_ITConfig(SPIx_Tx_DMA_Channel, DMA_IT_TC, DISABLE);
		SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, DISABLE);
		DMA_Cmd(SPIx_Tx_DMA_Channel, DISABLE);
	}
}

void SPIx_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	/* Store SPI_SLAVE received data */
	spi_rxdata = SPI_I2S_ReceiveData(SPIx);
	SPI_I2S_ITConfig(SPIx,SPI_I2S_IT_RXNE,DISABLE);
	xSemaphoreGiveFromISR(xSemaSpi,&xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void MSPI_Init(void)
{
	if (xSemaSpi)
		return;
	NVIC_DisableIRQ(SPIx_Rx_DMA_IRQn);
	//NVIC_DisableIRQ(SPIx_Tx_DMA_IRQn);
	NVIC_DisableIRQ(SPIx_IRQn);

	vSemaphoreCreateBinary(xSemaSpi);
	xSemaphoreTake(xSemaSpi,0);		// clear it

	xMutexSpi = xSemaphoreCreateMutex();

	SPI_I2S_DeInit(SPIx);

	/* Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(SPIx_DMA_CLK, ENABLE);

	/* SPIx Periph clock enable */
	SPIx_CLK_CMD(SPIx_CLK, ENABLE);

	spisetup = 0xff;
	MSPI_Lock(SPISETUP_Dataflash);
	MSPI_Unlock();

	/* SPIx_Rx_DMA_Channel configuration ---------------------------------*/
	DMA_DeInit(SPIx_Rx_DMA_Channel);
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPIx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&dummy;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = 1;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
	DMA_Init(SPIx_Rx_DMA_Channel, &DMA_InitStructure);

	/* SPIx_Tx_DMA_Channel configuration ---------------------------------*/
	DMA_DeInit(SPIx_Tx_DMA_Channel);  
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPIx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&dummy;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_Init(SPIx_Tx_DMA_Channel, &DMA_InitStructure);

	NVIC_SetPriority(SPIx_Rx_DMA_IRQn,SPIx_Rx_DMA_IRQ_PRIO);
	//NVIC_SetPriority(SPIx_Tx_DMA_IRQn,SPIx_Tx_DMA_IRQ_PRIO);
	NVIC_SetPriority(SPIx_IRQn,SPIx_IRQ_PRIO);

	SPI_I2S_ITConfig(SPIx,SPI_I2S_IT_RXNE,DISABLE);

	/* Enable RX DMA Channel Transfer Complete interrupt */
	DMA_ITConfig(SPIx_Rx_DMA_Channel, DMA_IT_TC, DISABLE);
	//DMA_ITConfig(SPIx_Tx_DMA_Channel, DMA_IT_TC, DISABLE);

	// Enable IRQ
	NVIC_EnableIRQ(SPIx_IRQn);
	NVIC_EnableIRQ(SPIx_Rx_DMA_IRQn);
	//NVIC_EnableIRQ(SPIx_Tx_DMA_IRQn);

	/* SPIx enable */
	SPI_Cmd(SPIx, ENABLE);
}

void MSPI_Write_Dma(const uint8_t *txb,uint8_t *rxb,uint16_t size,uint8_t incopt)
{
	dummy = 0;
	if (!txb) txb = (uint8_t *)&dummy;
	if (!rxb) rxb = (uint8_t *)&dummy;
	/* SPIx_Rx_DMA_Channel configuration ---------------------------------*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPIx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)rxb;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = (incopt & SPIDMAINC_RxInc)? DMA_MemoryInc_Enable: DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(SPIx_Rx_DMA_Channel, &DMA_InitStructure);

	/* SPIx_Tx_DMA_Channel configuration ---------------------------------*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPIx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)txb;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_MemoryInc = (incopt & SPIDMAINC_TxInc)? DMA_MemoryInc_Enable: DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
	DMA_Init(SPIx_Tx_DMA_Channel, &DMA_InitStructure);

	DMA_ClearITPendingBit(SPIx_Tx_DMA_CLR_Pendings);
	DMA_ClearITPendingBit(SPIx_Rx_DMA_CLR_Pendings);

	sema_clear(xSemaSpi);

	/* Enable Rx DMA Channel Transfer Complete interrupt */
	DMA_ITConfig(SPIx_Rx_DMA_Channel, DMA_IT_TC, ENABLE);

	/* Enable Tx DMA Channel Transfer Complete interrupt */
	// DMA_ITConfig(SPIx_Tx_DMA_Channel, DMA_IT_TC, ENABLE);

	/* Enable SPI DMA Rx request */
	SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Rx, ENABLE);

	/* Enable SPI DMA Tx request */
	SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, ENABLE);

	/* Enable DMA Rx Channel transfer */
	DMA_Cmd(SPIx_Rx_DMA_Channel, ENABLE);

	/* Enable DMA Tx Channel transfer */
	DMA_Cmd(SPIx_Tx_DMA_Channel, ENABLE);

	xSemaphoreTake(xSemaSpi,portMAX_DELAY);
	
	SPI_I2S_DMACmd(SPIx, SPI_I2S_DMAReq_Tx, DISABLE);
	DMA_Cmd(SPIx_Tx_DMA_Channel, DISABLE);
}

uint8_t MSPI_Write(uint8_t data)
{
	SPI_I2S_ClearITPendingBit(SPIx, SPI_I2S_IT_RXNE);
	sema_clear(xSemaSpi);
	SPI_I2S_ITConfig(SPIx,SPI_I2S_IT_RXNE,ENABLE);
	SPI_I2S_SendData(SPIx, data);
	xSemaphoreTake(xSemaSpi,portMAX_DELAY);
	return spi_rxdata;
}

void MSPI_Lock(uint8_t setup)
{
	xSemaphoreTake(xMutexSpi,portMAX_DELAY);
	if (setup != spisetup)
	{
		spisetup = setup;

		SPI_Cmd(SPIx, DISABLE);

		/* SPIx Config */
		SPI_Init(SPIx, (SPI_InitTypeDef *)SPI_InitStructure+spisetup);

		SPI_Cmd(SPIx, ENABLE);
	
		GPIO_PinRemapConfig(GPIO_Remap_SPI3,(spisetup == SPISETUP_Display)? ENABLE: DISABLE);	// PC10-SCK PC11-MISO PC12-MOSI
	}
}

void MSPI_Unlock(void)
{
	xSemaphoreGive(xMutexSpi);
}
