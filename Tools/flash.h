#ifndef __FLASH_H
#define	__FLASH_H

#define PAGE_SIZE				(0x800)
#define FLASH_SIZE				(0x40000)	/* 256K */
#define	FlashSpaceBase			(0x8000000)
#define	ApplicationOffset		(0x10000)
#define FlashUpperLimit			(SerialNumberOffset-ApplicationOffset)
#define	SerialNumberOffset		(MacAddrOffset-PAGE_SIZE)
#define	SerialNumberAddress		(FlashSpaceBase+SerialNumberOffset)
#define MacAddrOffset			(FLASH_SIZE-PAGE_SIZE)
#define MacAddrAddress			(FlashSpaceBase+MacAddrOffset)

#define PartitionSize			FlashUpperLimit
#define	ChecksumOffset			(PartitionSize-4)
#define	ApplicationSize			ChecksumOffset

#define ApplicationAddress		(FlashSpaceBase+ApplicationOffset)
#define	ChecksumAddress			(ApplicationAddress+ChecksumOffset)

#endif
