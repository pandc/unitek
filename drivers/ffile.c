#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "bsp.h"
#include "io.h"
#include "freertos.h"
#include "task.h"
#include "semphr.h"
#include "tools.h"
#include "irqprio.h"
#include "eeprom.h"
#include "eepmap.h"

#include "debug.h"
#include "com.h"

#include "ffile.h"

#define FILE_closed     0
#define FILE_openread   1
#define FILE_openwrite  2

#define FILE_MAX_OPEN_FILES  4
#define FILE_UNFILE_LOCKED   0
#define FILE_LOCKED          1
#define	FILE_LOCKED_COMPLETE 2

#define	FAT_ATTR_READ	(1 << 0)
#define	FAT_ATTR_TOSEND	(1 << 1)
#define	FAT_ATTR_SENT	(1 << 2)

#define MIN_DELAY()		vTaskDelay(1)

#define MAP_SET_PAGE(p)		map[(p) >> 3] |= 1 << ((p) & 7)
#define MAP_BUSY_PAGE(p)	((map[(p) >> 3] & (1 << ((p) & 7))) != 0)

uint8_t * StartFileFlag;

static xSemaphoreHandle xSemaphore_Work,xFsMutex;
static xQueueHandle xSema_Ready;
static volatile uint8_t spi_rxdata;

static FFILE file[FILE_MAX_OPEN_FILES];
static DIR dirdata;

static struct FATDATA {
	uint16_t fat_buffermask;
	uint16_t fat_pagemask;
	uint8_t fat_pageleftshift;
	uint16_t fat_pagedata;
	uint16_t fat_pagelength;
	uint8_t fat_writing;
} fd;

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//--- SPI micro dependant routines -------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

static inline void DF_ENABLE(void)
{
	GPIO_ResetBits(DF_SELn_PORT,DF_SELn_PIN);
}

static inline void DF_DISABLE(void)
{
	GPIO_SetBits(DF_SELn_PORT,DF_SELn_PIN);
}

void SPI1_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (SPI_I2S_GetITStatus(SPI1, SPI_I2S_IT_RXNE) == SET)
	{
		spi_rxdata = SPI_I2S_ReceiveData(SPI1);
        /* Disable the Tx buffer empty interrupt */
		xSemaphoreGiveFromISR(xSemaphore_Work,&xHigherPriorityTaskWoken);
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

// Initialization
static void SSC_vInit(void)
{
static uint8_t init;
SPI_InitTypeDef   SPI_InitStructure;

	if (!init)
	{
		init = 1;

		GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable,ENABLE);
		SPI_I2S_DeInit(SPI1);

		/* GPIOE and GPIOB Periph clock enable */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE | RCC_APB2Periph_GPIOB | RCC_APB2Periph_AFIO, ENABLE);

		/* SPI1 Periph clock enable */
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

		GPIO_PinRemapConfig(GPIO_Remap_SPI1, ENABLE);	// PB3-SPI1_SCK PB4-SPI1_MISO PB5-SPI1_MOSI

		GpioInit(DF_SCK_PORT,DF_SCK_PIN,GPIO_Mode_AF_PP,0);
		GpioInit(DF_MISO_PORT,DF_MISO_PIN,GPIO_Mode_AF_PP,0);
		GpioInit(DF_MOSI_PORT,DF_MOSI_PIN,GPIO_Mode_AF_PP,0);

		//GpioInit(SD_SELn_PORT,SD_SELn_PIN,GPIO_Mode_Out_PP,1);
		GpioInit(DF_SELn_PORT,DF_SELn_PIN,GPIO_Mode_Out_PP,1);

		/* SPI1 Config */
		SPI_StructInit(&SPI_InitStructure);
		SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
		SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
		SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
		SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
		SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
		SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
		SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
		SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
		SPI_InitStructure.SPI_CRCPolynomial = 7;
		SPI_Init(SPI1, &SPI_InitStructure);

		/* SPI1 enable */
		SPI_Cmd(SPI1, ENABLE);

		//vSemaphoreCreateBinary(xSemaphore_Work);
		//xSemaphoreTake(xSemaphore_Work,0);		// clear it

		//spi_rxdata = SPI_I2S_ReceiveData(SPI1);
		//SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
		//NVIC_SetPriority(SPI1_IRQn,SPI1_IRQ_PRIO);
		//NVIC_EnableIRQ(SPI1_IRQn);
	}
}

#if 0
uint8_t write_SPI(uint8_t data)
{
	while (xSemaphoreTake(xSemaphore_Work,0));	// empty the semaphore

	/*!< Send byte through the SPI1 peripheral */
	SPI_I2S_SendData(SPI1, data);

	/*!< Wait to receive a byte */
	xSemaphoreTake(xSemaphore_Work,portMAX_DELAY);

	/*!< Return the byte read from the SPI bus */
	return spi_rxdata;
}
#endif
static uint8_t write_SPI(uint8_t data)
{
	SPI_I2S_SendData(SPI1, data);
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	data = SPI_I2S_ReceiveData(SPI1);
	return data;
}

//----------------------------------------------------------------------------
//----------------------------------------------------------------------------
//----------------------------------------------------------------------------

typedef struct {
	uint16_t flags;
	uint16_t nextpage;
} FATPGPARAMST;

#define	FAT_FILE_SIZE_OFFSET		0
#define	FAT_FILE_ATTR_OFFSET		(FAT_FILE_SIZE_OFFSET+sizeof(uint32_t))
#define	FAT_FILE_NAME_OFFSET		(FAT_FILE_ATTR_OFFSET+sizeof(uint8_t))
#define FAT_FILE_LASTPAGE_OFFSET	(FAT_FILE_NAME_OFFSET+FAT_FILE_NAME_LEN+1)
typedef struct {
	uint32_t size;
	uint8_t attr;
	uint8_t name[FAT_FILE_NAME_LEN+1];
	uint16_t lastpage;
} FATFILEHEADERST;

