/************************************************************uint16_t
*                                                           *
*                       usart1.h                            *uint16_t
*                                                           *
*************************************************************
*                                                           *
*    Include file for custom usart communication metode     *
*                                                           *
*************************************************************/

/* Define to prevent recursive inclusion -----------------------------------*/
#ifndef __USART1_H
#define __USART1_H

#include "uart.h"

/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macro ------------------------------------------------------------*/
/* Exported functions ------------------------------------------------------- */
extern void U1_Open      (uint32_t speed,uint16_t parity,uint16_t stopbits);    // Open the serial port for DTE comunications
extern void U1_Close     (void);                       // Close the serial port
extern void U1_Remap     (uint8_t enable);                // Enable/disable the IO remapping
extern uint8_t U1_Status (void);					   // Return a boolean indicating the status of the port, TRUE->active
extern uint8_t U1_Puts      (const char* s);              // Send a string to DTE device
extern uint8_t U1_Putc      (uint8_t b);                     // Send a char to DTE device
extern uint8_t U1_Printf    (const char *fmt,...);        // Send a formatted string to DTE device
extern uint8_t U1_Write     (const uint8_t *c, uint16_t len);    // Send a frame of LEN char
extern int U1_Getch      (uint8_t *b);                       // Read a char from Rx buffer
extern int U1_TGetch     (uint8_t *c,uint32_t tout);
extern uint16_t U1_Read      (uint8_t *c, uint16_t len);          // Read a number of bytes from Rx Buffer
extern void U1_ClearTx   (void);                       // Flush trasmission buffer
extern void U1_ClearRx   (void);                       // Flush reception buffer
extern void U1_ClearBuf  (void);                       // Flush trasmission and reception buffers
extern uint16_t U1_TxCount   (void);                       // Return the number of bytes to send in trasmission buffer
extern uint16_t U1_TxSize    (void);                       // Return the free space of trasmission buffer in bytes
extern uint8_t U1_TxEmpty   (void);                       // Return TX empty buffer state
extern uint16_t U1_RxCount   (void);                       // Return the number of bytes to read in Rx buffer
extern uint16_t U1_RxSize    (void);                       // Return the free space in Rx buffer in bytes
extern uint8_t U1_RxEmpty   (void);                       // Return RX empty buffer state

#endif
