#ifndef __BACKUP_H
#define __BACKUP_H

#define BKP_DR_NUMBER			42

#define	BACKUP_FirstFreePage	0
#define BACKUP_LastUpdatedPage	1
#define BACKUP_PowerFails		2
#define BACKUP_Restarts			3
#define BACKUP_RtcInit			4

void BKP_storeWord(uint16_t bkpidx,uint16_t value);
uint16_t BKP_getWord(uint16_t bkpidx);
void BKP_storeDWord(uint16_t bkpidx,uint32_t value);
uint32_t BKP_getDWord(uint16_t bkpidx);

#endif
