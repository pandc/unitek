#ifndef __IO_H
#define __IO_H

// DF_SELn		PA15
#define	DF_SELn_PORT		GPIOA
#define DF_SELn_PIN			GPIO_Pin_15

// SPI1_SCK		PB3	
#define DF_SCK_PORT			GPIOB
#define DF_SCK_PIN			GPIO_Pin_3

// SPI1_MISO	PB4	
#define DF_MISO_PORT		GPIOB
#define DF_MISO_PIN			GPIO_Pin_4

// SPI1_MOSI	PB5	
#define DF_MOSI_PORT		GPIOB
#define DF_MOSI_PIN			GPIO_Pin_5

#define DISP_CSn_PORT		GPIOA
#define DISP_CSn_PIN		GPIO_Pin_4

#define DISP_MOSI_PORT		GPIOC
#define DISP_MOSI_PIN		GPIO_Pin_12

#define DISP_SCK_PORT		GPIOC
#define DISP_SCK_PIN		GPIO_Pin_10

#define DISP_VCC_ON_PORT	GPIOD
#define DISP_VCC_ON_PIN		GPIO_Pin_15

#define DISP_RSTn_PORT		GPIOD
#define DISP_RSTn_PIN		GPIO_Pin_14

#define DISP_CMDn_PORT		GPIOD
#define DISP_CMDn_PIN		GPIO_Pin_13

#define KBD_IRQn_PORT				GPIOD
#define KBD_IRQn_PIN				GPIO_Pin_0
#define KBD_IRQn_PIN_SOURCE			GPIO_PinSource0
#define KBD_IRQn_PORT_SOURCE		GPIO_PortSourceGPIOD
#define KBD_IRQn_EXTI_LINE			EXTI_Line0
#define KBD_IRQn_EXTI_IRQ_HANDLER	EXTI0_IRQHandler
#define KBD_IRQn_EXTI_IRQn			EXTI0_IRQn

#define KBD_RSTn_PORT		GPIOD
#define KBD_RSTn_PIN		GPIO_Pin_12

#endif
