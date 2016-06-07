#ifndef __KEYBOARD_H
#define __KEYBOARD_H

#define KEY_UPLEFT		'U'
#define KEY_PLUS		'+'
#define KEY_MINUS		'-'
#define KEY_OK			'K'
#define KEY_DOWNRIGHT	'D'
#define KEY_PROG		'P'

#define KEY_RELEASED	0x80

void keyboard_init(void);
int key_getstroke(uint8_t *key,uint32_t tout);
int key_putstroke(uint8_t stroke,uint32_t tout);

#endif
