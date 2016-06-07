/************************************************************
*                                                           *
*                       usart1.c                            *
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
#include "usart1.h"

#define QueueLength						64

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

void USART1_IRQHandler(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint8_t active;
static uint8_t mask;
static uint8_t remap;

/* Private function ----------------------------------------------------------*/
/*******************************************************************************
* Function Name  : U1_IRQHandler
* Description    : This function handles U1 custom usart interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART1_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
uint8_t cChar;

	if(USART_GetITStatus(USART1, USART_IT_TXE) != RESET)
	{   
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */
		if( xQueueReceiveFromISR( xTxQueue, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
		{
			/* A character was retrieved from the queue so can be sent to the
			THR now. */
			USART_SendData( USART1, cChar );
		}
		else
		{
			USART_ITConfig( USART1, USART_IT_TXE, DISABLE );		
		}		
	}

	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  
	{
		// receive data from host
		cChar = USART_ReceiveData( USART1 );
		xQueueSendFromISR( xRxQueue, &cChar, &xHigherPriorityTaskWoken );
		USART_ClearITPendingBit(USART1,USART_IT_RXNE);
		// Note: receive interrupt is always on!.
	}
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

/********************************************************************
*                                                                   *
*                           U1_Open                                 *
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
void U1_Open(uint32_t speed,uint16_t parity,uint16_t stopbits) 
{
USART_InitTypeDef USART_InitStructure;

	U1_Close() ;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);

	/* Enable/Disable the USART1 Pins Software Remapping */
	GPIO_PinRemapConfig(GPIO_Remap_USART1, (remap)? ENABLE: DISABLE);

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
		mask = 0xff;
		parity = USART_Parity_No;
		break;
	case UARTP_Even:
		mask = 0x7f;
		parity = USART_Parity_Even;
		break;
	case UARTP_Odd:
		mask = 0x7f;
		parity = USART_Parity_Odd;
		break;
	default:
		return;
	}

	//set Baud Rate  
	USART_InitStructure.USART_BaudRate = speed ;

	// USART1 parameters overtake configuration ----------------------------
	/* USART1 configured as follow:
	    - BaudRate = 115200 baud  
	    - Word Length = 8 Bits
	    - One Stop Bit
	    - No parity
	    - Hardware flow control enabled (RTS and CTS signals)
	    - Receive and transmit enabled
	*/
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_Parity = parity;	// USART_Parity_No ;
	USART_InitStructure.USART_StopBits = stopbits;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;

	// configure the USART1 
	USART_Init(USART1, &USART_InitStructure);

	if (remap)
	{
		/* Configure USART1 Rx (PB.07) as input floating */
		GpioInit(GPIOB,GPIO_Pin_7,GPIO_Mode_IN_FLOATING,0);

		/* Configure USART1 Tx (PB.06) as alternate function push-pull */
		GpioInit(GPIOB,GPIO_Pin_6,GPIO_Mode_AF_PP,0);
	}
	else
	{
		/* Configure USART1 Rx (PA.10) as input floating */
		GpioInit(GPIOA,GPIO_Pin_10,GPIO_Mode_IN_FLOATING,0);

		/* Configure USART1 Tx (PA.09) as alternate function push-pull */
		GpioInit(GPIOA,GPIO_Pin_9,GPIO_Mode_AF_PP,0);
	}

	if (!xRxQueue)
		xRxQueue = xQueueCreate( RxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!xTxQueue)
		xTxQueue = xQueueCreate( TxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!prntfBuf)
		prntfBuf = pvPortMalloc(PRNTFBUF_SIZE);
	if (!xSemaPrintf)
		xSemaPrintf = xSemaphoreCreateMutex();

	// Enable USART1 
	USART_Cmd(USART1, ENABLE);

	U1_ClearTx();
	U1_ClearRx();

	NVIC_SetPriority(USART1_IRQn,USART1_IRQ_PRIO);

	// Enable IRQ
	NVIC_EnableIRQ(USART1_IRQn);

	active = TRUE;
}

/********************************************************************
*                                                                   *
*                           U1_Close                                *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Close the serial port                                             *
*                                                                   *
********************************************************************/
void U1_Close(void)
{
	NVIC_DisableIRQ(USART1_IRQn);
	USART_DeInit(USART1);

	U1_ClearBuf();

	USART_ITConfig(USART1, USART_IT_PE, DISABLE);
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART1, USART_IT_TC, DISABLE);
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
	USART_ITConfig(USART1, USART_IT_LBD, DISABLE);
	USART_ITConfig(USART1, USART_IT_CTS, DISABLE);
	USART_ITConfig(USART1, USART_IT_ERR, DISABLE);
	USART_ITConfig(USART1, USART_IT_ORE, DISABLE);
	USART_ITConfig(USART1, USART_IT_NE, DISABLE);
	USART_ITConfig(USART1, USART_IT_FE, DISABLE);

	active = FALSE;
}

uint8_t U1_Status(void)
{
	return active;
}