#define	FATPG_PageBusy	(1 << 0)
#define	FATPG_FileName	(1 << 1)
#define	FATPG_LastPage	(1 << 2)

//#define	FATPG_FLAGS(a)	((uint16_t)((uint16_t)(a) ^ (uint16_t)0xffff))
#define	FATPG_FLAGS(a)	((uint16_t)(~(a)))

#define	FATPG_NoPage	0xffff

typedef struct {
	uint16_t firstpage;
	uint16_t lastpage;
	uint32_t freepages;
	uint16_t firstfreepage;
	uint16_t lastupdatedpage;
	uint16_t writtenpages;
} FATST;

static FATST fat;

#define B1  0
#define B2  1

int ffready(uint32_t tout)
{
uint8_t b;

	return (xSema_Ready && xQueuePeek(xSema_Ready,&b,tout))? TRUE: FALSE;
}

static uint8_t read_status(void)
{
uint8_t b;

	DF_ENABLE();
	write_SPI(0xd7);
	b = write_SPI(0);
	DF_DISABLE();
	return b;
}

static void write_SPI_page_buffer(uint16_t page_addr,uint16_t buffer_addr)
{
	page_addr &= fd.fat_pagemask;
	buffer_addr &= fd.fat_buffermask;
	write_SPI(page_addr >> (8-fd.fat_pageleftshift));
	write_SPI((page_addr << fd.fat_pageleftshift) | (buffer_addr >> 8));
	write_SPI((uint8_t)buffer_addr);
}

static uint16_t readPageParam(uint16_t page,void *dest)
{
uint16_t i;

	DF_ENABLE();
	write_SPI(0xd2);	// Read main memory page
	write_SPI_page_buffer(page,fd.fat_pagedata);
	write_SPI(0);	// don't care
	write_SPI(0);	// don't care
	write_SPI(0);	// don't care
	write_SPI(0);	// don't care
	for (i = 0; i < sizeof(FATPGPARAMST); i++)
		((uint8_t *)dest)[i] = write_SPI(0);
	DF_DISABLE();
	MIN_DELAY();
	return TRUE;
}

static uint16_t readPagePartial(uint16_t page,uint16_t offset,void *dest,uint16_t len)
{
uint16_t i;

	DF_ENABLE();
	write_SPI(0xd2);	// Read main memory page
	write_SPI_page_buffer(page,offset);
	write_SPI(0);	// don't care
	write_SPI(0);	// don't care
	write_SPI(0);	// don't care
	write_SPI(0);	// don't care
	for (i = 0; i < len; i++)
		((uint8_t *)dest)[i] = write_SPI(0);
	DF_DISABLE();
	MIN_DELAY();
	return TRUE;
}	

static void erasePage(uint16_t page)
{
	DF_ENABLE();
	write_SPI(0x81);	// Page erase
	write_SPI_page_buffer(page,0);
	DF_DISABLE();
	while ((read_status() & 0x80) == 0)
		MIN_DELAY();
}

static void autoPageRewrite(uint16_t bufnum,uint16_t page)
{
	DF_ENABLE();
	write_SPI(0x58+bufnum);	// Auto Page Rewrite
	write_SPI_page_buffer(page,0);
	DF_DISABLE();
	while ((read_status() & 0x80) == 0)
		MIN_DELAY();
}

// return TRUE if page is equal to buffer
static uint16_t comparePage(uint16_t bufnum,uint16_t page)
{
uint16_t st;

	DF_ENABLE();
	write_SPI(0x60+bufnum);	// Compare Buffer 1 with Main Memory Page
	write_SPI_page_buffer(page,0);
	DF_DISABLE();
	while (((st = read_status()) & 0x80) == 0)
		MIN_DELAY();
	return ((st & 0x40) == 0);
}

static void setBuffer(uint16_t bufnum,uint8_t val)
{
uint16_t i;

	DF_ENABLE();
	write_SPI(0x84 + (bufnum*3));
	write_SPI(0);
	write_SPI(0);
	write_SPI(0);
	for (i = 0; i < fd.fat_pagelength; i++)
		write_SPI(val);
	DF_DISABLE();
	MIN_DELAY();
}

static void writeBuffer(uint16_t bufnum,uint16_t offset,void *src,uint16_t len)
{
uint16_t i;

	DF_ENABLE();
	write_SPI(0x84 + (bufnum*3));
	write_SPI_page_buffer(0,offset);
	for (i = 0; i < len; i++)
		write_SPI(((uint8_t *)src)[i]);
	DF_DISABLE();
	MIN_DELAY();
}

static uint16_t readBuffer(uint16_t bufnum,uint16_t offset,void *dest,uint16_t len)
{
uint16_t i;

	DF_ENABLE();
	write_SPI(0xd4 + (bufnum*2));
	write_SPI_page_buffer(0,offset);
	write_SPI(0);
	for (i = 0; i < len; i++)
		((uint8_t *)dest)[i] = write_SPI(0);
	DF_DISABLE();
	MIN_DELAY();
	return TRUE;
}

static void mem2buffer(uint16_t bufnum,uint16_t page)
{
	DF_ENABLE();
	write_SPI(0x53 + (bufnum*2));
	write_SPI_page_buffer(page,0);
	DF_DISABLE();
	while ((read_status() & 0x80) == 0)
		MIN_DELAY();
}

static void buffer2mem(uint16_t bufnum,uint16_t page)
{
	DF_ENABLE();
	write_SPI(0x88+bufnum);
	write_SPI_page_buffer(page,0);
	DF_DISABLE();
	while ((read_status() & 0x80) == 0)
		MIN_DELAY();
	fat.writtenpages++;
}

//--------------------------------------------------------------------------------------
//---- Private functions ---------------------------------------------------------------
//--------------------------------------------------------------------------------------

