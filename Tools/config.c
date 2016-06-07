/******************************************************************
* config.c: Reads configuration from file server.ini, if not file is
*           present, default configuration is read
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
#include <string.h>
#include <stdio.h>
#include <freertos.h>
#include "bsp.h"
#include "tools.h"
#include "ffile.h"

#include "config.h"

#define CONFIG_OPTION_BUFFER_SIZE 100

#define TEMP_INI_FILE	"tmp.xxx"

/* searches for an option in server.ini, which should be formated
  as follows:

  [header1]
  name1=value1
  name2=value2

  [header2]
  .
  .
  .
*/
uint8_t getConfigVoice(const char *confFile, const char * header, const char * name, char * targetBuffer,uint16_t size)
{
char *buffer = NULL,*searchValue;
FFILE *server = NULL;
uint8_t res = 0;

	if (size < 2)
		goto exi;
	--size;

	buffer = pvPortMalloc(CONFIG_OPTION_BUFFER_SIZE);
	if (!buffer)
		goto exi;

	if (!(server = ffopen(confFile,'r')))
		goto exi;

    /*search for header*/
    while (!ffeof(server))
    {
        if ((ffreadln(server,buffer,CONFIG_OPTION_BUFFER_SIZE) > 0) && (*buffer == '['))
        {
            searchValue=strtok(&buffer[1],"]");
            if (searchValue && !strcmp(header,searchValue))
                break;  //header found
        }
    }

    /*if the header is not found, eof will be reached and the
    second while loop's conditon will be false, 0 will be
    returned*/

    /*search for the name*/
    while (!ffeof(server))
    {
        if (!ffreadln(server,buffer,CONFIG_OPTION_BUFFER_SIZE))
			continue;
        if (*buffer == '[')
            break;  //the start of next header field, stop search
        searchValue = strtok(buffer,"= \t");
        if (searchValue && (strlen(searchValue)==strlen(name)))
        {
            if (!strncmp(searchValue,name,strlen(name)) )//name found
            {
                searchValue=strtok(NULL," =\r\n\t");  //extract the value
                if (searchValue)
				{
					if (size > strlen(searchValue))
						size = strlen(searchValue);
					memcpy(targetBuffer,searchValue,size);
					targetBuffer[size] = 0;
				}
                else
					*targetBuffer = 0;
				res = TRUE;
                break;
            }
        }
    }
exi:
	if (server)
		ffclose(server);
	if (buffer)
		vPortFree(buffer);
    return res;
}

