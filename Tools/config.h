/******************************************************************
* config.h:
* 
* Copyright (c) 2001 Atmel Corporation.
* All Rights Reserved.
*
* You are autorized to use, copy and distribute this software only at 
* a single site (the term "site" meaning a single company location). 
* This copyright notice must be included in any copy, modification 
* or portion of this software merged into another program.
* 
* This software is licenced solely for use with Atmel AVR micro 
* controller family. The software may not be modified to execute on 
* any other microcontroller architectures
*
* This software is provided "as is"; Without warranties either express
* or implied, including any warranty regarding merchantability, 
* fitness for a particular purpose or noninfringement. 
*
* In no event shall Atmel or its suppliers be liable for any special, 
* indirect,incidential or concequential damages resulting from the 
* use or inability to use this software.
*
* Revision history:
*
* January 17, 2001:   Version 1.0   Created by JB
* July 13, 2001:      Version 1.2   JB
*                                   - Changed to IAR compiler V2.25
*                                   - Renamed flash file functions to avoid 
*                                     conflict with standard file I/O names	
*                                   - Bug fixes in HTTP
*                                   - Speed optimization in TCP 
* January 24, 2002    version 1.3   JB & JEB
*                                   - Merged IAR and ImageCraft codebases
*                                   - Modifications to allow use of the AVR's 
*                                     internal dataflash (see readme)
*                                   - Changed from interrupts to polling CS8900A
*                                   - Bug fixes to UDP, DHCP and SMTP
*
*
*******************************************************************/

#ifndef __CONFIG_H
#define __CONFIG_H

#define CONF_FILE		"config.ini"

/*get an option from server.ini, returns number of char copied to targetBuffer*/
uint8_t setConfigMultiVoice(const char *confFile,const char *header,const char **tag, const char **value,uint16_t n);
uint8_t setConfigVoice(const char *confFile,const char *header,const char *name, const char *targetBuffer);
uint8_t getConfigVoice(const char *confFile, const char * header, const char * name, char * targetBuffer,uint16_t size);
uint8_t deleteConfigSection(const char *confFile,const char *header);
uint8_t emptyConfigSection(const char *confFile,const char *header);

#endif
