/************************************************************
*                                                           *
*                       usart4.c                            *
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
#include "usart4.h"

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

void UART4_IRQHandler(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint8_t active_U4;
static uint8_t mask_U4;

/* Private function ----------------------------------------------------------*/
/*******************************************************************************
* Function Name  : UART4_IRQHandler
* Description    : This function handles U4 custom usart interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UART4_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
uint8_t cChar;

	if(USART_GetITStatus(UART4, USART_IT_TXE) != RESET)
	{   
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */
		if( xQueueReceiveFromISR( xTxQueue, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_SendData( UART4, cChar );
		}
		else
		{
			USART_ITConfig( UART4, USART_IT_TXE, DISABLE );		
		}		
	}

	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  
	{
		// receive data from host
		cChar = USART_ReceiveData( UART4 );
		xQueueSendFromISR( xRxQueue, &cChar, &xHigherPriorityTaskWoken );
		USART_ClearITPendingBit(UART4,USART_IT_RXNE);
		// Note: receive interrupt is always on!.
	}
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void U4_Remap(int enable)
{
}

/********************************************************************
*                                                                   *
*                           U4_Open                                 *
*                                                                   *
*********************************************************************
 INPUT  :   uint32_t speed : baudrate
            Word  ft    : frame type    see FRAME_TYPE_enum
            Word  pt    : parity type   see PARITY_TYPE_enum
*********************************************************************
*                                                                   *
* Open the serial port for DTE comunications                        *
*                                                                   *
********************************************************************/
void U4_Open(uint32_t speed,uint16_t parity,uint16_t stopbits) 
{
USART_InitTypeDef USART_InitStructure;

	U4_Close() ;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4, ENABLE);

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
		mask_U4 = 0xff;
		parity = USART_Parity_No;
		break;
	case UARTP_Even:
		mask_U4 = 0x7f;
		parity = USART_Parity_Even;
		break;
	case UARTP_Odd:
		mask_U4 = 0x7f;
		parity = USART_Parity_Odd;
		break;
	default:
		return;
	}

  //set Baud Rate  
  USART_InitStructure.USART_BaudRate = speed ;

	// UART4 parameters overtake configuration ----------------------------
	/* UART4 configured as follow:
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

	// configure the UART4
	USART_Init(UART4, &USART_InitStructure);

	/* Configure UART$ Rx (PC.11) as input floating */
	GpioInit(GPIOC,GPIO_Pin_11,GPIO_Mode_IN_FLOATING,0);

	/* Configure UART4 Tx (PC.10) as alternate function push-pull */
	GpioInit(GPIOC,GPIO_Pin_10,GPIO_Mode_AF_PP,0);

	if (!xRxQueue)
		xRxQueue = xQueueCreate( RxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!xTxQueue)
		xTxQueue = xQueueCreate( TxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!prntfBuf)
		prntfBuf = pvPortMalloc(PRNTFBUF_SIZE);
	if (!xSemaPrintf)
		xSemaPrintf = xSemaphoreCreateMutex();

	// Enable UART4 
	USART_Cmd(UART4, ENABLE);

	U4_ClearTx();
	U4_ClearRx();

	NVIC_SetPriority(UART4_IRQn,UART4_IRQ_PRIO);

	// Enable IRQ
	NVIC_EnableIRQ(UART4_IRQn);

	active_U4 = TRUE;
}

/********************************************************************
*                                                                   *
*                           U4_Close                                *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Close the serial port                                             *
*                                                                   *
********************************************************************/
void U4_Close(void)
{
uint8_t b;

	NVIC_DisableIRQ(UART4_IRQn);
	USART_DeInit(UART4);

	U4_ClearBuf();

	USART_ITConfig(UART4, USART_IT_PE, DISABLE);
	USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
	USART_ITConfig(UART4, USART_IT_TC, DISABLE);
	USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);
	USART_ITConfig(UART4, USART_IT_IDLE, DISABLE);
	USART_ITConfig(UART4, USART_IT_LBD, DISABLE);
	USART_ITConfig(UART4, USART_IT_CTS, DISABLE);
	USART_ITConfig(UART4, USART_IT_ERR, DISABLE);
	USART_ITConfig(UART4, USART_IT_ORE, DISABLE);
	USART_ITConfig(UART4, USART_IT_NE, DISABLE);
	USART_ITConfig(UART4, USART_IT_FE, DISABLE);

	active_U4 = FALSE;
}

uint8_t U4_Status(void)
{
	return active_U4;
}

