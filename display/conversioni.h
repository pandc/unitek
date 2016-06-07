/*
 * Conversioni.h
 *
 *  Created on: Jul 17, 2013
 *      Author: User
 */

#ifndef CONVERSIONI_H_
#define CONVERSIONI_H_

void BinToBCDisp(unsigned int mybin,unsigned char mode,unsigned char start_x,unsigned char start_y);
 void BinToBCD(unsigned int mybin);
 void BinToBCDTimeDate(unsigned int mybin,unsigned char neg_digit);
 unsigned char BinToBCD_to_RTCC(unsigned int mybin);
 void PrintFakeFloat(float*speed);
 
#define NEG_DIGIT  1
#define NORM_DIGIT 0 

#endif /* CONVERSIONI_H_ */
