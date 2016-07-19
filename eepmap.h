#ifndef __EEPMAP_H
#define __EEPMAP_H

// Calibration values
#define EEP_HNch_addr				0x00
#define EEP_HNcl_addr				0x04
#define EEP_HGF_addr				0x08
#define EEP_HNos_addr				0x0c
#define EEP_HPhase_addr				0x10
#define EEP_MNch_addr				0x14
#define EEP_MNcl_addr				0x18
#define EEP_MGF_addr				0x1c
#define EEP_MNos_addr				0x20
#define EEP_MPhase_addr				0x24
#define EEP_LNch_addr				0x28
#define EEP_LNcl_addr				0x2c
#define EEP_LGF_addr				0x30
#define EEP_LNos_addr				0x34
#define EEP_LPhase_addr				0x38
#define EEP_PNch_addr				0x3c
#define EEP_PNcl_addr				0x40
#define EEP_PGF_addr				0x44
#define EEP_PNos_addr				0x48
#define EEP_PPhase_addr				0x4c
#define EEP_KCell_addr				0x50
#define EEP_GCable_addr				0x54
#define EEP_Freq_addr				0x58
#define EEP_Freq_len				4
#define EEP_RCable_addr				0x5c
#define EEP_RCable_len				4

#define EEP_fsfirtfree_addr			0xf00	//!< File system non-volatile parameter
#define EEP_fsfirtfree_len			2		//!< EEP_fsfirtfree_addr length
#define EEP_fslastupage_addr		0xf02	//!< File system non-volatile parameter
#define EEP_fslastupage_len			2		//!< EEP_fslastupage_addr length

//! @brief	double word 32bit debug flags, stored in little endian format,
//!			this data are read at startup and used by debug.c modules, the available debug flags are listed inside debug.h file
//!			set to 0 or 0xffffffff to start with no debug flag activated
#define	EEP_dbglvl_addr				0xf04
#define EEP_dbglvl_len				4		//!< EEP_dbglvl_addr length

#define EEP_runAct_addr				0xf08	//!< set to 1 to activate activity_task
#define EEP_runAct_len				1		//!< EEP_runAct_addr length

#define EEP_runBle_addr				0xf09	//!< set to 1 to activate ble channel
#define EEP_runBle_len				1		//!< EEP_runBle_addr length

#endif