/********************************************************************
*                                                                   *
*                           U1Puts                                  *
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
uint8_t U1_Puts(const char* s)
{
	return U1_Write((const uint8_t *)s,strlen(s));
}

/********************************************************************
*                                                                   *
*                           U1_Putc                                 *
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
uint8_t U1_Putc(uint8_t b)
{
	return U1_Write(&b,1);
}

/********************************************************************
*                                                                   *
*                           U1_Printf                               *
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
uint8_t U1_Printf(const char *fmt,...)
{
uint8_t res;
va_list argptr;

	xSemaphoreTake(xSemaPrintf,portMAX_DELAY);	// get mutex

    va_start(argptr, fmt);
    vsnprintf(prntfBuf, PRNTFBUF_SIZE, fmt, argptr);
    va_end(argptr);

    res = U1_Puts((const char *)prntfBuf);

	xSemaphoreGive(xSemaPrintf);		// release Mutex
	return res;
}

/********************************************************************
*                                                                   *
*                           U1_Write                                *
*                                                                   *
*********************************************************************
 INPUT  :   uint8_t *c             : Pointer to the buffer to send
            uint16_t len            : Number of bytes to send
*********************************************************************
*                                                                   *
* Send a frame of LEN char                                          *
*                                                                   *
********************************************************************/
uint8_t U1_Write(const uint8_t *c, uint16_t len)
{
uint8_t cOutChar;

	// Write all the frame to the transmit data register   
	for (; len > 0; len--,c++)
	{
		cOutChar = *c & mask;
		xQueueSend( xTxQueue, &cOutChar, portMAX_DELAY );
		USART_ITConfig(USART1, USART_IT_TXE, ENABLE);
	}
	return TRUE;
}

/********************************************************************
*                                                                   *
*                          U1_Getch                                 *
*                                                                   *
*********************************************************************
 OUTPUT :   int     :   -1      = No char in Rx buffer
                        0÷256   = Char readed
*********************************************************************
*                                                                   *
* Read a char from Rx buffer                                        *
*                                                                   *
********************************************************************/
int U1_Getch(uint8_t *b)
{
	// Check for char in Rx buffer
	xQueueReceive( xRxQueue, b, portMAX_DELAY);
	*b &= mask;
	return TRUE;
}

/********************************************************************
*                                                                   *
*                          U1_TGetch                                *
*                                                                   *
*********************************************************************
 OUTPUT :   int     :   -1      = No char in Rx buffer
                        0÷256   = Char readed
*********************************************************************
*                                                                   *
* Read a char from Rx buffer                                        *
*                                                                   *
********************************************************************/
int U1_TGetch(uint8_t *c,uint32_t tout)
{
	// Check for char in Rx buffer
	if (xQueueReceive( xRxQueue, c, tout))
	{
		*c &= mask;
		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************
*                                                                   *
*                           U1_Read                                 *
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
uint16_t U1_Read(uint8_t *c, uint16_t len)
{
uint16_t w;

	for (w = 0; w < len; w++,c++)
		U1_Getch(c);
	return len;
}

/********************************************************************
*                                                                   *
*                          U1_ClearTx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission buffer                                          *
*                                                                   *
********************************************************************/
void U1_ClearTx(void)
{
	// disable the USART1 TX interrupt when the receive data register is empty   
	USART_ITConfig(USART1, USART_IT_TXE, DISABLE);
	if (xTxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xTxQueue, &b, 0));
	}
}

/********************************************************************
*                                                                   *
*                          U1_ClearRx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush reception buffer                                            *
*                                                                   *
********************************************************************/
void U1_ClearRx(void)
{
	// disable the USART1 RX interrupt when the receive data register is empty 
	USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);

	if (xRxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xRxQueue,&b,0));
	}
 
	// enable the USART1 RX interrupt when the receive data register is empty 
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
}

/********************************************************************
*                                                                   *
*                          U1_ClearBuf                              *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission and reception buffers                           *
*                                                                   *
********************************************************************/
void U1_ClearBuf(void)
{
    U1_ClearTx();
    U1_ClearRx();
}

/********************************************************************
*                                                                   *
*                          U1_TxCount                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Number of bytes to send in trasmission buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in TX buffer                           *
*                                                                   *
********************************************************************/
uint16_t U1_TxCount(void)
{
	return uxQueueMessagesWaiting(xTxQueue);
}

/********************************************************************
*                                                                   *
*                           U1_TxSize                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Free space of trasmission buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space of trasmission buffer in bytes              *
*                                                                   *
********************************************************************/
uint16_t U1_TxSize(void)
{
	return TxQueueSize - U1_TxCount();
}

/********************************************************************
*                                                                   *
*                          U1_TxEmpty                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the TX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U1_TxEmpty(void)
{
	return U1_TxCount() == 0;
}

/********************************************************************
*                                                                   *
*                           U1_RxCount                              *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Number of receive bytes in Rx buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in RX buffer already receive from USART*
*                                                                   *
********************************************************************/
uint16_t U1_RxCount(void)
{
	return uxQueueMessagesWaiting(xRxQueue);
}

/********************************************************************
*                                                                   *
*                          U1_RxSize                                *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Free space of Rx buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space in Rx buffer in bytes                       *
*                                                                   *
********************************************************************/
uint16_t U1_RxSize(void)
{
    return (RxQueueSize - U1_RxCount());
}

/********************************************************************
*                                                                   *
*                           U1_RxEmpty                              *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the RX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U1_RxEmpty(void)
{
	return U1_RxCount() == 0;
}
