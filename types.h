#ifndef __TYPES_H
#define	__TYPES_H

#include <stdint.h>
//#include <stdbool.h>

// typedef unsigned long long QWord;
// typedef unsigned long  DWord;
// typedef unsigned short Word;
// typedef unsigned char  Byte;

// typedef unsigned char uint8_t;
// typedef unsigned short uint16_t;
// typedef unsigned long uint32_t;
// typedef unsigned long long uint64_t;

// typedef signed char int8;
// typedef	signed short int16;
// typedef signed long int32;
// typedef signed long long int64;

#define XBYTE(a)	*((uint8_t *)(a))
#define XWORD(a)	*((uint16_t *)(a))
#define XDWORD(a)	*((uint32_t *)(a))
#define XQWORD(a)	*((uint64_t *)(a))

#ifndef False
#define	False	0
#endif

#ifndef FALSE
#define	FALSE	0
#endif

#ifndef True
#define	True	1
#endif

#ifndef TRUE
#define	TRUE	1
#endif

#define QUOTEME_(x) #x
#define QUOTEME(x) QUOTEME_(x)

#include "freertos.h"

#define kSec		configTICK_RATE_HZ
#define kHalfSec	(kSec/2)
#define kDec		(kSec/10)
#define kCen		(kDec/10)
#define kMin		(kSec*60)

#endif