#define	TEST_FALSE(w,f)	((w & f) != 0)
#define	TEST_TRUE(w,f)	((w & f) == 0)

void ffsync(void)
{
	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if ((fat.lastpage == FATPG_NoPage) || (fat.lastpage == 0))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return;
	}

	if (fd.fat_writing)
		ffcloseall();

	if (!fat.writtenpages)
		return;

	for ( ;fat.writtenpages; fat.writtenpages--)
	{
		WDOG_Refresh();
		autoPageRewrite(B2,fat.lastupdatedpage);
		if (fat.lastupdatedpage == 0)
		{
			fat.lastupdatedpage = fat.lastpage;	// we have to update every page
												// even those not managed by the file system
		}
		else
			fat.lastupdatedpage--;
	}
	EEP_Write(EEP_fsfirtfree_addr,&fat.firstfreepage,2);
	EEP_Write(EEP_fslastupage_addr,&fat.lastupdatedpage,2);
    setBuffer(B2,0xff);     // reset buffer 2 to its default content (it's used for page blank checks)
	fat.lastpage = 0;		// inihibit any other writing
}

void ffrefresh(void)
{
	if (!ffready(0) || (fat.lastpage == FATPG_NoPage) || (fat.lastpage == 0))
		return;

	if (fat.writtenpages && !fd.fat_writing)
	{
		xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
		autoPageRewrite(B2,fat.lastupdatedpage);
		setBuffer(B2,0xff);		// reset buffer 2 to its default content (it's used for page blank checks)
		if (fat.lastupdatedpage == 0)
			fat.lastupdatedpage = fat.lastpage;
		else
			fat.lastupdatedpage--;
		if (--fat.writtenpages == 0)
		{
			EEP_Write(EEP_fslastupage_addr,&fat.lastupdatedpage,2);
			EEP_Write(EEP_fsfirtfree_addr,&fat.firstfreepage,2);
		}
		xSemaphoreGiveRecursive(xFsMutex);
	}
}

static uint16_t FAT_getFreePage(void)
{
uint16_t ffp;

	if (fat.freepages == 0)
		return FATPG_NoPage;

	ffp = fat.firstfreepage;
	if (ffp != FATPG_NoPage)
	{
		for (;;)
		{
			if (++ffp > fat.lastpage)
				ffp = fat.firstpage;
			if (ffp == fat.firstfreepage)
			{
				ffp = FATPG_NoPage;
				break;
			}
			if (comparePage(B2,ffp))
				break;
		}
		fat.freepages--;
	}
	fat.firstfreepage = ffp;
	return ffp;
}

static void SetStartFileFlag( uint16_t i )
{
	StartFileFlag[i >> 3] |= 1 << (i & 7);
}

static void RstStartFileFlag( uint16_t i )
{
	StartFileFlag[i >> 3] &= ~(1 << (i & 7));
}

//--------------------------------------------------------------------------------------
//---- Public functions ----------------------------------------------------------------
//--------------------------------------------------------------------------------------

/*format the file system and write a Media Status Table to EEPROM*/
int16_t fformat(void)
{
uint16_t	i;

	if ((fat.lastpage == FATPG_NoPage) || (fat.lastpage == 0))
		return -1;	// DataFlash Missing

	if (fat.firstfreepage == FATPG_NoPage)
		fat.firstfreepage = fat.firstpage;

	for (i = 0; i < FILE_MAX_OPEN_FILES; i++)
		file[i].state = FILE_closed;

	fd.fat_writing = 0;
	setBuffer(B1,0xff);
	setBuffer(B2,0xff);
	fat.freepages = fat.lastpage - fat.firstpage + 1;
	for (i = fat.firstpage; i <= fat.lastpage; i++)
	{
		if (!comparePage(B2,i))
		{
			erasePage(i);

			RstStartFileFlag( i );

			++fat.writtenpages;
		}
	}
	ffsync();
	return 0;
}

static void destroyBadFile(uint16_t firstp,uint16_t lastp)
{
FATPGPARAMST par;

	do {
		if (firstp > fat.lastpage)
			return;
		readPageParam(firstp,&par);
		erasePage(firstp);
		++fat.writtenpages;

		if (firstp == lastp)
			return;

		firstp = par.nextpage;
	} while (1);
}

static uint8_t checkFileName(FATFILEHEADERST *fh)
{
uint8_t c;
uint16_t i;

	//readPagePartial(np,0,&fhdr,sizeof(fhdr));
	for (i = 0; (i < FAT_FILE_NAME_LEN) && fh->name[i]; i++)
	{
		c = fh->name[i];
		if ((c < ' ') || (c > 0x7e) || (c == ','))
			break;
	}
	if ((i == 0) || fh->name[i])
		return FALSE;
	else
		return TRUE;
}

uint8_t searchStartFilePageIdx(uint16_t *ppage)
{
uint16_t page,i,j;

	for (page = *ppage; page <= fat.lastpage; )
	{
		if(StartFileFlag[page >> 3])
		{
			for (j = page >> 3, i = page & 7; i < 8 ; i++,page++)
			{
				if(StartFileFlag[j] & (1 << i))
				{
                    *ppage = page;
					return TRUE;
				}
			}
		}
        else
            page = (page + 8) & ~7;
	}
    *ppage = page;
	return FALSE;
}

