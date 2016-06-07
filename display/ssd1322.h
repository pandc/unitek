/*
 * mySSD1322.h
 *
 *  Created on: 29/feb/2016
 *      Author: Administrator
 */

#ifndef SOURCES_SSD1322_H_
#define SOURCES_SSD1322_H_

#define CMD_COL         	0x15
#define CMD_ROW         	0x75
#define CMD_WRITE       	0x5C
#define CMD_READ        	0x5D
#define CMD_DISPON      	0xAF
#define CMD_DISPOFF     	0xAE
#define CMD_ENGREYSCALE		0x00
#define CMD_MODE			0xA0
#define CMD_SETSTART		0xA1
#define CMD_DISPOFFSET		0xA2
#define CMD_DISPNORM		0xA6
#define CMD_DISPINVERT		0xA7
#define CMD_DISPALLON		0xA5
#define CMD_DISPALLOFF		0xA4
#define CMD_EXIT_PARTIAL	0xA9
#define CMD_VDDSEL			0xAB
#define CMD_PHASELEN		0xB1
#define CMD_SETCLKFREQ		0xB3
#define CMD_DISPENHA		0xB4
#define CMD_SETGPIO			0xB5
#define CMD_SECPRECHRG		0xB6
#define CMD_SETGRYTABLE		0xB8
#define CMD_DEFGRYTABLE		0xB9
#define CMD_PRECHRGVOL		0xBB
#define CMD_SETVCOMH		0xBE
#define CMD_CONTRSTCUR		0xC1
#define CMD_MSTCONTRST		0xC7
#define CMD_MUXRATIO		0xCA
#define CMD_DISPENHB		0xD1
#define CMD_COMLOCK			0xFD

#define DISPLAY_NORMAL		0x16//cioè capolvogendolo è leggibile
#define DISPLAY_REVERSED	0x4//cioè capolvogendolo è leggibile

#endif /* SOURCES_SSD1322_H_ */