uint8_t setConfigMultiVoice(const char *confFile,const char *header,const char **tag, const char **value,uint16_t n)
{
char *buffer = NULL;
uint8_t headerfound,modified;
int16_t nr;
FFILE *fin = NULL,*fout = NULL;
int16_t i=0;

	modified = headerfound = 0;

	if (!(fin=ffopen(confFile,'r')))
		goto exi;

	if (!(fout=ffopen(TEMP_INI_FILE,'w')))
		goto exi;

	buffer = pvPortMalloc(CONFIG_OPTION_BUFFER_SIZE);
	if (!buffer)
		goto exi;

	if (header && (*header != 0) && (*header != '*'))
	{
		/*search for header*/
		while (!ffeof(fin))
		{
			if (!ffreadln(fin,buffer,CONFIG_OPTION_BUFFER_SIZE) || (*buffer <= ' '))
				continue;
			strtrim(buffer);
			if (*buffer=='[')
			{
				nr = strlen(header);
				if ((strlen(buffer) == (nr+2)) && !strncmp(header,buffer+1,nr) &&
					(buffer[nr+1] == ']'))
				{
					headerfound = TRUE;
					if (n)
					{
						ffputs(fout,buffer);
						ffputs(fout,"\r\n");
					}
					else
					{
						modified = TRUE;	// header section deleted
						if (tag[0])		// exception to empty the section instead of a complete deletion
						{
							ffputs(fout,buffer);
							ffputs(fout,"\r\n");
						}
						while (!ffeof(fin))		// remove all the lines till the next section or till the end of the file
						{
							if (!ffreadln(fin,buffer,CONFIG_OPTION_BUFFER_SIZE) || (*buffer <= ' '))
								continue;
							strtrim(buffer);
							if (*buffer=='[')
							{
								ffputs(fout,buffer);
								ffputs(fout,"\r\n");
								break;
							}
						}
					}
					break;  //header found
				}
			}
			ffputs(fout,buffer);
			ffputs(fout,"\r\n");
		}
		if (!headerfound && !n)
		{
			// add new section
			sprintf(buffer,"[%s]\r\n",header);
			ffputs(fout,buffer);
			modified = headerfound = TRUE;
		}
    }

	while (!ffeof(fin))
	{
		if (!ffreadln(fin,buffer,CONFIG_OPTION_BUFFER_SIZE) || (*buffer <= ' '))
			continue;
		strtrim(buffer);
		if (header && (*header != 0) && (*header != '*'))
		{
			if (buffer[0]=='[')
			{
				// Session completed
				// Go to add new voices
				for (i = 0; i < n; i++)
				{
					if (tag[i] && value[i])
					{
						ffputs(fout,tag[i]);
						ffput(fout,'=');
						ffputs(fout,value[i]);
						ffputs(fout,"\r\n");
						modified = TRUE;
					}
					tag[i] = NULL;
				}
				headerfound = 0;
				ffputs(fout,buffer);
				ffputs(fout,"\r\n");
				break;  //the start of next header field, stop search
			}
		}
		for (i=0; i<n; i++)
		{
			if (tag[i])
			{
				nr = strlen(tag[i]);
				if ((buffer[nr] == '=') && !strncmp(buffer, tag[i], nr))
				{
					if (value[i])
					{
						ffputs(fout, tag[i]);
						ffput(fout, '=');
						ffputs(fout, value[i]);
						ffputs(fout, "\r\n");
					}
					modified = TRUE;
					tag[i] = NULL;
					break;
				}
			}
		}
		if (i>=n)
		{
			ffputs(fout, buffer);
			ffputs(fout,"\r\n");
		}
	}
	while (!ffeof(fin))
	{
		if (ffreadln(fin,buffer,CONFIG_OPTION_BUFFER_SIZE))
		{
			strtrim(buffer);
			ffputs(fout, buffer);
			ffputs(fout,"\r\n");
		}
	}

	if (headerfound)
	{
		// Session completed
		// Go to add new voices
		for (i = 0; i < n; i++)
		{
			if (tag[i] && value[i])
			{
				ffputs(fout,tag[i]);
				ffput(fout,'=');
				ffputs(fout,value[i]);
				ffputs(fout,"\r\n");
				modified = TRUE;
			}
			tag[i] = NULL;
		}
	}

exi:
	if (fin)
		ffclose(fin);
	if (fout)
		ffclose(fout);
	if (buffer)
		vPortFree(buffer);
	if (modified)
	{
		ffdelete(confFile);
		ffmove(TEMP_INI_FILE,confFile);
	}
	else
		ffdelete(TEMP_INI_FILE);
	for (i = 0; i < n; i++)
		if (tag[i])
			return FALSE;
	return TRUE;
}

uint8_t setConfigVoice(const char *confFile,const char *header,const char *name, const char *targetBuffer)
{
	return setConfigMultiVoice(confFile,header,&name,&targetBuffer,1);
}

uint8_t deleteConfigSection(const char *confFile,const char *header)
{
const char *name = NULL,*targetBuffer = NULL;

	return setConfigMultiVoice(confFile,header,&name,&targetBuffer,0);
}

uint8_t emptyConfigSection(const char *confFile,const char *header)
{
const char *name = header,*targetBuffer = NULL;

	return setConfigMultiVoice(confFile,header,&name,&targetBuffer,0);
}
