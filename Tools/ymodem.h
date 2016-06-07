#ifndef __YMODEM_H
#define	__YMODEM_H

//! @addtogroup ymodem
//! @brief	Module with the implementation of protocol ymodem to receive files from a host connected on COM channel.
//!			The file is stored inside the file system
//! @{

/**
 *  @brief Start listening for the reception of a file via ymodem protocol on COM channel
 *  
 *  @param [in] yprog  boolean, TRUE to store the received file in the filesystem,
 *                     FALSE to compare the data received with the content of the file already present if the filesystem
 *  @param [in] ycrctt boolean, TRUE to select the ccitt16 as crc algorythm for the ymodem packets (recommended)
 *  @return TRUE if the transfer file was successful
 *  
 *  @details Details
 */
int PRSR_YmodemRx(int yprog,int ycrctt);

//! @}

#endif
