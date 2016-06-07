/************************************************************
*                                                           *
*                       usart5.c                            *
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
#include "usart5.h"

#define QueueLength						128

#define RxQueueSize						QueueLength
#define TxQueueSize						QueueLength

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

void UART5_IRQHandler(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint8_t active_U5;
static uint8_t mask_U5;

/* Private function ----------------------------------------------------------*/
/*******************************************************************************
* Function Name  : UART5_IRQHandler
* Description    : This function handles U5 custom usart interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART5_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
uint8_t cChar;

	if(USART_GetITStatus(UART5, USART_IT_TXE) != RESET)
	{   
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */
		if( xQueueReceiveFromISR( xTxQueue, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_SendData( USART5, cChar );
		}
		else
		{
			USART_ITConfig( USART5, USART_IT_TXE, DISABLE );		
		}		
	}

	if(USART_GetITStatus(USART5, USART_IT_RXNE) != RESET)  
	{
		// receive data from host
		cChar = USART_ReceiveData( USART5 );
		xQueueSendFromISR( xRxQueue, &cChar, &xHigherPriorityTaskWoken );
		USART_ClearITPendingBit(USART5,USART_IT_RXNE);
		// Note: receive interrupt is always on!.
	}
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

/********************************************************************
*                                                                   *
*                           U5_Open                                 *
*                                                                   *
*********************************************************************
 INPUT  :   uint32_t speed : baudrate
            uint16_t  ft    : frame type    see FRAME_TYPE_enum
            uint16_t  pt    : parity type   see PARITY_TYPE_enum
*********************************************************************
*                                                                   *
* Open the serial port for DTE comunications                        *
*                                                                   *
********************************************************************/
void U5_Open(uint32_t speed,uint16_t parity,uint16_t stopbits) 
{
USART_InitTypeDef USART_InitStructure;

	U5_Close() ;

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
		mask_U5 = 0xff;
		parity = USART_Parity_No;
		break;
	case UARTP_Even:
		mask_U5 = 0x7f;
		parity = USART_Parity_Even;
		break;
	case UARTP_Odd:
		mask_U5 = 0x7f;
		parity = USART_Parity_Odd;
		break;
	default:
		return;
	}

	//set Baud Rate  
	USART_InitStructure.USART_BaudRate = speed ;

	// UART5 parameters overtake configuration ----------------------------
	/* UART5 configured as follow:
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

	// configure the UART5
	USART_Init(UART5, &USART_InitStructure);

	/* Configure UART5 Rx (PD.2) as input floating */
	GpioInit(GPIOD,GPIO_Pin_2,GPIO_Mode_IN_FLOATING,0);

	/* Configure UART5 Tx (PC.12) as alternate function push-pull */
	GpioInit(GPIOC,GPIO_Pin_12,GPIO_Mode_AF_PP,0);

	if (!xRxQueue)
		xRxQueue = xQueueCreate( RxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!xTxQueue)
		xTxQueue = xQueueCreate( TxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!prntfBuf)
		prntfBuf = pvPortMalloc(PRNTFBUF_SIZE);
	if (!xSemaPrintf)
		xSemaPrintf = xSemaphoreCreateMutex();

	// Enable UART5 
	USART_Cmd(UART5, ENABLE);

	U5_ClearTx();
	U5_ClearRx();

	NVIC_SetPriority(UART5_IRQn,UART5_IRQ_PRIO);

	// Enable IRQ
	NVIC_EnableIRQ(UART5_IRQn);

	active_U5 = TRUE;
}

/********************************************************************
*                                                                   *
*                           U5_Close                                *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Close the serial port                                             *
*                                                                   *
********************************************************************/
void U5_Close(void)
{
	NVIC_DisableIRQ(UART5_IRQn);
	USART_DeInit(UART5);

	U5_ClearBuf();

	USART_ITConfig(UART5, USART_IT_PE, DISABLE);
	USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
	USART_ITConfig(UART5, USART_IT_TC, DISABLE);
	USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);
	USART_ITConfig(UART5, USART_IT_IDLE, DISABLE);
	USART_ITConfig(UART5, USART_IT_LBD, DISABLE);
	USART_ITConfig(UART5, USART_IT_CTS, DISABLE);
	USART_ITConfig(UART5, USART_IT_ERR, DISABLE);
	USART_ITConfig(UART5, USART_IT_ORE, DISABLE);
	USART_ITConfig(UART5, USART_IT_NE, DISABLE);
	USART_ITConfig(UART5, USART_IT_FE, DISABLE);

	active_U5 = FALSE;
}

uint8_t U5_Status(void)
{
	return active_U5;
}