static int16_t ffinit(void)
{
FATFILEHEADERST fhdr;
FATPGPARAMST par;
uint16_t i,j;
uint8_t err,*map;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	fat.lastpage = FATPG_NoPage;
	SSC_vInit();

	for (i = 0; i < 3; i++)
	{
		read_status();
		MIN_DELAY();
	}

	for (i = 0; i < 100; i++)
	{
		j = read_status();
		if ((j & 0x83) == 0x80)
			break;
		MIN_DELAY();
	}
	if ((j & 0x80) == 0)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return -101;	// No connection to DataFlash
	}

	switch(j & 0x38)
	{
	//case 0x08:		// XX001XXX - AT45DB011B
	//	fat.lastpage = 512-1;
	//	break;
	case 0x10:		// XX010XXX - AT45DB021B
		fat.lastpage = 1024-1;
		fd.fat_pagedata = 0x100;
		fd.fat_pagelength = 0x108;
		break;
	case 0x18:		// XX011XXX - AT45DB041B
		fat.lastpage = 2048-1;
		fd.fat_pagedata = 0x100;
		fd.fat_pagelength = 0x108;
		break;
	case 0x20:		// XX100XXX - AT45DB081B
		fat.lastpage = 4096-1;
		fd.fat_pagedata = 0x100;
		fd.fat_pagelength = 0x108;
		break;
	case 0x28:	// XX101XXX - AT45DB161B
		fat.lastpage = 4096-1;
		fd.fat_pagedata = 0x200;
		fd.fat_pagelength = 0x210;
		break;
	case 0x30:	// XX111XXX - AT45DB321B
		if ((j & 0x3c) == 0x34)
		{
			fat.lastpage = 8192-1;
			fd.fat_pagedata = 0x200;
			fd.fat_pagelength = 0x210;
		}
		else
		{
			xSemaphoreGiveRecursive(xFsMutex);
			return -102;	// Dataflash not recognized
		}
		break;
	default:
		xSemaphoreGiveRecursive(xFsMutex);
		return -102;	// Dataflash not recognized
	}

	i = fd.fat_pagelength;
	for (fd.fat_pageleftshift = 0; i; i >>= 1,fd.fat_pageleftshift++);
	fd.fat_buffermask = (1 << fd.fat_pageleftshift) - 1;
	fd.fat_pageleftshift -= 8;
	fd.fat_pagemask = fat.lastpage;

	fat.firstpage = 0;			// non salto la prima pagina poich� non e' riservata per parametri vitali

	fat.writtenpages = 0;

	EEP_Read(EEP_fsfirtfree_addr,&fat.firstfreepage,2);
	EEP_Read(EEP_fslastupage_addr,&fat.lastupdatedpage,2);

	if ((fat.firstfreepage > fat.lastpage) || (fat.firstfreepage < fat.firstpage) ||
		(fat.lastupdatedpage > fat.lastpage))
	{
		fat.firstfreepage = FATPG_NoPage;
		fat.lastupdatedpage = 0;
	}

	for (i = 0; i < FILE_MAX_OPEN_FILES; i++)
		file[i].state = FILE_closed;

	fd.fat_writing = 0;
	dirdata.state = FILE_UNFILE_LOCKED;
	dirdata.page = fat.lastpage+1;

	if(!StartFileFlag)
		if(!(StartFileFlag	= (uint8_t *)pvPortMalloc(((fat.lastpage - fat.firstpage + 1) >> 3)+1)))
		{
			xSemaphoreGiveRecursive(xFsMutex);
			return -103;
		}

	map = (uint8_t *)pvPortMalloc(((fat.lastpage - fat.firstpage + 1) >> 3)+1);
	
	setBuffer(B1,0xff);
	setBuffer(B2,0xff);
	do {
		if (map)
			memset(map, 0,((fat.lastpage - fat.firstpage + 1) >> 3)+1);

		memset(StartFileFlag, 0,((fat.lastpage - fat.firstpage + 1) >> 3)+1);

		err = FALSE;
		fat.freepages = 0;
		for (i = fat.firstpage; i <= fat.lastpage; i++)
		{
			if (!comparePage(B2,i))
			{
				readPageParam(i,&par);
				if (par.flags == FATPG_FLAGS(FATPG_PageBusy | FATPG_FileName))
				{
					readPagePartial(i,0,&fhdr,sizeof(fhdr));
					if (!checkFileName(&fhdr) || (fhdr.lastpage > fat.lastpage))
					{
						destroyBadFile(i,i);
						//COM_Printf("ffinit: invalid filename at page %u\r\n",i);
						err = TRUE;
					}
					else
					{
						if (map)
							MAP_SET_PAGE(i);

						SetStartFileFlag( i );

						// next page must be a valid one
						j = i;
						do 
						{
							if ((par.nextpage < fat.firstpage) || (par.nextpage > fat.lastpage))
							{
								destroyBadFile(i,j);
								err = TRUE;
								//COM_Printf("ffinit: invalid nextpage %u at page %u\r\n",par.nextpage,j);
								break;
							}
							else
							{
								j = par.nextpage;
								if (map)
									MAP_SET_PAGE(j);
								readPageParam(par.nextpage,&par);
								if (par.flags == FATPG_FLAGS(FATPG_PageBusy | FATPG_LastPage))
								{
									if ((par.nextpage != FATPG_NoPage) || (fhdr.lastpage != j))
									{
										destroyBadFile(i,j);
										err = TRUE;
										//COM_Printf("ffinit: invalid nextpage %u at last page %u\r\n",par.nextpage,j);
									}
									break;
								}
								else if (par.flags != FATPG_FLAGS(FATPG_PageBusy))
								{
									destroyBadFile(i,j);
									err = TRUE;
									//COM_Printf("ffinit: invalid flags %04X at page %u\r\n",par.flags,j);
									break;;
								}
							}
						} while (1);
					}
				}
				else if (par.flags == FATPG_FLAGS(FATPG_PageBusy | FATPG_FileName | FATPG_LastPage))
				{
					// next page must be NoPage
					readPagePartial(i,0,&fhdr,sizeof(fhdr));
					if (!checkFileName(&fhdr) || (fhdr.lastpage != i) || (par.nextpage != FATPG_NoPage))
					{
						destroyBadFile(i,i);
						err = TRUE;
						//COM_Printf("ffinit: nextpage %u is not nopage at firstpage %u\r\n",par.nextpage,i);
					}
					else
					{
						if (map)
							MAP_SET_PAGE(i);

						SetStartFileFlag( i );
					}
				}
				else if (par.flags == FATPG_FLAGS(FATPG_PageBusy | FATPG_LastPage))
				{
					// next page must be NoPage
					if (par.nextpage != FATPG_NoPage)
					{
						destroyBadFile(i,i);
						err = TRUE;
						//COM_Printf("ffinit: nextpage %u is not nopage at lastpage %u\r\n",par.nextpage,i);
					}
				}
				else if (par.flags == FATPG_FLAGS(FATPG_PageBusy))
				{
					// next page must be a valid one
					if ((par.nextpage < fat.firstpage) || (par.nextpage > fat.lastpage))
					{
						destroyBadFile(i,i);
						err = TRUE;
						//COM_Printf("ffinit: invalid nextpage %u at page %u\r\n",par.nextpage,i);
					}
				}
				else
				{
					// invalid page flags
					destroyBadFile(i,i);
					err = TRUE;
					//COM_Printf("ffinit: invalid flags %04X at page %u\r\n",par.flags,i);
				}
			}
			else
			{
				if (fat.firstfreepage == FATPG_NoPage)
					fat.firstfreepage = i;
				fat.freepages++;
			}
		}
	} while (err);

	if (map)
	{
		for (i = fat.firstpage; i <= fat.lastpage; i++)
		{
			if (!comparePage(B2,i))
			{
				if (!MAP_BUSY_PAGE(i))
				{
					erasePage(i);
					++fat.writtenpages;
					++fat.freepages;
				}
			}
		}
		vPortFree(map);
	}
	xQueueSend(xSema_Ready,&err,portMAX_DELAY);		// file system ready
	xSemaphoreGiveRecursive(xFsMutex);
	return 0;
}

