/**
  ******************************************************************************
  * @file    stm32f107.c
  * @author  MCD Application Team
  * @version V1.0.0
  * @date    11/20/2009
  * @brief   STM32F107 hardware configuration
  ******************************************************************************
  * @copy
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2009 STMicroelectronics</center></h2>
  */

/* Includes ------------------------------------------------------------------*/
#include "bsp.h"
#include "io.h"
#include "backup.h"

#define RTC_CONF_DONE	0x55AA

#pragma section = "MHEAPH"
#define MAGIC_WORD_ADDRESS		((uint32_t)__section_end("MHEAPH"))

#pragma section = ".intvec"
#define INTVEC_START			((uint32_t)__section_begin(".intvec"))

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
void GPIO_Configuration(void);
void NVIC_Configuration(void);
void ADC_Configuration(void);
//void Ethernet_Configuration(void);

void GpioInit(GPIO_TypeDef* reg,u16 pin,GPIOMode_TypeDef type, u8 defValue)
{
GPIO_InitTypeDef GPIO_InitStructure;

	if ((type == GPIO_Mode_Out_OD) || (type == GPIO_Mode_Out_PP))
	{
		if (defValue)
			GPIO_SetBits(reg,pin);
		else
			GPIO_ResetBits(reg,pin);
	}
	GPIO_InitStructure.GPIO_Pin = pin;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_Default;
	GPIO_InitStructure.GPIO_Mode = type;
	GPIO_Init(reg, &GPIO_InitStructure);
	if ((type == GPIO_Mode_Out_OD) || (type == GPIO_Mode_Out_PP))
	{
		if (defValue)
			GPIO_SetBits(reg,pin);
		else
			GPIO_ResetBits(reg,pin);
	}
}

static void WDOG_Configuration(void)
{
#if !defined(NO_WATCHDOG)
	/* IWDG timeout equal to 280 ms (the timeout may varies due to LSI frequency dispersion) */
	/* Enable write access to IWDG_PR and IWDG_RLR registers */
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
	
	/* IWDG counter clock: 40KHz(LSI) / 32 = 1.25 KHz */
	IWDG_SetPrescaler(IWDG_Prescaler_32);
	
	// Set counter reload value to 349
	IWDG_SetReload(0xfff);	// max value
	
	/* Reload IWDG counter */
	IWDG_ReloadCounter();
	
	/* Enable IWDG (the LSI oscillator will be enabled by hardware) */
	IWDG_Enable();
#endif
}

static void InitRtc(void)
{
uint16_t WaitForOscSource;

	/*Allow access to Backup Registers*/
	PWR_BackupAccessCmd(ENABLE);

	//if (BKP_getWord(BACKUP_RtcInit) != RTC_CONF_DONE)
	{
		/*Enables the clock to Backup and power interface peripherals    */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_BKP | RCC_APB1Periph_PWR,ENABLE);

		/*Enable 32.768 kHz external oscillator */
		//RCC_LSEConfig(RCC_LSE_ON);
		RCC_LSICmd(ENABLE);

		for(WaitForOscSource=0;WaitForOscSource<5000;WaitForOscSource++)
		{
		}

		RCC_RTCCLKConfig(RCC_RTCCLKSource_LSI);
		/* RTC Enabled */
		RCC_RTCCLKCmd(ENABLE);
		RTC_WaitForLastTask();
		/*Wait for RTC registers synchronisation */
		RTC_WaitForSynchro();
		RTC_WaitForLastTask();
		/* Setting RTC Interrupts-Seconds interrupt enabled */
		/* Enable the RTC Second */
		//RTC_ITConfig(RTC_IT_SEC , ENABLE);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();

		/* Set RTC prescaler: set RTC period to 1 sec */
		RTC_SetPrescaler(45000); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */
		/* Prescaler is set to 32766 instead of 32768 to compensate for
		lower as well as higher frequencies*/
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		if (BKP_getWord(BACKUP_RtcInit) != RTC_CONF_DONE)
		{
			BKP_storeWord(BACKUP_RtcInit, RTC_CONF_DONE);
			RTC_SetCounter(0);
		}
	}
