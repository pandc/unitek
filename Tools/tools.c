#include <string.h>
#include <ctype.h>
#include "bsp.h"
#include "flash.h"
#include "tools.h"

uint16_t valid_fatname(const char *s)
{
int16_t i,e;

	for (e = -1,i = -1; *s; s++)
	{
		if (*s == '/')
		{
			if (i == 0)
				return FALSE;
			i = 0;
			e = -1;
		}
		else if (*s == '.')
		{
			if ((i < 1) || (i > 8) || (e >= 0))
				return FALSE;
			e = 0;
		}
		else if (isalnum(*s) || (*s == '-') || (*s == '_') || (*s == '~'))
		{
			if (e >= 0)
			{
				if (++e > 3)
					return FALSE;
			}
			else
			{
				if (i < 0) i = 0;
				if ((i == 0) && !isalnum(*s))
					return FALSE;
				if (++i > 11)
					return FALSE;
			}
		}
		else
			return FALSE;
	}
	return TRUE;
}

uint16_t strcmpNoCase(const char *str1,const char *str2)
{
    for ( ;toupper(*str1) == toupper(*str2) ; str1++,str2++)
    {
        if (*str1 == 0)
            return TRUE;
    }
    return FALSE;
}

uint16_t strmatchNoCase(const char *str,const char *match)
{
    for ( ; *match; match++,str++)
    {
        if (toupper(*str) != toupper(*match))
            return FALSE;
    }
    return TRUE;
}

int16_t decode_str(char *dst,char *src)
{
uint8_t escape = FALSE;
int16_t len = 0;

	for (; *src; src++)
	{
		if (escape)
		{
			if (*src == '!')
				dst[len++] = '!';
			else if ((*src >= '@') && (*src <= '_'))
				dst[len++] = *src + 0x40;
			else
				return -1;
			escape = FALSE;
		}
		else if ((*src >= 0x80) && (*src < 0xa0))
			dst[len++] = *src & 0x7f;
		else if (*src == '!')
			escape = TRUE;
		else
			dst[len++] = *src;
	}

//	for (; *src; src++)
//	{
//		if (escape)
//		{
//			if (*src == '!')
//				dst[len++] = '!';
//			else if ((*src >= '@') && (*src <= '_'))
//				dst[len++] = *src - '@';
//			else
//				return -1;
//			escape = FALSE;
//		}
//		else if (*src == '!')
//			escape = TRUE;
//		else
//			dst[len++] = *src;
//	}
	return len;
}

uint16_t encode_chr(char *s,char b)
{
uint16_t i = 0;

	if (b < ' ')
		s[i++] = b + 0x80;
	else if ((b >= 0x80) && (b < 0xa0))
	{
		s[i++] = '!';
		s[i++] = b - 0x40;
	}
	else if (b == '!')
	{
		s[i++] = '!';
		s[i++] = '!';
	}
	else
		s[i++] = b;

//	if (b >= ' ')
//	{
//		if (b == '!')
//		{
//			s[i++] = '!';
//			s[i++] = '!';
//		}
//		else
//			s[i++] = b;
//	}
//	else
//	{
//		s[i++] = '!';
//		s[i++] = b + '@';
//	}
	s[i] = 0;
	return i;
}

uint8_t getHex(const char *s)
{
uint8_t uc,b;

	b = toupper(*s);
	uc = b - ((b >= 'A')? ('A'-10): '0') << 4;
	b = toupper(s[1]);
	uc |= b - ((b >= 'A')? ('A'-10): '0');
	return uc;
}

long getHexLong(const char *s)
{
long l;

	if (!*s)
		return -1;
	for (l = 0; *s; s++)
	{
		l <<= 4;
		if (isdigit(*s))
			l |= *s - '0';
		else if (isxdigit(*s))
			l |= toupper(*s) - 'A' + 10;
		else
			return -1;
	}
	return l;
}

int getHexByte(const char *s,uint8_t *b,uint16_t digs)
{
	if (!digs) digs = strlen(s);
	for (*b = 0; digs; digs--,s++)
	{
		*b <<= 4;
		if (isdigit(*s))
			*b |= *s - '0';
		else if (isxdigit(*s))
			*b |= toupper(*s)-'A'+10;
		else
			return FALSE;
	}
	return TRUE;
}

int getHexDWord(const char *s,uint32_t *dw,uint16_t digs)
{
	if (!digs) digs = strlen(s);
	for (*dw = 0; digs; digs--,s++)
	{
		*dw <<= 4;
		if (isdigit(*s))
			*dw |= *s - '0';
		else if (isxdigit(*s))
			*dw |= toupper(*s)-'A'+10;
		else
			return FALSE;
	}
	return TRUE;
}

int getHexWord(const char *s,uint16_t *w,uint16_t digs)
{
	if (!digs) digs = strlen(s);
	for (*w = 0; digs; digs--,s++)
	{
		*w <<= 4;
		if (isdigit(*s))
			*w |= *s - '0';
		else if (isxdigit(*s))
			*w |= toupper(*s) - 'A' + 10;
		else
			return FALSE;
	}
	return TRUE;
}

