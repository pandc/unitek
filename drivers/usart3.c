/************************************************************
*                                                           *
*                       usart3.c                            *
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
#include "usart3.h"

#define QueueLength					64

#define RxQueueSize					QueueLength
#define TxQueueSize					QueueLength

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

void USART3_IRQHandler(void);

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

static uint8_t active_U3;
static uint8_t mask_U3;
static uint8_t remap_U3;

/* Private function ----------------------------------------------------------*/
/*******************************************************************************
* Function Name  : USART3_IRQHandler
* Description    : This function handles U3 custom usart interrupt request.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void USART3_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
uint8_t cChar;

	if(USART_GetITStatus(USART3, USART_IT_TXE) != RESET)
	{   
		/* The interrupt was caused by the THR becoming empty.  Are there any
		more characters to transmit? */
		if( xQueueReceiveFromISR( xTxQueue, &cChar, &xHigherPriorityTaskWoken ) == pdTRUE )
		{
			// transmit data to host        
			USART_SendData(USART3, cChar);                    
		}
		else
		{
			// Disable the USART3 Transmit empty register interrupt 
			USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
		}
	}

	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  
	{
		// receive data from host
		cChar = USART_ReceiveData( USART3 );
		xQueueSendFromISR( xRxQueue, &cChar, &xHigherPriorityTaskWoken );
		USART_ClearITPendingBit(USART3,USART_IT_RXNE);
		// Note: receive interrupt is always on!.
	}
	portEND_SWITCHING_ISR( xHigherPriorityTaskWoken );
}

void U3_Remap(uint8_t enable)
{
	remap_U3 = enable;
}

/********************************************************************
*                                                                   *
*                           U3_Open                                 *
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
void U3_Open(uint32_t speed,uint16_t parity,uint16_t stopbits) 
{
USART_InitTypeDef USART_InitStructure;

	U3_Close() ;

	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);

    /* Enable/Disable the USART3 Pins Software Remapping */
    GPIO_PinRemapConfig(GPIO_FullRemap_USART3, (remap_U3)? ENABLE: DISABLE);

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
		mask_U3 = 0xff;
		parity = USART_Parity_No;
		break;
	case UARTP_Even:
		mask_U3 = 0x7f;
		parity = USART_Parity_Even;
		break;
	case UARTP_Odd:
		mask_U3 = 0x7f;
		parity = USART_Parity_Odd;
		break;
	default:
		return;
	}

	//set Baud Rate  
	USART_InitStructure.USART_BaudRate = speed ;

	// USART3 parameters overtake configuration ----------------------------
	/* USART3 configured as follow:
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

	// configure the USART3
	USART_Init(USART3, &USART_InitStructure);

	if (remap_U3)
	{
		/* Configure USART3 Rx (PD.9) as input floating */
		GpioInit(GPIOD,GPIO_Pin_9,GPIO_Mode_IN_FLOATING,0);

		/* Configure USART3 Tx (PD.8) as alternate function push-pull */
		GpioInit(GPIOD,GPIO_Pin_8,GPIO_Mode_AF_PP,0);
	}
	else
	{
		/* Configure USART3 Rx (PB.11) as input floating */
		GpioInit(GPIOB,GPIO_Pin_11,GPIO_Mode_IN_FLOATING,0);

		/* Configure USART3 Tx (PB.10) as alternate function push-pull */
		GpioInit(GPIOB,GPIO_Pin_10,GPIO_Mode_AF_PP,0);
	}

	if (!xRxQueue)
		xRxQueue = xQueueCreate( RxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!xTxQueue)
		xTxQueue = xQueueCreate( TxQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	if (!prntfBuf)
		prntfBuf = pvPortMalloc(PRNTFBUF_SIZE);
	if (!xSemaPrintf)
		xSemaPrintf = xSemaphoreCreateMutex();

	// Enable USART3
	USART_Cmd(USART3, ENABLE);

	U3_ClearTx();
	U3_ClearRx();

	NVIC_SetPriority(USART3_IRQn,USART3_IRQ_PRIO);

	// Enable IRQ
	NVIC_EnableIRQ(USART3_IRQn);

	active_U3 = TRUE;    
}

