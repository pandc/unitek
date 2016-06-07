/************************************************************
*                                                           *
*                       usart5.h                            *
*                                                           *
*************************************************************
*                                                           *
*    Include file for custom usart communication metode     *
*                                                           *
************************************************************/

/* Define to prevent recursive inclusion -----------------------------------*/
#ifndef __USART5_H
#define __USART5_H

#include "uart.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void U5_Open      (uint32_t speed,Word parity,uint16_t stopbits);    // Open the serial port for DTE comunications
extern void U5_Close     (void);                       // Close the serial port
extern uint8_t U5_Status	 (void);					   // Return a boolean indicating the status of the port, TRUE->active
extern uint8_t U5_Puts      (const char* s);              // Send a string to DTE device
extern uint8_t U5_Putc      (uint8_t b);                     // Send a char to DTE device
extern uint8_t U5_Printf    (const char *fmt,...);        // Send a formatted string to DTE device
extern uint8_t U5_Write     (const uint8_t *c, uint16_t len);    // Send a frame of LEN char
extern uint8_t U5_Getch     (void);                       // Read a char from Rx buffer
extern uint16_t U5_Read      (uint8_t *c, uint16_t len); // Read a number of bytes from Rx Buffer
extern void U5_ClearTx   (void);                       // Flush trasmission buffer
extern void U5_ClearRx   (void);                       // Flush reception buffer
extern void U5_ClearBuf  (void);                       // Flush trasmission and reception buffers
extern uint16_t U5_TxCount   (void);                       // Return the number of bytes to send in trasmission buffer
extern uint16_t U5_TxSize    (void);                       // Return the free space of trasmission buffer in bytes
extern uint8_t U5_TxEmpty   (void);                       // Return TX empty buffer state
extern uint16_t U5_RxCount   (void);                       // Return the number of bytes to read in Rx buffer
extern uint16_t U5_RxSize    (void);                       // Return the free space in Rx buffer in bytes
extern uint8_t U5_RxEmpty   (void);                       // Return RX empty buffer state

#endif