/********************************************************************
*                                                                   *
*                           U4_Puts                                 *
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
uint8_t U4_Puts(const char* s)
{
	return U4_Write((const uint8_t *)s,strlen(s));
}

/********************************************************************
*                                                                   *
*                           U4_Putc                                 *
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
uint8_t U4_Putc(uint8_t b)
{
	return U4_Write(&b,1);
}

/********************************************************************
*                                                                   *
*                           U4_Printf                               *
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
uint8_t U4_Printf(const char *fmt,...)
{
uint8_t res;
va_list argptr;

	xSemaphoreTake(xSemaPrintf,portMAX_DELAY);	// get mutex

    va_start(argptr, fmt);
    vsnprintf(prntfBuf, PRNTFBUF_SIZE, fmt, argptr);
    va_end(argptr);

    res = U4_Puts((const char *)prntfBuf);

	xSemaphoreGive(xSemaPrintf);		// release Mutex
	return res;
}

/********************************************************************
*                                                                   *
*                           U4_Write                                *
*                                                                   *
*********************************************************************
 INPUT  :   uint8_t *c             : Pionter to the buffer to send
            uint16_t len            : Number of bytes to send
*********************************************************************
*                                                                   *
* Send a frame of LEN char                                          *
*                                                                   *
********************************************************************/
uint8_t U4_Write(const uint8_t *c, uint16_t len)
{
uint8_t cOutChar;

	// Write all the frame to the transmit data register   
	for (; len > 0; len--,c++)
	{
		cOutChar = *c & mask_U4;
		xQueueSend( xTxQueue, &cOutChar, portMAX_DELAY );
		USART_ITConfig(UART4, USART_IT_TXE, ENABLE);
	}
	return TRUE;
}

/********************************************************************
*                                                                   *
*                          U4_Getch                                 *
*                                                                   *
*********************************************************************
 OUTPUT :   int     :   -1      = No char in Rx buffer
                        0÷256   = Char readed
*********************************************************************
*                                                                   *
* Read a char from Rx buffer                                        *
*                                                                   *
********************************************************************/
int U4_Getch(void)
{
uint8_t cInChar;

  // Check for char in Rx buffer
	xQueueReceive( xRxQueue, &cInChar, portMAX_DELAY);
	cInChar &= mask_U4;
	return cInChar;
}

/********************************************************************
*                                                                   *
*                           U4_Read                                 *
*                                                                   *
*********************************************************************
 INPUT  :   unsigned char *c    : pointer to destinatione buffer
            uint16_t len            : Number of bytes to read
 OUTPUT :   uint16_t                : Number of readed bytes
*********************************************************************
*                                                                   *
* Read a number of bytes from Rx Buffer                             *
*                                                                   *
********************************************************************/
uint16_t U4_Read(unsigned char *c, uint16_t len)
{
uint16_t w;

	for (w = 0; w < len; w++,c++)
		*c = U4_Getch();
	return len;
}

/********************************************************************
*                                                                   *
*                          U4_ClearTx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission buffer                                          *
*                                                                   *
********************************************************************/
void U4_ClearTx(void)
{
	// disable the UART4 TX interrupt when the receive data register is empty   
	USART_ITConfig(UART4, USART_IT_TXE, DISABLE);
	if (xTxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xTxQueue, &b, 0));
	}
}

/********************************************************************
*                                                                   *
*                          U4_ClearRx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush reception buffer                                            *
*                                                                   *
********************************************************************/
void U4_ClearRx(void)
{
	// disable the UART4 RX interrupt when the receive data register is empty 
	USART_ITConfig(UART4, USART_IT_RXNE, DISABLE);

	if (xRxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xRxQueue,&b,0));
	}

	// enable the UART4 RX interrupt when the receive data register is empty 
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
}

/********************************************************************
*                                                                   *
*                          U4_ClearBuf                              *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission and reception buffers                           *
*                                                                   *
********************************************************************/
void U4_ClearBuf(void)
{
    U4_ClearTx();
    U4_ClearRx();
}

/********************************************************************
*                                                                   *
*                          U4_TxCount                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Number of bytes to send in trasmission buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in TX buffer                           *
*                                                                   *
********************************************************************/
uint16_t U4_TxCount(void)
{
	return uxQueueMessagesWaiting(xTxedChars);
}

/********************************************************************
*                                                                   *
*                           U4_TxSize                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Free space of trasmission buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space of trasmission buffer in bytes              *
*                                                                   *
********************************************************************/
uint16_t U4_TxSize(void)
{
	return TxQueueSize - U4_TxCount();
}

/********************************************************************
*                                                                   *
*                          U4_TxEmpty                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the TX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U4_TxEmpty(void)
{
	return U4_TxCount() == 0;
}

/********************************************************************
*                                                                   *
*                           U4_RxCount                              *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Number of receive bytes in Rx buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in RX buffer already receive from USART*
*                                                                   *
********************************************************************/
Word U4_RxCount(void)
{
	return uxQueueMessagesWaiting(xRxQueue);
}

/********************************************************************
*                                                                   *
*                          U4_RxSize                                *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Free space of Rx buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space in Rx buffer in bytes                       *
*                                                                   *
********************************************************************/
Word U4_RxSize(void)
{
    return (RxQueueSize - U4_RxCount());
}

/********************************************************************
*                                                                   *
*                           U4_RxEmpty                              *
*                                                                   *
*********************************************************************
 OUTPUT :   Word    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the RX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U4_RxEmpty(void)
{
	return U4_RxCount() == 0;
}