/********************************************************************
*                                                                   *
*                           U3_Close                                *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Close the serial port                                             *
*                                                                   *
********************************************************************/
void U3_Close(void)
{
	NVIC_DisableIRQ(USART3_IRQn);
	USART_DeInit(USART3);

	U3_ClearBuf();

	USART_ITConfig(USART3, USART_IT_PE, DISABLE);
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	USART_ITConfig(USART3, USART_IT_TC, DISABLE);
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);
	USART_ITConfig(USART3, USART_IT_IDLE, DISABLE);
	USART_ITConfig(USART3, USART_IT_LBD, DISABLE);
	USART_ITConfig(USART3, USART_IT_CTS, DISABLE);
	USART_ITConfig(USART3, USART_IT_ERR, DISABLE);
	USART_ITConfig(USART3, USART_IT_ORE, DISABLE);
	USART_ITConfig(USART3, USART_IT_NE, DISABLE);
	USART_ITConfig(USART3, USART_IT_FE, DISABLE);

	active_U3 = FALSE;
}

uint8_t U3_Status(void)
{
	return active_U3;
}

/********************************************************************
*                                                                   *
*                           U3_Puts                                 *
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
uint8_t U3_Puts(const char* s)
{
	return U3_Write((const uint8_t *)s,strlen(s));
}

/********************************************************************
*                                                                   *
*                           U3_Putc                                 *
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
uint8_t U3_Putc(uint8_t b)
{
	return U3_Write(&b,1);
}

/********************************************************************
*                                                                   *
*                           U3_Printf                               *
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
uint8_t U3_Printf(const char *fmt,...)
{
uint8_t res;
va_list argptr;

	xSemaphoreTake(xSemaPrintf,portMAX_DELAY);	// get mutex

    va_start(argptr, fmt);
    vsnprintf(prntfBuf, PRNTFBUF_SIZE, fmt, argptr);
    va_end(argptr);

    res = U3_Puts((const char *)prntfBuf);

	xSemaphoreGive(xSemaPrintf);		// release Mutex
	return res;
}

/********************************************************************
*                                                                   *
*                           U3_Write                                *
*                                                                   *
*********************************************************************
 INPUT  :   uint8_t *c             : Pointer to the buffer to send
            uint16_t len            : Number of bytes to send
*********************************************************************
*                                                                   *
* Send a frame of LEN char                                          *
*                                                                   *
********************************************************************/
uint8_t U3_Write(const uint8_t *c, uint16_t len)
{
uint8_t cOutChar;

	// Write all the frame to the transmit data register   
	for (; len > 0; len--,c++)
	{
		cOutChar = *c & mask_U3;
		xQueueSend( xTxQueue, &cOutChar, portMAX_DELAY );
		USART_ITConfig(USART3, USART_IT_TXE, ENABLE);
	}
	return TRUE;
}

/********************************************************************
*                                                                   *
*                          U3_Getch                                 *
*                                                                   *
*********************************************************************
 OUTPUT :   int     :   -1      = No char in Rx buffer
                        0÷256   = Char readed
*********************************************************************
*                                                                   *
* Read a char from Rx buffer                                        *
*                                                                   *
********************************************************************/
uint8_t U3_Getch(void)
{
uint8_t cInChar;

  // Check for char in Rx buffer
	xQueueReceive( xRxQueue, &cInChar, portMAX_DELAY);
	cInChar &= mask_U3;
	return cInChar;
}