#if 0
	else
	{
		/* PWR and BKP clocks selection */
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
		for(WaitForOscSource=0;WaitForOscSource<5000;WaitForOscSource++);
		/* Wait until last write operation on RTC registers has finished */
		RTC_WaitForLastTask();
		/* Enable the RTC Second */
		//RTC_ITConfig(RTC_IT_SEC, ENABLE);
		RTC_WaitForLastTask();
	}
#endif
	BKP_RTCOutputConfig(BKP_RTCOutputSource_None);
}

/**
  * @brief  Setup STM32 system (clocks, Ethernet, GPIO, NVIC) and STM3210C-EVAL resources.
  * @param  None
  * @retval None
  */
void System_Setup(void)
{
RCC_ClocksTypeDef RCC_Clocks;

	/* Setup STM32 clock, PLL and Flash configuration) */
	SystemInit();

	/* Enable ETHERNET clock  */
	//RCC_AHBPeriphClockCmd(RCC_AHBPeriph_ETH_MAC | RCC_AHBPeriph_ETH_MAC_Tx |
	//					RCC_AHBPeriph_ETH_MAC_Rx, ENABLE);

	/* Enable PWR and BKP clock */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

	/* Enable GPIOs and ADC1 clocks */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_GPIOB | RCC_APB2Periph_GPIOC |
							RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE | RCC_APB2Periph_AFIO
							, ENABLE);

	/* Enable write access to Backup domain */
	PWR_BackupAccessCmd(ENABLE);

	/* Clear Tamper pin Event(TE) pending flag */
	BKP_ClearFlag();

	/* NVIC configuration */
	NVIC_Configuration();  

	/* ADC configuration */
	//ADC_Configuration();

	/* Configure the GPIO ports */
	GPIO_Configuration();

	/* Configure the Ethernet peripheral */
	//Ethernet_Configuration();

	/* SystTick configuration: an interrupt every 1ms */
	RCC_GetClocksFreq(&RCC_Clocks);

	// Disable systick
	SysTick->CTRL = 0;

	WDOG_Configuration();

	InitRtc();
}

void GPIO_Configuration(void)
{
	GpioInit(KBD_RSTn_PORT,KBD_RSTn_PIN,GPIO_Mode_Out_PP,1);
	GpioInit(KBD_IRQn_PORT,KBD_IRQn_PIN,GPIO_Mode_IN_FLOATING,0);
}

#if 0
/**
  * @brief  Configures the different GPIO ports.
  * @param  None
  * @retval None
  */
