/************************************************************
*                                                           *
*                       usart4.h                            *
*                                                           *
*************************************************************
*                                                           *
*    Include file for custom usart communication metode     *
*                                                           *
************************************************************/

/* Define to prevent recursive inclusion -----------------------------------*/
#ifndef __USART4_H
#define __USART4_H

#include "uart.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void U4_Open      (uint32_t speed,uint16_t parity,uint16_t stopbits);    // Open the serial port for DTE comunications
extern void U4_Close     (void);                       // Close the serial port
extern uint8_t U4_Status	 (void);					   // Return a boolean indicating the status of the port, TRUE->active
extern uint8_t U4_Puts      (const char* s);              // Send a string to DTE device
extern uint8_t U4_Putc      (uint8_t b);                     // Send a char to DTE device
extern uint8_t U4_Printf    (const char *fmt,...);        // Send a formatted string to DTE device
extern uint8_t U4_Write     (const uint8_t *c, uint16_t len);    // Send a frame of LEN char
extern uint8_t U4_Getch     (void);                       // Read a char from Rx buffer
extern uint16_t U4_Read      (uint8_t *c, uint16_t len); // Read a number of bytes from Rx Buffer
extern void U4_ClearTx   (void);                       // Flush trasmission buffer
extern void U4_ClearRx   (void);                       // Flush reception buffer
extern void U4_ClearBuf  (void);                       // Flush trasmission and reception buffers
extern uint16_t U4_TxCount   (void);                       // Return the number of bytes to send in trasmission buffer
extern uint16_t U4_TxSize    (void);                       // Return the free space of trasmission buffer in bytes
extern uint8_t U4_TxEmpty   (void);                       // Return TX empty buffer state
extern uint16_t U4_RxCount   (void);                       // Return the number of bytes to read in Rx buffer
extern uint16_t U4_RxSize    (void);                       // Return the free space in Rx buffer in bytes
extern uint8_t U4_RxEmpty   (void);                       // Return RX empty buffer state

#endif
