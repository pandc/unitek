#ifndef __VERSION_H
#define __VERSION_H

//! @addtogroup version
//! @brief Module containing informations about the fw version
//! @{

#define QUOTEME_(x)			#x
#define QUOTEME(x)			QUOTEME_(x)

#define TOKENPASTE0(x, y)	x ## y
#define TOKENPASTE(x, y)	TOKENPASTE0(x, y)

#define BOARD_HW_ID			1

#define VARIANT_ID			1

#define	VERSION_MAJOR		00
#define	VERSION_MINOR		90
#define VERSION_REVISION	00

//! @brief struct containing fw informations
__packed struct fw_struct {
	unsigned long firm;			//!< Specific firm
	unsigned long fw_revision; 	//!< variable combining major, minor, revision and variant id
	unsigned short board_id;	//!< board id
	unsigned short chksum;		//!< checksum
};

//! @brief variable containing fw informations
extern const __packed struct fw_struct fw_version;
//! @brief null terminated string containing fw version and compile date and time
extern const char gBanner[];

//! @}

#endif