/*Open a file for reading or writing. Only one file can be open
  for writing at a time.*/
FFILE *ffopen(const char *fileName, char type)
{
FFILE *stream;
char x;
FATPGPARAMST par;
FATFILEHEADERST fhdr;
int16_t i;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (!ffready(0))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return NULL;
	}
	if (type == 'r')
	{
		i = ffindFile(fileName);		//Check if file exists.
		if (i >= 0)
		{
			for (stream=file,x=0;x<FILE_MAX_OPEN_FILES;x++)       //Find a free FILE structure
			{
				if (stream->state == FILE_closed)       //Prepare FILE struct for file.
				{
					readPageParam(i,&par);
					readPagePartial(i,0,&stream->size,sizeof(uint32_t));
					stream->fnlen = strlen(fileName);
					strcpy(stream->filename,fileName);
					stream->state = FILE_openread;
					stream->firstpage = i;
					stream->page = par.nextpage;
					stream->ind = 0;
					stream->addr = 0;
					xSemaphoreGiveRecursive(xFsMutex);
					return stream;                        //return FILE ptr.
				}
				stream++;
			}
		}
		xSemaphoreGiveRecursive(xFsMutex);
		return NULL;
	}
	else if ((type == 'w') || (type == 'a'))
	{
		for (stream=file,x=0;x<FILE_MAX_OPEN_FILES;x++)
		{
			if (stream->state == FILE_openwrite)         //Check if there are other files open for writing.
			{
				xSemaphoreGiveRecursive(xFsMutex);
				return NULL;
			}
			stream++;
		}
		if (ffindFile(fileName) >= 0)	// If the file already exists
		{
			if (type == 'w')
				ffdelete(fileName);		// I delete it
		}
		else
			type = 'w';
		for (stream=file,x=0;x<FILE_MAX_OPEN_FILES;x++)
		{
			if (stream->state == FILE_closed)         //find free FILE struct.
			{
				stream->fnlen = strlen(fileName);
				strcpy(stream->filename,fileName);
				stream->state = FILE_openwrite;
				if (type == 'a')
				{
					i = ffindFile(fileName);
					//readPageParam(i,&par);
					//readPagePartial(i,0,&stream->size,sizeof(uint32_t));
					readPagePartial(i,0,&fhdr,sizeof(fhdr));

					stream->size = fhdr.size;
					stream->page = fhdr.lastpage;
					stream->firstpage = i;
					stream->addr = 0;

					if (stream->page > fat.lastpage)
					{
						stream->state = FILE_closed;
						xSemaphoreGiveRecursive(xFsMutex);
						return NULL;
					}
					readPageParam(stream->page,&par);
					
					if ((par.nextpage != FATPG_NoPage) || TEST_FALSE(par.flags,FATPG_LastPage))
					{
						// error, invalid sector chain
						stream->state = FILE_closed;
						xSemaphoreGiveRecursive(xFsMutex);
						return NULL;
					}
					stream->ind = stream->size % fd.fat_pagedata;
					if (stream->ind == 0) stream->ind = fd.fat_pagedata;
					mem2buffer(B1,stream->page);
					par.flags |= FATPG_LastPage;	// clear this flag
					writeBuffer(B1,fd.fat_pagedata,&par,sizeof(par));
					erasePage(stream->page);
					if (fat.firstfreepage == stream->page)
						FAT_getFreePage();	// avoid the reselection of the same page we just cleared
				}
				else
				{
					i = FAT_getFreePage();
					if (i > fat.lastpage)
					{
						// error, no further free page
						stream->state = FILE_closed;
						xSemaphoreGiveRecursive(xFsMutex);
						return NULL;
					}
					mem2buffer(B1,i);
					stream->firstpage = i;
					stream->page = i;
					stream->ind = fd.fat_pagedata;
					stream->size = 0;
					stream->addr = 0;

					memset(&fhdr,0xff,sizeof(fhdr));
					strcpy((char *)fhdr.name,fileName);
					fhdr.size = 0;
					fhdr.attr = 0;
					fhdr.lastpage = 0;
					writeBuffer(B1,0,&fhdr,sizeof(fhdr));

					par.flags = FATPG_FLAGS(FATPG_PageBusy | FATPG_FileName);
					par.nextpage = FATPG_NoPage;
					writeBuffer(B1,fd.fat_pagedata,&par,sizeof(par));

					SetStartFileFlag((uint16_t) i );
				}
				fd.fat_writing++;
				xSemaphoreGiveRecursive(xFsMutex);
				return stream;
			}
			stream++;
		}
		xSemaphoreGiveRecursive(xFsMutex);
		return NULL;
	}
	else
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return NULL;
	}
}