uint8_t getNHex(const char *s,uint8_t *b,uint8_t n)
{
uint8_t	i;

	for (i = 0; i < n; i++)
	{
		if (!isxdigit(*s) || !isxdigit(*s))
			break;

		b[i] = getHex(s);
		s += 2;
	}
	return i;
}

char *strupr(char *s)
{
	for (; *s; s++)
		*s = toupper(*s);
	return s;
}

char *strtrim(char *s)
{
char *sw,*sr;
int i;

	for (sw = sr = s; *sr && isspace(*sr); sr++);
	for (; *sr; sr++,sw++)
		*sw = *sr;
	*sw = 0;
	for (i = strlen(s)-1; i >= 0; i--)
		if (isspace(s[i]))
			s[i] = 0;
		else
			break;
	return s;
}

int isdigits(const char *s,int n)
{
int i;

	if (n <= 0)
		n = strlen(s);
	for (i = 0; i < n; i++)
		if (!isdigit(s[i]))
			return FALSE;
	return TRUE;
}

int isxdigits(const char *s,int n)
{
int i;

	if (n <= 0)
		n = strlen(s);
	for (i = 0; i < n; i++)
		if (!isxdigit(s[i]))
			return FALSE;
	return TRUE;
}

int convint(const char *s,int len)
{
int n,i;

	if (!isdigits(s,len))
		return -1;
	for (n = i = 0; i < len; i++)
		n = (n * 10) + s[i]-'0';
	return n;
}

uint16_t atow(const char *s)
{
uint16_t w;

	for (w = 0; isdigit(*s); s++)
	{
		w *= 10;
		w += *s - '0';
	}
	return w;
}

uint32_t atodw(const char *s)
{
uint32_t dw;

	for (dw = 0; isdigit(*s); s++)
	{
		dw *= 10;
		dw += *s - '0';
	}
	return dw;
}

char *strsep(char **st,char delim)
{
char *s,*s1;

	if (!st || !*st)
		return NULL;
	s = s1 = *st;
	s1 = strchr(s,delim);
	if (s1)
	{
		*s1 = 0;
		*st = s1+1;
	}
	else
		*st = NULL;
	return s;
}

int range_fits_range(uint16_t addr,uint16_t regs,uint16_t rngbase,uint16_t rngregs)
{
	return ((addr >= rngbase) && ((addr + regs) <= (rngbase+rngregs)));
}

int ranges_overlap(uint16_t addr1,uint16_t regs1,uint16_t addr2,uint16_t regs2)
{
	if (!regs1 || !regs2)
		return 0;
	if ((addr1 >= addr2) && (addr1 < (addr2+regs2)))
		return 1;
	if ((addr2 >= addr1) && (addr2 < (addr1+regs1)))
		return 1;
	return 0;
}

int get_sn(uint64_t *pdw)
{
uint32_t pf;

	*pdw = 0;
	pf = SerialNumberAddress;
	if (*((uint64_t *)pf) == ~(((uint64_t *)pf)[1]))
	{
		*pdw = *((uint64_t *)pf);
		return TRUE;
	}
	else
		return FALSE;
}

int get_sn32(uint32_t *psn)
{
uint64_t qw;

	*psn = 0;
	if (get_sn(&qw))
	{
		*psn = (uint32_t)qw;
		return TRUE;
	}
	else
		return FALSE;
}

void store_dword(uint8_t *preg,uint32_t value,int endian)
{
int i;

	if (endian == GS_BigEndian)
	{
		for (i = 3; i >= 0; i--)
		{
			preg[i] = value & 0xff;
			value >>= 8;
		}
	}
	else
	{
		for (i = 0; i < 4; i++)
		{
			preg[i] = value & 0xff;
			value >>= 8;
		}
	}
}

uint32_t get_dword(uint8_t *preg,int endian)
{
int i;
uint32_t dw = 0;

	if (endian == GS_BigEndian)
	{
		for (i = 0; i < 4; i++)
		{
			dw <<= 8;
			dw += preg[i];
		}
	}
	else
	{
		for (i = 3; i >= 0; i--)
		{
			dw <<= 8;
			dw += preg[i];
		}
	}
	return dw;
}

void store_word(uint8_t *preg,uint16_t value,int endian)
{
	if (endian == GS_BigEndian)
	{
		*preg = value >> 8;
		preg[1] = (uint8_t)value;
	}
	else
	{
		*preg = (uint8_t)value;
		preg[1] = value >> 8;
	}
}

uint16_t get_word(uint8_t *preg,int endian)
{
uint16_t w;

	if (endian == GS_BigEndian)
	{
		w = *preg;
		w <<= 8;
		w += preg[1];
	}
	else
	{
		w = preg[1];
		w <<= 8;
		w += *preg;
	}
	return w;
}
