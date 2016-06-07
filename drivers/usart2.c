/************************************************************
*                                                           *
*                       usart2.c                            *
*                                                           *
*************************************************************
*                                                           *
*        C file for custom usart communication              *
*                                                           *
*************************************************************
* 07.11.2008                                                *
************************************************************/

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include "FreeRTOS.h"
#include "queue.h"
#include "semphr.h"

#include "bsp.h"

#include "irqprio.h"
#include "usart2.h"

#define QueueLength						64

#define RxQueueSize						QueueLength
#define TxQueueSize						(QueueLength*2)

/* Misc defines. */
#define serINVALID_QUEUE				( ( xQueueHandle ) 0 )
#define serNO_BLOCK						( ( portTickType ) 0 )
#define serTX_BLOCK_TIME				( 40 / portTICK_RATE_MS )

/* The queue used to hold received characters. */
static xQueueHandle xRxQueue;
static xQueueHandle xTxQueue;

#define PRNTFBUF_SIZE		128
static char *prntfBuf;
static xSemaphoreHandle xSemaPrintf;

void USART2_IRQHandler(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint8_t active_U2;
static uint8_t mask_U2;
static uint8_t remap_U2;

/* Private function ----------------------------------------------------------*/
/*******************************************************************************
* Function Name  : USART2_IRQHandler
* Description    : This function handles U2 custom usart interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART2_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
uint8_t cChar;

	if(USART_GetITStatus(USART2, USART_IT_TXE) != RESET)
	{   
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */
		if( xQueueReceiveFromISR( xTxQueue, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_SendData( USART2, cChar );
		}
		else
		{
			USART_ITConfig( USART2, USART_IT_TXE, DISABLE );		
		}		
	}

	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  
	{
		// receive data from host
		cChar = USART_ReceiveData( USART2 );
		xQueueSendFromISR( xRxQueue, &cChar, &xHigherPriorityTaskWoken );
		USART_ClearITPendingBit(USART2,USART_IT_RXNE);
		// Note: receive interrupt is always on!.
	}
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void U2_Remap(uint8_t enable)
{
	remap_U2 = enable;
}

/********************************************************************
*                                                                   *
*                           U2_Open                                 *
*                                                                   *
*********************************************************************
 INPUT  :   uint32_t speed : baudrate
            u  ft    : frame type    see FRAME_TYPE_enum
            uint16_t  pt    : parity type   see PARITY_TYPE_enum
*********************************************************************
*                                                                   *
* Open the serial port for DTE comunications                        *
*                                                                   *
********************************************************************/
void U2_Open(uint32_t speed,uint16_t parity,uint16_t stopbits) 
{
USART_InitTypeDef USART_InitStructure;

	U2_Close() ;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

    /* Enable/Disable the USART2 Pins Software Remapping */
    GPIO_PinRemapConfig(GPIO_Remap_USART2, (remap_U2)? ENABLE: DISABLE);

	switch(stopbits)
	{
	case UARTSB_1:
		stopbits = USART_StopBits_1;
		break;
	case UARTSB_2:
		stopbits = USART_StopBits_2;
		break;
	default:
		return;
	}

	switch(parity)
	{
	case UARTP_None:
		mask_U2 = 0xff;
		parity = USART_Parity_No;
		break;
	case UARTP_Even:
		mask_U2 = 0x7f;
		parity = USART_Parity_Even;
		break;
	case UARTP_Odd:
		mask_U2 = 0x7f;
		parity = USART_Parity_Odd;
		break;
	default:
		return;
	}

	//set Baud Rate  
	USART_InitStructure.USART_BaudRate = speed ;

	// USART2 parameters overtake configuration ----------------------------
	/* USART2 configured as follow:
		- BaudRate = 115200 baud  
		- Word Length = 8 Bits
		- One Stop Bit
		- No parity
		- Hardware flow control enabled (RTS and CTS signals)
		- Receive and transmit enabled
	*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = parity ;
	USART_InitStructure.USART_StopBits = stopbits;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	
	// configure the USART2
	USART_Init(USART2, &USART_InitStructure);

	if (remap_U2)
	{
		/* Configure USART2 Rx (PD.6) as input floating */
		GpioInit(GPIOD,GPIO_Pin_6,GPIO_Mode_IN_FLOATING,0);
		
		/* Configure USART2 Tx (PD.5) as alternate function push-pull */
		GpioInit(GPIOD,GPIO_Pin_5,GPIO_Mode_AF_PP,0);
	}
	else
	{
		/* Configure USART2 Rx (PA.3) as input floating */
		GpioInit(GPIOA,GPIO_Pin_3,GPIO_Mode_IN_FLOATING,0);
		
		/* Configure USART2 Tx (PA.2) as alternate function push-pull */
		GpioInit(GPIOA,GPIO_Pin_2,GPIO_Mode_AF_PP,0);
	}

	if (!xRxQueue)
		xRxQueue = xQueueCreate( RxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!xTxQueue)
		xTxQueue = xQueueCreate( TxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!prntfBuf)
		prntfBuf = pvPortMalloc(PRNTFBUF_SIZE);
	if (!xSemaPrintf)
		xSemaPrintf = xSemaphoreCreateMutex();

	// Enable USART2 
	USART_Cmd(USART2, ENABLE);

	U2_ClearTx();
	U2_ClearRx();

	NVIC_SetPriority(USART2_IRQn,USART2_IRQ_PRIO);

	// Enable IRQ
	NVIC_EnableIRQ(USART2_IRQn);

	active_U2 = TRUE;
}

