/************************************************************
*                                                           *
*                       usart3.h                            *
*                                                           *
*************************************************************
*                                                           *
*    Include file for custom usart communication metode     *
*                                                           *
*************************************************************
* 07.11.2008                                                *
************************************************************/

/* Define to prevent recursive inclusion -----------------------------------*/
#ifndef __USART3_H
#define __USART3_H

#include "uart.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void U3_Open      (uint32_t speed,uint16_t parity,uint16_t stopbits);    // Open the serial port for DTE comunications
extern void U3_Close     (void);                       // Close the serial port
extern void U3_Remap     (uint8_t enable);                // Enable/disable the IO remapping
extern uint8_t U3_Status	 (void);					   // Return a boolean indicating the status of the port, TRUE->active
extern uint8_t U3_Puts      (const char* s);              // Send a string to DTE device
extern uint8_t U3_Putc      (uint8_t b);                     // Send a char to DTE device
extern uint8_t U3_Printf    (const char *fmt,...);        // Send a formatted string to DTE device
extern uint8_t U3_Write     (const uint8_t *c, uint16_t len);    // Send a frame of LEN char
extern uint8_t U3_Getch     (void);                       // Read a char from Rx buffer, wait forever
extern int U3_TGetch     (uint8_t *c,uint32_t tout);         // Read a char from Rx buffer, wait for a predefined time
extern uint16_t U3_Read      (uint8_t *c, uint16_t len);          // Read a number of bytes from Rx Buffer
extern void U3_ClearTx   (void);                       // Flush trasmission buffer
extern void U3_ClearRx   (void);                       // Flush reception buffer
extern void U3_ClearBuf  (void);                       // Flush trasmission and reception buffers
extern uint16_t U3_TxCount   (void);                       // Return the number of bytes to send in trasmission buffer
extern uint16_t U3_TxSize    (void);                       // Return the free space of trasmission buffer in bytes
extern uint8_t U3_TxEmpty   (void);                       // Return TX empty buffer state
extern uint16_t U3_RxCount   (void);                       // Return the number of bytes to read in Rx buffer
extern uint16_t U3_RxSize    (void);                       // Return the free space in Rx buffer in bytes
extern uint8_t U3_RxEmpty   (void);                       // Return RX empty buffer state

#endif