/* Write one char to FILE->buffer. If the buffer is full, write to page.*/
void ffput(FFILE *stream, char data)
{
	ffwrite(stream,&data,1);
}

void ffputs(FFILE *stream, const char *str)
{
	ffwrite(stream,str,strlen(str));
}

/* Read one char from FILE->buffer. If buffer is empty, read new page into buffer.*/
char ffget(FFILE *stream)
{
char data;

	ffread(stream,&data,1);
	return data;
}

/* write uses fput to write strings into file.*/
uint16_t ffwrite(FFILE *stream, const void *pvoid, uint16_t size)
{
uint8_t *pb;
uint16_t slice,len;
uint16_t i;
FATPGPARAMST par;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (stream->state != FILE_openwrite)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}

	len = size;
	pb = (uint8_t *)pvoid;

	while (size > 0)
	{
		if (stream->ind >= fd.fat_pagedata)
		{
			if ((i = FAT_getFreePage()) > fat.lastpage)
			{
				xSemaphoreGiveRecursive(xFsMutex);
				return 0;
			}
			if (stream->page == stream->firstpage)
			{
				readBuffer(B1,fd.fat_pagedata,&par,sizeof(par));
				par.nextpage = i;
				writeBuffer(B1,fd.fat_pagedata,&par,sizeof(par));
			}
			else
			{
				par.flags = FATPG_FLAGS(FATPG_PageBusy);
				par.nextpage = i;
				writeBuffer(B1,fd.fat_pagedata,&par,sizeof(par));
			}
			buffer2mem(B1,stream->page);
			stream->ind = 0;
			stream->page = i;
			mem2buffer(B1,i);
		}
		slice = fd.fat_pagedata-stream->ind;
		if (slice > size)
			slice = size;
		writeBuffer(B1,stream->ind,pb,slice);
		size -= slice;
		stream->ind += slice;
		pb += slice;
		stream->size += slice;
	}
	xSemaphoreGiveRecursive(xFsMutex);
	return len;
}

/* read uses fget to read strings from file.*/
uint16_t ffread(FFILE *stream, void *pvoid, uint16_t size)
{
FATPGPARAMST par;
uint8_t *pb;
uint16_t slice,len;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (stream->state != FILE_openread)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}

	len = size;
	pb = (uint8_t *)pvoid;
	while ((size > 0) && (stream->addr < stream->size))
	{
		if (stream->ind >= fd.fat_pagedata)
		{
			readPageParam(stream->page,&par);
			stream->page = par.nextpage;
			stream->ind = 0;
		}
		slice = fd.fat_pagedata - stream->ind;
		if (slice > size)
			slice = size;
		if (slice > (stream->size - stream->addr))
			slice = stream->size - stream->addr;
		readPagePartial(stream->page,stream->ind,pb,slice);
		size -= slice;
		stream->ind += slice;
		stream->addr += slice;
		pb += slice;
	}
	xSemaphoreGiveRecursive(xFsMutex);
	return len-size;
}

//read one line. 
uint16_t ffreadln(FFILE *stream, char *buffer, uint16_t size)
{
uint16_t x;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (size < 2)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}
	size--;
	
	if (stream->state != FILE_openread)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}

	for (x = 0; !ffeof(stream); )
	{
		if (ffread(stream,buffer+x,1) != 1)
			break;
		if (buffer[x] >= ' ')
		{
			if (x < size)
				x++;
		}
		else if ((buffer[x] == '\r') || (buffer[x] == '\n'))
		{
			if (x > 0)
			break;		//If EOL is found, stop reading and return.
	}
	}
	//EOL is not found, but the buffer is full.
	buffer[x] = 0;
	xSemaphoreGiveRecursive(xFsMutex);
	return x;
}

/* feof returns 1 if EOF is reached.*/
uint16_t ffeof(FFILE *stream)
{
	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (stream->state == FILE_openread)
	{
		if (stream->addr >= stream->size)
		{
			xSemaphoreGiveRecursive(xFsMutex);
			return 1;
		}
	}
	xSemaphoreGiveRecursive(xFsMutex);
	return 0;
}

void ffcloseall(void)
{
FFILE *stream;
uint8_t i;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (!ffready(0))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return;
	}
	for (i = 0, stream = file; i < FILE_MAX_OPEN_FILES; i++,stream++)
		ffclose(stream);
	xSemaphoreGiveRecursive(xFsMutex);
}

uint16_t ffclose (FFILE *stream)
{
FATPGPARAMST par;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (stream->state == FILE_openwrite)
	{
		if (stream->page == stream->firstpage)
		{
			par.flags = FATPG_FLAGS(FATPG_PageBusy | FATPG_FileName | FATPG_LastPage);
			par.nextpage = FATPG_NoPage;
			writeBuffer(B1,fd.fat_pagedata,&par,sizeof(par));
		}
		else
		{
			par.flags = FATPG_FLAGS(FATPG_LastPage | FATPG_PageBusy);
			par.nextpage = FATPG_NoPage;
			writeBuffer(B1,fd.fat_pagedata,&par,sizeof(par));
			buffer2mem(B1,stream->page);

			mem2buffer(B1,stream->firstpage);
			erasePage(stream->firstpage);
			fat.freepages++;
			stream->firstpage = FAT_getFreePage();
			SetStartFileFlag(stream->firstpage);
		}
		writeBuffer(B1,FAT_FILE_SIZE_OFFSET,&stream->size,sizeof(uint32_t));
		writeBuffer(B1,FAT_FILE_LASTPAGE_OFFSET,&stream->page,sizeof(uint16_t));
		buffer2mem(B1,stream->firstpage);
		stream->state = FILE_closed;
		fd.fat_writing--;
	}
	else if (stream->state == FILE_openread)
		stream->state = FILE_closed;
	xSemaphoreGiveRecursive(xFsMutex);
	return 1;
}

