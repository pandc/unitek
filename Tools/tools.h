#ifndef __TOOLS_H
#define	__TOOLS_H

uint16_t valid_fatname(const char *s);
uint16_t strcmpNoCase(const char *str1,const char *str2);
uint16_t strmatchNoCase(const char *str,const char *match);
int16_t decode_str(char *dst,char *src);
uint16_t encode_chr(char *s,char b);
char *strupr(char *s);
char *strtrim(char *s);
int isdigits(const char *s,int n);
int isxdigits(const char *s,int n);
int convint(const char *s,int len);

uint8_t getHex(const char *ss);
long getHexLong(const char *s);
int getHexWord(const char *s,uint16_t *w,uint16_t digs);
uint8_t getNHex(const char *s,uint8_t *b,uint8_t n);
int getHexDWord(const char *s,uint32_t *dw,uint16_t digs);
int getHexByte(const char *s,uint8_t *b,uint16_t digs);

char *myinet_ntoa(uint32_t addr);
uint16_t atow(const char *s);
uint32_t atodw(const char *s);
char *strsep(char **st,char delim);
int arp_test(uint32_t ip);

int range_fits_range(uint16_t addr,uint16_t regs,uint16_t rngbase,uint16_t rngregs);
int ranges_overlap(uint16_t addr1,uint16_t regs1,uint16_t addr2,uint16_t regs2);

int get_sn(uint64_t *pdw);
int get_sn32(uint32_t *psn);

enum GetStore_Endian {
	GS_LittleEndian,
 	GS_BigEndian
};
void store_dword(uint8_t *preg,uint32_t value,int endian);
uint32_t get_dword(uint8_t *preg,int endian);
void store_word(uint8_t *preg,uint16_t value,int endian);
uint16_t get_word(uint8_t *preg,int endian);

#endif