/********************************************************************
*                                                                   *
*                          U3_TGetch                                *
*                                                                   *
*********************************************************************
 OUTPUT :   int     :   -1      = No char in Rx buffer
                        0÷256   = Char readed
*********************************************************************
*                                                                   *
* Read a char from Rx buffer                                        *
*                                                                   *
********************************************************************/
int U3_TGetch(uint8_t *c,uint32_t tout)
{
	// Check for char in Rx buffer
	if (xQueueReceive( xRxQueue, c, tout))
	{
		*c &= mask_U3;
		return TRUE;
	}
	else
		return FALSE;
}

/********************************************************************
*                                                                   *
*                           U3_Read                                 *
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
uint16_t U3_Read(uint8_t *c, uint16_t len)
{
uint16_t w;

	for (w = 0; w < len; w++,c++)
		*c = U3_Getch();
	return len;
}

/********************************************************************
*                                                                   *
*                          U3_ClearTx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission buffer                                          *
*                                                                   *
********************************************************************/
void U3_ClearTx(void)
{
	// disable the USART3 TX interrupt when the receive data register is empty   
	USART_ITConfig(USART3, USART_IT_TXE, DISABLE);
	if (xTxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xTxQueue, &b, 0));
	}
}

/********************************************************************
*                                                                   *
*                          U3_ClearRx                               *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush reception buffer                                            *
*                                                                   *
********************************************************************/
void U3_ClearRx(void)
{
	// disable the USART3 RX interrupt when the receive data register is empty 
	USART_ITConfig(USART3, USART_IT_RXNE, DISABLE);

	if (xRxQueue)
	{
		uint8_t b;
		while (xQueueReceive(xRxQueue,&b,0));
	}

	// enable the USART3 RX interrupt when the receive data register is empty 
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
}

/********************************************************************
*                                                                   *
*                          U3_ClearBuf                              *
*                                                                   *
*********************************************************************
*********************************************************************
*                                                                   *
* Flush trasmission and reception buffers                           *
*                                                                   *
********************************************************************/
void U3_ClearBuf(void)
{
    U3_ClearTx();
    U3_ClearRx();
}

/********************************************************************
*                                                                   *
*                          U3_TxCount                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Number of bytes to send in trasmission buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in TX buffer                           *
*                                                                   *
********************************************************************/
uint16_t U3_TxCount(void)
{
	return uxQueueMessagesWaiting(xTxQueue);
}

/********************************************************************
*                                                                   *
*                           U3_TxSize                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    : Free space of trasmission buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space of trasmission buffer in bytes              *
*                                                                   *
********************************************************************/
uint16_t U3_TxSize(void)
{
	return TxQueueSize - U3_TxCount();
}

/********************************************************************
*                                                                   *
*                          U3_TxEmpty                               *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the TX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U3_TxEmpty(void)
{
	return U3_TxCount() == 0;
}

/********************************************************************
*                                                                   *
*                           U3_RxCount                              *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Number of receive bytes in Rx buffer
*********************************************************************
*                                                                   *
* Return the number of bytes in RX buffer already receive from USART*
*                                                                   *
********************************************************************/
uint16_t U3_RxCount(void)
{
	return uxQueueMessagesWaiting(xRxQueue);
}

/********************************************************************
*                                                                   *
*                          U3_RxSize                                *
*                                                                   *
*********************************************************************
 OUTPUT :   uint8_t    : Free space of Rx buffer in bytes
*********************************************************************
*                                                                   *
* Return the free space in Rx buffer in bytes                       *
*                                                                   *
********************************************************************/
uint16_t U3_RxSize(void)
{
    return (RxQueueSize - U3_RxCount());
}

/********************************************************************
*                                                                   *
*                           U3_RxEmpty                              *
*                                                                   *
*********************************************************************
 OUTPUT :   uint16_t    :   TRUE  Buffer empty State
                        FALSE Buffer not empty state
*********************************************************************
*                                                                   *
* Return if the RX buffer is empty or not.                          *
*                                                                   *
********************************************************************/
uint8_t U3_RxEmpty(void)
{
	return U3_RxCount() == 0;
}
