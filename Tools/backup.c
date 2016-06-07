#include "bsp.h"
#include "portmacro.h"
#include "backup.h"

static const uint16_t BKPDataReg[BKP_DR_NUMBER] = {
	BKP_DR1, BKP_DR2, BKP_DR3, BKP_DR4, BKP_DR5, BKP_DR6, BKP_DR7, BKP_DR8,
	BKP_DR9, BKP_DR10, BKP_DR11, BKP_DR12, BKP_DR13, BKP_DR14, BKP_DR15, BKP_DR16,
	BKP_DR17, BKP_DR18, BKP_DR19, BKP_DR20, BKP_DR21, BKP_DR22, BKP_DR23, BKP_DR24,
	BKP_DR25, BKP_DR26, BKP_DR27, BKP_DR28, BKP_DR29, BKP_DR30, BKP_DR31, BKP_DR32,
	BKP_DR33, BKP_DR34, BKP_DR35, BKP_DR36, BKP_DR37, BKP_DR38, BKP_DR39, BKP_DR40,
	BKP_DR41, BKP_DR42
};  


void BKP_storeDWord(uint16_t bkpidx,uint32_t value)
{
	if (bkpidx >= (BKP_DR_NUMBER-1))
		return;
	vPortEnterCritical();
	BKP_WriteBackupRegister(BKPDataReg[bkpidx],(uint16_t)(value >> 16));
	BKP_WriteBackupRegister(BKPDataReg[bkpidx+1],(uint16_t)value);
	vPortExitCritical();
}

uint32_t BKP_getDWord(uint16_t bkpidx)
{
uint32_t value;

	if (bkpidx >= (BKP_DR_NUMBER-1))
		return 0;
	value = BKP_ReadBackupRegister(BKPDataReg[bkpidx]);
	value <<= 16;
	value += BKP_ReadBackupRegister(BKPDataReg[bkpidx+1]);
	return value;
}

uint16_t BKP_getWord(uint16_t bkpidx)
{
	if (bkpidx >= BKP_DR_NUMBER)
		return 0;
	return BKP_ReadBackupRegister(BKPDataReg[bkpidx]);
}

void BKP_storeWord(uint16_t bkpidx,uint16_t value)
{
	if (bkpidx >= BKP_DR_NUMBER)
		return;
	vPortEnterCritical();
	BKP_WriteBackupRegister(BKPDataReg[bkpidx],value);
	vPortExitCritical();
}