#include "types.h"
#include "version.h"

const char gBanner[] =
	QUOTEME(VERSION_MAJOR) "." QUOTEME(VERSION_MINOR) "." QUOTEME(VERSION_REVISION)
	" Unitek-demo"
	" - " __DATE__ " " __TIME__;

#pragma location="REV_CODE"
const __packed struct fw_struct fw_version = {
	.firm = 0x4B455455,		// UniTEK
	.fw_revision = (TOKENPASTE(0x,VERSION_MAJOR) << 24) | (TOKENPASTE(0x,VERSION_MINOR) << 16) | (TOKENPASTE(0x,VERSION_REVISION) << 8) | TOKENPASTE(0x,VARIANT_ID),
	.board_id = TOKENPASTE(0x,BOARD_HW_ID),
	.chksum = 0
};
