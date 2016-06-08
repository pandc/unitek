#ifndef __MSPI_H
#define __MSPI_H

enum SPISETUP_Enum {
	SPISETUP_Display,
	SPISETUP_Dataflash,
	SPISETUP_Items
};

enum SPIDMAINC_Enum {
	SPIDMAINC_NoInc,
	SPIDMAINC_TxInc,
	SPIDMAINC_RxInc,
	SPIDMAINC_Items
};

void MSPI_Unlock(void);;
void MSPI_Lock(uint8_t setup);
uint8_t MSPI_Write(uint8_t data);
void MSPI_Write_Dma(const uint8_t *txb,uint8_t *rxb,uint16_t size,uint8_t incopt);
void MSPI_Init(void);

#endif
