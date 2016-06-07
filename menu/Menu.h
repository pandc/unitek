/*
 * Menu.h
 *
 *  Created on: 11/apr/2016
 *      Author: Administrator
 */

#ifndef SOURCES_MENU_H_
#define SOURCES_MENU_H_

extern unsigned char simbolo_in_uso;
extern void (*MenuFunctionPt[30])(void);

void MenuInit(void);

#endif /* SOURCES_MENU_H_ */