/********************************************************************
*                                                                   *
*                           U5_Puts                                 *
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
uint8_t U5_Puts(const char* s)
{
	return U5_Write((const uint8_t *)s,strlen(s));
}

/********************************************************************
*                                                                   *
*                           U5_Putc                                 *
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
uint8_t U5_Putc(uint8_t b)
{
	return U5_Write(&b,1);
}

/********************************************************************
*                                                                   *
*                           U5_Printf                               *
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
uint8_t U5_Printf(const char *fmt,...)
{
uint8_t res;
va_list argptr;

	xSemaphoreTake(xSemaPrintf,portMAX_DELAY);	// get mutex

    va_start(argptr, fmt);
    vsnprintf(prntfBuf, PRNTFBUF_SIZE, fmt, argptr);
    va_end(argptr);

    res = U5_Puts((const char *)prntfBuf);

	xSemaphoreGive(xSemaPrintf);		// release Mutex
	return res;
}

/********************************************************************
*                                                                   *
*                           U5_Write                                *
*                                                                   *
*********************************************************************
 INPUT  :   uint8_t *c             : Pionter to the buffer to send
            uint16_t len            : Number of bytes to send
*********************************************************************
*                                                                   *
* Send a frame of LEN char                                          *
*                                                                   *
********************************************************************/
uint8_t U5_Write(const uint8_t *c, uint16_t len)
{
uint8_t cOutChar;

	// Write all the frame to the transmit data register   
	for (; len > 0; len--,c++)
	{
		cOutChar = *c & mask_U5;
		xQueueSend( xTxQueue, &cOutChar, portMAX_DELAY );
		USART_ITConfig(UART5, USART_IT_TXE, ENABLE);
	}
	return TRUE;
}

/********************************************************************
*                                                                   *
*                          U5_Getch                                 *
*                                                                   *
*********************************************************************
 OUTPUT :   int     :   -1      = No char in Rx buffer
                        0÷256   = Char readed
*********************************************************************
*                                                                   *
* Read a char from Rx buffer                                        *
*                                                                   *
********************************************************************/
int U5_Getch(void)
{
uint8_t cInChar;

  // Check for char in Rx buffer
	xQueueReceive( xRxQueue, &cInChar, portMAX_DELAY);
	cInChar &= mask_U5;
	return cInChar;
}

/********************************************************************
*                                                                   *
*                           U5_Read                                 *
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
uint16_t U5_Read(uint8_t *c, uint16_t len)
{
uint16_t w;

	for (w = 0; w < len; w++,c++)
		*c = U5_Getch();
	return len;
}

/********************************************************************
*                                                                   *
*                          U5_ClearTx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission buffer                                          *
*                                                                   *
********************************************************************/
void U5_ClearTx(void)
{
	// disable the UART5 TX interrupt when the receive data register is empty   
	USART_ITConfig(UART5, USART_IT_TXE, DISABLE);
	if (xTxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xTxQueue, &b, 0));
	}
}

/********************************************************************
*                                                                   *
*                          U5_ClearRx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush reception buffer                                            *
*                                                                   *
********************************************************************/
void U5_ClearRx(void)
{
	// disable the UART5 RX interrupt when the receive data register is empty 
	USART_ITConfig(UART5, USART_IT_RXNE, DISABLE);

	if (xRxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xRxQueue,&b,0));
	}

	// enable the UART5 RX interrupt when the receive data register is empty 
	USART_ITConfig(UART5, USART_IT_RXNE, ENABLE);
}

/********************************************************************
*                                                                   *
*                          U5_ClearBuf                              *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission and reception buffers                           *
*                                                                   *
********************************************************************/
void U5_ClearBuf(void)
{
    U5_ClearTx();
    U5_ClearRx();
}

/********************************************************************
*                                                                   *
*                          U5_TxCount                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Number of bytes to send in trasmission buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in TX buffer                           *
*                                                                   *
********************************************************************/
uint16_t U5_TxCount(void)
{
	return uxQueueMessagesWaiting(xTxedChars);
}

/********************************************************************
*                                                                   *
*                           U5_TxSize                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Free space of trasmission buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space of trasmission buffer in bytes              *
*                                                                   *
********************************************************************/
uint16_t U5_TxSize(void)
{
	return TxQueueSize - U5_TxCount();
}

/********************************************************************
*                                                                   *
*                          U5_TxEmpty                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the TX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U5_TxEmpty(void)
{
	return U5_TxCount() == 0;
}

/********************************************************************
*                                                                   *
*                           U5_RxCount                              *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Number of receive bytes in Rx buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in RX buffer already receive from USART*
*                                                                   *
********************************************************************/
uint16_t U5_RxCount(void)
{
	return uxQueueMessagesWaiting(xRxQueue);
}

/********************************************************************
*                                                                   *
*                          U5_RxSize                                *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Free space of Rx buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space in Rx buffer in bytes                       *
*                                                                   *
********************************************************************/
uint16_t U5_RxSize(void)
{
    return (RxQueueSize - U5_RxCount());
}

/********************************************************************
*                                                                   *
*                           U5_RxEmpty                              *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the RX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U5_RxEmpty(void)
{
	return U5_RxCount() == 0;
}