void GPIO_ETH_Configuration(void)
{
	/* ETHERNET pins configuration */
	/* AF Output Push Pull:
	- ETH_MII_MDIO / ETH_RMII_MDIO: PA2
	- ETH_MII_MDC / ETH_RMII_MDC: PC1
	- ETH_MII_TXD2: PC2
	- ETH_MII_TX_EN / ETH_RMII_TX_EN: PB11
	- ETH_MII_TXD0 / ETH_RMII_TXD0: PB12
	- ETH_MII_TXD1 / ETH_RMII_TXD1: PB13
	- ETH_MII_PPS_OUT / ETH_RMII_PPS_OUT: PB5	(Not used)
	- ETH_MII_TXD3: PB8 */

	/* Configure PA2 as alternate function push-pull */
	GpioInit(GPIOA,GPIO_Pin_2,GPIO_Mode_AF_PP,0);
	
	/* Configure PC1, PC2 as alternate function push-pull */
	GpioInit(GPIOC,GPIO_Pin_1 | GPIO_Pin_2,GPIO_Mode_AF_PP,0);
	
	/* Configure PB8, PB11, PB12 and PB13 as alternate function push-pull */
	GpioInit(GPIOB,GPIO_Pin_8 | GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13,GPIO_Mode_AF_PP,0);

	/**************************************************************/
	/*               For Remapped Ethernet pins                   */
	/*************************************************************/
	/* Input (Reset Value):
	- ETH_MII_CRS CRS: PA0
	- ETH_MII_RX_CLK / ETH_RMII_REF_CLK: PA1
	- ETH_MII_COL: PA3
	- ETH_MII_RX_DV / ETH_RMII_CRS_DV: PA7
	- ETH_MII_TX_CLK: PC3
	- ETH_MII_RXD0 / ETH_RMII_RXD0: PC4
	- ETH_MII_RXD1 / ETH_RMII_RXD1: PC5
	- ETH_MII_RXD2: PB0
	- ETH_MII_RXD3: PB1
	- ETH_MII_RX_ER: PB10 */
	
	/* ETHERNET pins remapp in STM3210C-EVAL board: RX_DV and RxD[3:0] */
	//GPIO_PinRemapConfig(GPIO_Remap_ETH, ENABLE);
	
	// Configure PA0, PA1 and PA3 as input - MII_CRS, MII_RX_CLK, MII_COL
	GpioInit(GPIOA,GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_3,GPIO_Mode_IN_FLOATING,0);
	
	// Configure PC3 as input - MII_TX_CLK
	GpioInit(GPIOC,GPIO_Pin_3,GPIO_Mode_IN_FLOATING,0);
	
	// Configure PA7 as Input - MII_RX_DV
	GpioInit(GPIOA,GPIO_Pin_7,GPIO_Mode_IN_FLOATING,0);
	
	// Configure PC4, PC5 as Input - MII_RXD0, MII_RXD1
	GpioInit(GPIOC,GPIO_Pin_4 | GPIO_Pin_5,GPIO_Mode_IN_FLOATING,0);
	
	// Configure PB0, PB1 as Input - MII_RXD2, MII_RXD3
	GpioInit(GPIOB,GPIO_Pin_0 | GPIO_Pin_1,GPIO_Mode_IN_FLOATING,0);
  
	/* Configure PB10 as input */
	GpioInit(GPIOB,GPIO_Pin_10,GPIO_Mode_IN_FLOATING,0);
	
	/* MCO pin configuration------------------------------------------------- */
	/* Configure MCO (PA8) as alternate function push-pull */
	GpioInit(MCO_PORT,MCO_PIN,GPIO_Mode_AF_PP,0);

	// Configure PE4 MII_LINKN 1
	GpioInit(MII_LINKN_PORT,MII_LINKN_PIN,GPIO_Mode_Out_PP,1);

	// Configure PE14 SWITCH_PWRDN 0
	GpioInit(SWITCH_PWRDN_PORT,SWITCH_PWRDN_PIN,GPIO_Mode_Out_PP,0);

	// Configure PE3 SWITCH_RST 0
	GpioInit(SWITCH_RST_PORT,SWITCH_RST_PIN,GPIO_Mode_Out_PP,1);	// switch in reset
}
#endif

/**
  * @brief  Configures the nested vectored interrupt controller.
  * @param  None
  * @retval None
  */
void NVIC_Configuration(void)
{
	/* Set the Vector Table base location at 0x08000000 */
	//NVIC_SetVectorTable(NVIC_VectTab_FLASH, INTVEC_START - NVIC_VectTab_FLASH);
#if defined(IAP)
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0x10000);
#else
	NVIC_SetVectorTable(NVIC_VectTab_FLASH, 0);
#endif	

	/* 4 bits for pre-emption priority, 0 bits for subpriority */
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_4); 
}

void system_reset(void)
{
	WDOG_Refresh();
	NVIC_SystemReset();
}

uint64_t system_getcmd(void)
{
	return XQWORD(MAGIC_WORD_ADDRESS);
}

void system_setcmd(uint64_t cmd)
{
	XQWORD(MAGIC_WORD_ADDRESS) = cmd;
}

/******************* (C) COPYRIGHT 2009 STMicroelectronics *****END OF FILE****/
