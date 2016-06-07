#include "bsp.h"

#include "freertos.h"
#include "task.h"
#include "semphr.h"

#include "types.h"
#include "io.h"
#include "irqprio.h"
#include "ssd1322.h"
#include "display_interface.h"

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

#define NOP								0xe3

static DMA_InitTypeDef DMA_InitStructure;
static volatile uint8_t dummy;
static xSemaphoreHandle xSemaSpi;

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

	xSemaphoreGiveFromISR(xSemaSpi,&xHigherPriorityTaskWoken);
	/* Store SPI_SLAVE received data */
	dummy = SPI_I2S_ReceiveData(SPIx);
	SPI_I2S_ITConfig(SPIx,SPI_I2S_IT_RXNE,DISABLE);
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void LCD_Init_Spi(void)
{
SPI_InitTypeDef   SPI_InitStructure;

	NVIC_DisableIRQ(SPIx_Rx_DMA_IRQn);
	//NVIC_DisableIRQ(SPIx_Tx_DMA_IRQn);
	NVIC_DisableIRQ(SPIx_IRQn);

	if (!xSemaSpi)
		vSemaphoreCreateBinary(xSemaSpi);
	xSemaphoreTake(xSemaSpi,0);		// clear it

	SPI_I2S_DeInit(SPIx);

	/* Periph clock enable */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);
	RCC_AHBPeriphClockCmd(SPIx_DMA_CLK, ENABLE);

	/* SPIx Periph clock enable */
	SPIx_CLK_CMD(SPIx_CLK, ENABLE);

    GPIO_PinRemapConfig(GPIO_Remap_SPI3, ENABLE);	// PC10-SCK PC11-MISO PC12-MOSI

	GpioInit(DISP_SCK_PORT,DISP_SCK_PIN,GPIO_Mode_AF_PP,0);
	//GpioInit(DISP_MISO_PORT,DISP_MISO_PIN,GPIO_Mode_AF_PP,0);
	GpioInit(DISP_MOSI_PORT,DISP_MOSI_PIN,GPIO_Mode_AF_PP,0);

	GpioInit(DISP_CSn_PORT,DISP_CSn_PIN,GPIO_Mode_Out_PP,1);		// unselected
	GpioInit(DISP_CMDn_PORT,DISP_CMDn_PIN,GPIO_Mode_Out_PP,1);
	GpioInit(DISP_RSTn_PORT,DISP_RSTn_PIN,GPIO_Mode_Out_PP,0);		// chip reset
	GpioInit(DISP_VCC_ON_PORT,DISP_VCC_ON_PIN,GPIO_Mode_Out_PP,0);	// Display vcc

	/* SPIx Config */
	SPI_StructInit(&SPI_InitStructure);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_32;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPIx, &SPI_InitStructure);

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

static void LCD_Write_Dma(const uint8_t *pattern,uint16_t size,uint16_t inc)
{
	/* SPIx_Rx_DMA_Channel configuration ---------------------------------*/
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)&dummy;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
	DMA_InitStructure.DMA_BufferSize = size;
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Disable;
	DMA_InitStructure.DMA_Priority = DMA_Priority_High;
	DMA_Init(SPIx_Rx_DMA_Channel, &DMA_InitStructure);

	/* SPIx_Tx_DMA_Channel configuration ---------------------------------*/
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)SPIx_DR_Base;
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)pattern;
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
	DMA_InitStructure.DMA_MemoryInc = (inc)? DMA_MemoryInc_Enable: DMA_MemoryInc_Disable;
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

static void LCD_Write_Spi(uint8_t data)
{
	SPI_I2S_ClearITPendingBit(SPIx, SPI_I2S_IT_RXNE);
	sema_clear(xSemaSpi);
	SPI_I2S_ITConfig(SPIx,SPI_I2S_IT_RXNE,ENABLE);
	SPI_I2S_SendData(SPIx, data);
	xSemaphoreTake(xSemaSpi,portMAX_DELAY);
}

void LCD_Send_Command_Bytes(const uint8_t *d,uint16_t len)
{
	CHIP_SELECT();
	COMMAND_SELECT();
	if (len == 1)
		LCD_Write_Spi(*d);
	else if (len > 1)		
		LCD_Write_Dma(d,len,TRUE);
	//LCD_Write_Spi(NOP);
	CHIP_UNSELECT();
}

void LCD_Send_Command(uint8_t cmd,uint8_t data)
{
	LCD_Send_Reg(cmd);
	LCD_Send_Data(&data,1,TRUE);
}

void LCD_Send_Reg(uint8_t r)
{
	CHIP_SELECT();
	COMMAND_SELECT();
	LCD_Write_Spi(r);
	//LCD_Write_Spi(NOP);
	CHIP_UNSELECT();
}

void LCD_Send_Data(const uint8_t *d,uint16_t c,uint8_t inc)
{
	CHIP_SELECT();
	DATA_SELECT();
	if (c == 1)
		LCD_Write_Spi(*d);
	else if (c > 1)
		LCD_Write_Dma(d,c,inc);
	//COMMAND_SELECT();
	//LCD_Write_Spi(NOP);
	CHIP_UNSELECT();
}