/*fdelete deletes file by putting the page number of the
  file header into the deallocated_ptrs storage. The space
  is not freed before reclaim() is activated.*/
// if successfull return 1 else return 0
uint16_t ffdelete(const char *fn)
{
int16_t i;	//,fh
FATPGPARAMST par;

//	for (fh = 0; fh < FILE_MAX_OPEN_FILES; fh++)
//		if (file[fh].state == FILE_openwrite)
//			break;
//	if (fh < FILE_MAX_OPEN_FILES)
//		return 0;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (!ffready(0))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}

	i = ffindFile(fn);
	if (i < 0)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}

	if (fat.firstfreepage == FATPG_NoPage)
		fat.firstfreepage = i;

	while ((i >= 0) && (i <= fat.lastpage))
	{
		readPageParam(i,&par);
		erasePage(i);

		RstStartFileFlag((uint16_t) i );

		++fat.writtenpages;
		fat.freepages++;
		i = par.nextpage;
	}
	xSemaphoreGiveRecursive(xFsMutex);
	return 1;
}

uint16_t ffcopy(const char *fn,const char *cn)
{
int16_t i,fpg,j,pgs;
uint16_t npgdt;
FATPGPARAMST par;
FATFILEHEADERST fhdr;

	if (!strcmp(fn,cn) || (strlen(cn) > FAT_FILE_NAME_LEN))
		return 0;
	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (!ffready(0) || fd.fat_writing)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}
	fpg = ffindFile(fn);
	if (fpg < 0)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}
	if (ffindFile(cn) >= 0)
		ffdelete(cn);

	pgs = 0;
	i = fpg;
	do {
		pgs++;
		readPageParam(i,&par);
		i = par.nextpage;
	} while ((i >= 0) && (i <= fat.lastpage));
	if (pgs >= fat.freepages)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}

	readPageParam(fpg,&par);
	if (par.nextpage != FATPG_NoPage)
	{
		i = par.nextpage;
		npgdt = j = FAT_getFreePage();
		while ((i >= 0) && (i < fat.lastpage))
		{
			mem2buffer(B1,i);
			readBuffer(B1,fd.fat_pagedata,&par,sizeof(par));
			i = par.nextpage;
			if (par.nextpage != FATPG_NoPage)
				par.nextpage = FAT_getFreePage();
			writeBuffer(B1,fd.fat_pagedata,&par,sizeof(par));
			buffer2mem(B1,j);
			pgs = j;	// could be the last page of the new file
			j = par.nextpage;
		}
		j = FAT_getFreePage();
	}
	else
	{
		npgdt = FATPG_NoPage;
		pgs = j = FAT_getFreePage();
	}

	mem2buffer(B1,fpg);
	readBuffer(B1,fd.fat_pagedata,&par,sizeof(par));
	par.nextpage = npgdt;
	writeBuffer(B1,fd.fat_pagedata,&par,sizeof(par));	
	readBuffer(B1,0,&fhdr,sizeof(fhdr));
	memset(fhdr.name,0xff,sizeof(fhdr.name));
	strcpy((char *)fhdr.name,cn);
	fhdr.lastpage = pgs;
	writeBuffer(B1,0,&fhdr,sizeof(fhdr));	// insert name of the copy
	buffer2mem(B1,j);
	SetStartFileFlag(j);

	xSemaphoreGiveRecursive(xFsMutex);
	return 1;
}


/*ffmove rename file */
// if successfull return 1 else return 0
uint16_t ffmove(const char *fn,const char *nn)
{
int16_t i;
FATFILEHEADERST fhdr;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (!ffready(0))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}
	if (ffindFile(nn) >= 0)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}

	i = ffindFile(fn);
	if (i < 0)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}

	mem2buffer(B2,i);

	readBuffer(B2,0,&fhdr,sizeof(fhdr));
	memcpy((char *)fhdr.name,nn,FAT_FILE_NAME_LEN);
	fhdr.name[FAT_FILE_NAME_LEN] = 0;

	writeBuffer(B2,0,&fhdr,sizeof(fhdr));

	if (!comparePage(B2,i))
	{
		// the page is different, must rewrite it
		erasePage(i);
		buffer2mem(B2,i);
	}

	setBuffer(B2,0xff);	// reset the page to its default (blank)

	xSemaphoreGiveRecursive(xFsMutex);
	return 1;
}

int16_t ffindFile(const char *fn)
{
uint16_t page = fat.firstpage;
FATPGPARAMST par;
FATFILEHEADERST fhdr;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (!ffready(0))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return -1;
	}
	while (searchStartFilePageIdx(&page))
	{
		if (!comparePage(B2,page))
		{
	 		if (readPageParam(page,&par) && TEST_TRUE(par.flags,FATPG_FileName))
			{
				readPagePartial(page,0,&fhdr,sizeof(fhdr));
				fhdr.name[FAT_FILE_NAME_LEN] = 0;
				if (strcmp((char *)fhdr.name,fn) == 0)
				{
					xSemaphoreGiveRecursive(xFsMutex);
					return (int16_t)page;
			}
		}
		}
		page++;
	}
	xSemaphoreGiveRecursive(xFsMutex);
	return -1;
}