/********************************************************************
*                                                                   *
*                           U2_Close                                *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Close the serial port                                             *
*                                                                   *
********************************************************************/
void U2_Close(void)
{
	NVIC_DisableIRQ(USART2_IRQn);
	USART_DeInit(USART2);

	U2_ClearBuf();

	USART_ITConfig(USART2, USART_IT_PE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART2, USART_IT_TC, DISABLE);
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART2, USART_IT_IDLE, DISABLE);
	USART_ITConfig(USART2, USART_IT_LBD, DISABLE);
	USART_ITConfig(USART2, USART_IT_CTS, DISABLE);
	USART_ITConfig(USART2, USART_IT_ERR, DISABLE);
	USART_ITConfig(USART2, USART_IT_ORE, DISABLE);
	USART_ITConfig(USART2, USART_IT_NE, DISABLE);
	USART_ITConfig(USART2, USART_IT_FE, DISABLE);

	active_U2 = FALSE;
}

uint8_t U2_Status(void)
{
	return active_U2;
}

/********************************************************************
*                                                                   *
*                           U2Puts                                  *
*                                                                   *
*********************************************************************
 INPUT  :   const char *s   : pointer to the string to send
 OUTPUT :   uint8_t            : TRUE  = succes of insertion operation
                              FALSE = error of insertion operation
*********************************************************************
*                                                                   *
* Send a string to DTE device                                       *
*                                                                   *
********************************************************************/
uint8_t U2_Puts(const char* s)
{
	return U2_Write((const uint8_t *)s,strlen(s));
}

/********************************************************************
*                                                                   *
*                           U2_Putc                                 *
*                                                                   *
*********************************************************************
 INPUT  :   uint8_t b      : char to send
 OUTPUT :   uint8_t        : TRUE  = succes of insertion operation
                          FALSE = error of insertion operation
*********************************************************************
*                                                                   *
* Send a char to DTE device                                         *
*                                                                   *
********************************************************************/
uint8_t U2_Putc(uint8_t b)
{
	b &= mask_U2;
	xQueueSend(xTxQueue,&b,portMAX_DELAY);
	USART_ITConfig(USART2, USART_IT_TXE, ENABLE);
	return TRUE;
}

uint8_t U2_Echo(uint8_t b)
{
	return U2_Putc(b);
}

/********************************************************************
*                                                                   *
*                           U2_Printf                               *
*                                                                   *
*********************************************************************
 INPUT  :   const char *fmt : formatter string
            ...             : argument list
 OUTPUT :   uint8_t            : TRUE  = succes of insertion operation
                              FALSE = error of insertion operation

*********************************************************************
*                                                                   *
* Send a formatted string to DTE device                             *
*                                                                   *
********************************************************************/
uint8_t U2_Printf(const char *fmt,...)
{
uint8_t res;
va_list argptr;

	xSemaphoreTake(xSemaPrintf,portMAX_DELAY);	// get mutex

    va_start(argptr, fmt);
    vsnprintf(prntfBuf, PRNTFBUF_SIZE, fmt, argptr);
    va_end(argptr);

    res = U2_Puts((const char *)prntfBuf);

	xSemaphoreGive(xSemaPrintf);		// release Mutex
	return res;
}

