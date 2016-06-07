#ifndef __DBGLVL_H
#define __DBGLVL_H

//! @addtogroup debug
//! @brief Module controlling trace debug output
//! @{

#define DBGLVL_Info			0				//!< Unconditional trace print flag
#define DBGLVL_Error		0xffffffff		//!< Unconditional error trace print flag
#define DBGLVL_Logit		0x80000000		//!< Flag to activate file system storage of debug trace

//! @brief Enumerated type listing all the debug flages recognized by application sources
enum DBGLVL_Enum {
	DBGLVL_Meas=(1 << 0),			//!< Measures
	DBGLVL_Keyboard=(1 << 1),		//!< Keyboard
};

#define DBGLVL_NumBits		2		//!< Total number of debug flags available

#if defined(COM_NONE)
#include "board.h"
#define DBG_Printf(lvlmsk,...)		__no_operation()
#define DBG_Puts(lvlmsk,str)		__no_operation()

#define Dprintf(dbg,...)			__no_operation()
#define DBG_CheckLevel(lvl)			0
#define DBG_GetMutex()				__no_operation()
#define DBG_RelMutex()				__no_operation()
#define DBG_Init()					__no_operation()
#define DBG_SetLvl(a)				__no_operation()
#define DBG_GetLvl()				0

#else
extern const char *str_dbgBitsName[DBGLVL_NumBits];		//!< list of string describing the debug flags included in enum DBGLVL_Enum, these strings are printed using the command AT*DBG=?

/**
 *  @brief Check if one of a bunch of flags is active
 *  
 *  @param [in] lvl mask of debug flags to check
 *  @return TRUE if at least one of the flags included in lvl is active
 */
int DBG_CheckLevel(uint32_t lvl);
/**
 *  @brief Get all the debug flags already active
 *  
 *  @return mask of debug flags currently active
 */
uint32_t DBG_GetLvl(void);
/**
 *  @brief Function to set new debug flags mask
 *  
 *  @param [in] lvl new debug flags mask to set active
 */
void DBG_SetLvl(uint32_t lvl);
/**
 *  @brief Flag conditioned formatted print
 *  
 *  @param [in] dbg one or more flags identifying the kind of debug output, string cannot be greater than 128 characters
 *  @param [in] fmt formatted string
 *  @param [in] ... variable list of parameters
 */
void Dprintf(uint32_t dbg,const char *fmt,...);
//! @brief Get debug mutex, reserving the debug channel to print informations using other methods, COM_Printf i.e.
void DBG_GetMutex(void);
//! @brief Release debug mutex
void DBG_RelMutex(void);
//! @brief Debug module initialization, called by main.c
void DBG_Init(void);
#endif

//! @}

#endif