int16_t ffseek(FFILE *stream,long pos,enum FSEEK_Enum whence)
{
FATPGPARAMST par;
int16_t h,i;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if ((stream == NULL) || (stream->state != FILE_openread))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return -1;
	}

	if (whence == FSEEK_CUR)
		pos = stream->addr + pos;
	else if (whence == FSEEK_END)
		pos = stream->size + pos;
	else if (whence != FSEEK_SET)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return -1;
	}

	if ((pos < 0) || (pos >= stream->size))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return -1;
	}

	if (pos != stream->addr)
	{
		h = pos / fd.fat_pagedata;

		readPageParam(stream->firstpage,&par);

		for (i = par.nextpage; (i >= fat.firstpage) && (i <= fat.lastpage) && (h > 0);	h--)
		{
			readPageParam(i,&par);
			i = par.nextpage;
		}

		if ((h > 0) || (i < fat.firstpage) || (i > fat.lastpage))
		{
			xSemaphoreGiveRecursive(xFsMutex);
			return -1;
		}

		stream->page = i;
		stream->ind = pos % fd.fat_pagedata;
		stream->addr = pos;
	}
	xSemaphoreGiveRecursive(xFsMutex);
	return 0;
}

long fftell(FFILE *stream)
{
long res;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if ((stream == NULL) || (stream->state == FILE_closed))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return -1;
	}

	res = (stream->state == FILE_openread)? stream->addr: stream->size;
	xSemaphoreGiveRecursive(xFsMutex);
	return res;
}

long ffsize(FFILE *stream)
{
long res;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if ((stream == NULL) || (stream->state == FILE_closed))
		res = -1;
	else
		res = stream->size;
	xSemaphoreGiveRecursive(xFsMutex);
	return res;
}

/*opendir opens the directory for reading*/
DIR *opendir(void)
{
	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (!ffready(0))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return NULL;
	}
	if (dirdata.state == FILE_UNFILE_LOCKED)
	{
		dirdata.state = FILE_LOCKED;
		dirdata.page = fat.firstpage;
		xSemaphoreGiveRecursive(xFsMutex);
		return (DIR *)&dirdata;
	}
	xSemaphoreGiveRecursive(xFsMutex);
	return NULL;
}

/* readdir outputs one line of dir in reduced mode (only file name).*/
char *readdir(DIR *_dir)
{
FATPGPARAMST par;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if ((_dir == NULL) || (dirdata.state != FILE_LOCKED))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return NULL;
	}
	else
	{
		while (searchStartFilePageIdx(&dirdata.page))
		{
			if (!comparePage(B2,dirdata.page))
			{
				if (readPageParam(dirdata.page,&par) && TEST_TRUE(par.flags,FATPG_FileName))
				{
					readPagePartial(dirdata.page,FAT_FILE_NAME_OFFSET,dirdata.strdir,FAT_FILE_NAME_LEN+1);
					strcat(dirdata.strdir,"\r\n");
					dirdata.page++;
					xSemaphoreGiveRecursive(xFsMutex);
					return dirdata.strdir;
				}
			}
			dirdata.page++;
		}
		dirdata.state = FILE_LOCKED_COMPLETE;
		xSemaphoreGiveRecursive(xFsMutex);
		return NULL;
	}
}

/* readdir outputs one line of dir in extended mode (file name and file size).*/
char *readwdir(DIR *_dir)
{
FATPGPARAMST par;
uint32_t fileSize;
uint16_t offset;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if ((_dir == NULL) || (dirdata.state != FILE_LOCKED))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return NULL;
	}
	else
	{
		while (searchStartFilePageIdx(&dirdata.page))
		{
			if (!comparePage(B2,dirdata.page))
			{
				if (readPageParam(dirdata.page,&par) && TEST_TRUE(par.flags,FATPG_FileName))
				{
					readPagePartial(dirdata.page,0,&fileSize,sizeof(uint32_t));
					offset = sprintf(dirdata.strdir, "---------- 1 ews ews %8lu  Jan  1 00:00 ",fileSize);
					readPagePartial(dirdata.page,FAT_FILE_NAME_OFFSET,dirdata.strdir+offset,FAT_FILE_NAME_LEN+1);
					strcat(dirdata.strdir,"\r\n");
					dirdata.page++;
					xSemaphoreGiveRecursive(xFsMutex);
					return dirdata.strdir;
				}
			}
			dirdata.page++;
		}
		dirdata.page = fat.lastpage+1;
		dirdata.state = FILE_LOCKED_COMPLETE;
        sprintf(dirdata.strdir,"Free space: %lu\r\n",ffreeSpace());
		xSemaphoreGiveRecursive(xFsMutex);
		return dirdata.strdir;
	}
}

/*closedir prepare DIR for new directory.*/
uint16_t closedir(DIR *_dir)
{
	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (_dir->state == FILE_UNFILE_LOCKED)
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return (0);
	}
	_dir->state = FILE_UNFILE_LOCKED;
	xSemaphoreGiveRecursive(xFsMutex);
	return (1);
}

long ffreeSpace(void)
{
long l;

	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
	if (!ffready(0))
	{
		xSemaphoreGiveRecursive(xFsMutex);
		return 0;
	}
	l = fat.freepages;
	if (l > 0) l--;
	l *= fd.fat_pagedata;
	xSemaphoreGiveRecursive(xFsMutex);
	return l;
}

static void file_system_task(void *prv)
{
int i;

	i = ffinit();

	if (i != 0)
		Dprintf(DBGLVL_Error,"ffile: error initializing fs on dataflash (%d)\r\n",i);

	for (;;)
	{
		vTaskDelay(kSec);
		ffrefresh();
	}
}

void fflock(void)
{
	xSemaphoreTakeRecursive(xFsMutex,portMAX_DELAY);
}

void ffstart(void)
{
	if (xSema_Ready)
		return;
	xSema_Ready = xQueueCreate( 1, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );
	xFsMutex = xSemaphoreCreateRecursiveMutex();
	xTaskCreate(file_system_task,"ffile",128, NULL,tskIDLE_PRIORITY,NULL);
}
