#ifndef __CRC16_H
#define __CRC16_H

//! @addtogroup crc16
//! @brief Module containing the implementation of crc16 algorythm, used by ymodem protocol
//! @{

/**
 *  @brief Calculation of crc16 byte per byte
 *  
 *  @param [in] fcs previous crc value
 *  @param [in] c   new byte to compute in the crc calculation
 *  @return new crc16 value
 */
uint16_t crc16(uint16_t fcs, uint8_t c);
/**
 *  @brief Calculation of crc16 on a block of bytes
 *  
 *  @param [in] pb  pointer to the first byte of the pattern object of the calculation
 *  @param [in] len number of bytes to include in the calculation
 *  @return the crc16 value calculated
 */
uint16_t crc16_calc(uint8_t *pb,uint16_t len);

//! @}

#endif