/********************************************************************
*                                                                   *
*                           U2_Write                                *
*                                                                   *
*********************************************************************
 INPUT  :   uint8_t *c             : Pointer to the buffer to send
            uint16_t len            : Number of bytes to send
*********************************************************************
*                                                                   *
* Send a frame of LEN char                                          *
*                                                                   *
********************************************************************/
uint8_t U2_Write(const uint8_t *c, uint16_t len)
{
	for (; len > 0; len--,c++)
		if (!U2_Putc(*c))
			break;
	return TRUE;
}

/********************************************************************
*                                                                   *
*                          U2_Getch                                 *
*                                                                   *
*********************************************************************
 OUTPUT :   int     :   -1      = No char in Rx buffer
                        0÷256   = Char readed
*********************************************************************
*                                                                   *
* Read a char from Rx buffer                                        *
*                                                                   *
********************************************************************/
int U2_Getch(uint8_t *b)
{
	// Check for char in Rx buffer
	xQueueReceive( xRxQueue, b, portMAX_DELAY);
	*b &= mask_U2;
	return TRUE;
}

int U2_TGetch(uint8_t *c,uint32_t tout)
{
	if (xQueueReceive( xRxQueue, c, tout))
		return TRUE;
	else
		return FALSE;
}

/********************************************************************
*                                                                   *
*                           U2_Read                                 *
*                                                                   *
*********************************************************************
 INPUT  :   uint8_t *c             : pointer to destinatione buffer
            uint16_t len            : Number of bytes to read
 OUTPUT :   uint16_t                : Number of readed bytes
*********************************************************************
*                                                                   *
* Read a number of bytes from Rx Buffer                             *
*                                                                   *
********************************************************************/
uint16_t U2_Read(uint8_t *c, uint16_t len)
{
uint16_t w;

	for (w = 0; w < len; w++,c++)
		if (!U2_Getch(c))
			break;
	return w;
}

/********************************************************************
*                                                                   *
*                          U2_ClearTx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission buffer                                          *
*                                                                   *
********************************************************************/
void U2_ClearTx(void)
{
	// disable the USART2 TX interrupt when the receive data register is empty   
	USART_ITConfig(USART2, USART_IT_TXE, DISABLE);
	if (xTxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xTxQueue, &b, 0));
	}
}

/********************************************************************
*                                                                   *
*                          U2_ClearRx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush reception buffer                                            *
*                                                                   *
********************************************************************/
void U2_ClearRx(void)
{
	// disable the USART2 RX interrupt when the receive data register is empty 
	USART_ITConfig(USART2, USART_IT_RXNE, DISABLE);

	if (xRxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xRxQueue,&b,0));
	}

	// enable the USART2 RX interrupt when the receive data register is empty 
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
}

/********************************************************************
*                                                                   *
*                          U2_ClearBuf                              *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission and reception buffers                           *
*                                                                   *
********************************************************************/
void U2_ClearBuf(void)
{
    U2_ClearTx();
    U2_ClearRx();
}

/********************************************************************
*                                                                   *
*                          U2_TxCount                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Number of bytes to send in trasmission buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in TX buffer                           *
*                                                                   *
********************************************************************/
uint16_t U2_TxCount(void)
{
	return uxQueueMessagesWaiting(xTxQueue);
}

/********************************************************************
*                                                                   *
*                           U2_TxSize                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Free space of trasmission buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space of trasmission buffer in bytes              *
*                                                                   *
********************************************************************/
uint16_t U2_TxSize(void)
{
	return TxQueueSize - U2_TxCount();
}

/********************************************************************
*                                                                   *
*                          U2_TxEmpty                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the TX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U2_TxEmpty(void)
{
	return U2_TxCount() == 0;
}

/********************************************************************
*                                                                   *
*                           U2_RxCount                              *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Number of receive bytes in Rx buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in RX buffer already receive from USART*
*                                                                   *
********************************************************************/
uint16_t U2_RxCount(void)
{
	return uxQueueMessagesWaiting(xRxQueue);
}

/********************************************************************
*                                                                   *
*                          U2_RxSize                                *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Free space of Rx buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space in Rx buffer in bytes                       *
*                                                                   *
********************************************************************/
uint16_t U2_RxSize(void)
{
    return (RxQueueSize - U2_RxCount());
}

/********************************************************************
*                                                                   *
*                           U2_RxEmpty                              *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the RX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U2_RxEmpty(void)
{
	return U2_RxCount() == 0;
}
