#include "stm32f10x.h"
#include "bsp.h"
#include "io.h"
#include "irqprio.h"

#include "freertos.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#include "i2c.h"

#include "com.h"
#include "debug.h"
#include "my_definitions.h"
#include "ioexp.h"

#include "keyboard.h"

#define KEYBCTRL_ADDR			0x5

#define KBREG_CTRL				0
#define KBREG_STROKE			1
#define KBREG_MASK				9

#define KEYSTR_Stroke			0x80

#define KEYRELEASED_TIME	(kCen)
#define KEYPRESSED_TIME		(kCen*3)

#define KeyQueueSize		16

static xSemaphoreHandle xSemaKeybo;
static xQueueHandle xKeyQueue;
static TaskHandle_t tockhandle;

static void keyboard_task(void *par);
static void tock_signal(void);
static void tock_task(void *par);

extern unsigned char immagine_stato_uscite;

static uint8_t key_decode(uint16_t mask)
{
	switch(mask)
	{
	case 8:
		return 'U';
	case 0x10:
		return '+';
	case 0x20:
		return '-';
	case 2:
		return 'K';
	case 1:
		return 'D';
	case 4:
		return 'P';
	}
	return 0;
}

int key_getstroke(uint8_t *key,uint32_t tout)
{
	return xQueueReceive(xKeyQueue,key,tout);
}

int key_putstroke(uint8_t stroke,uint32_t tout)
{
	return xQueueSend(xKeyQueue,&stroke,tout);
}

void KBD_IRQn_EXTI_IRQ_HANDLER(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (EXTI_GetITStatus(KBD_IRQn_EXTI_LINE) != RESET)
	{
		/* Clear IT flag */
		EXTI_ClearITPendingBit(KBD_IRQn_EXTI_LINE);
		/* Disable EXTI Line9 IRQ */
		//EXTI->IMR &= ~KB_IRQn_EXTI_LINE;
		
		/* CR95HF is ready to send data */
		xSemaphoreGiveFromISR(xSemaKeybo,&xHigherPriorityTaskWoken);
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

static int keyboard_irq_eve(uint32_t tout)
{
	Dprintf(DBGLVL_Keyboard,"keyboard_irq_eve: enter %lX",tout);
	if (!xSemaphoreTake(xSemaKeybo,tout))
	{
		Dprintf(DBGLVL_Keyboard,"keyboard_irq_eve: return FALSE");
		return FALSE;
	}
	else
		Dprintf(DBGLVL_Keyboard,"keyboard_irq_eve: semaphore event");
	return TRUE;
}

static void keyboard_stroke(void)
{
uint8_t b = 0;

	I2C_RandWrite(KEYBCTRL_ADDR,KBREG_STROKE,1,&b,1);
	Dprintf(DBGLVL_Keyboard,"keyboard_stroke");
}

static void keyboard_waitrelease(void)
{
uint8_t b;

	Dprintf(DBGLVL_Keyboard,"keyboard_waitrelease: enter");
	for (;;)
	{
		keyboard_irq_eve(portMAX_DELAY);
		I2C_RandRead(KEYBCTRL_ADDR,KBREG_MASK,1,&b,1);
		keyboard_stroke();
		if (!b)
		{
			if (!keyboard_irq_eve(KEYRELEASED_TIME))
			{
				Dprintf(DBGLVL_Keyboard,"keyboard_waitrelease: exit");
				return;
			}
			Dprintf(DBGLVL_Keyboard,"keyboard_waitrelease: not enough time");
			keyboard_stroke();
		}
	}
}

void keyboard_init(void)
{
EXTI_InitTypeDef EXTI_InitStructure;

	if (xSemaKeybo)
		return;

	NVIC_DisableIRQ(KBD_IRQn_EXTI_IRQn);

	vSemaphoreCreateBinary(xSemaKeybo);
	xSemaphoreTake(xSemaKeybo,0);		// clear it

	xKeyQueue = xQueueCreate( KeyQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

	/* Enable the Clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);

	NVIC_SetPriority(KBD_IRQn_EXTI_IRQn,KBD_IRQn_EXTI_IRQ_PRIO);

	vSemaphoreCreateBinary(xSemaKeybo);
	xSemaphoreTake(xSemaKeybo,0);		// clear it

	xKeyQueue = xQueueCreate( KeyQueueSize, ( unsigned portBASE_TYPE ) sizeof( signed portCHAR ) );

	/* Configure IRQ pin as input floating */
	GpioInit(KBD_IRQn_PORT,KBD_IRQn_PIN,GPIO_Mode_IN_FLOATING,0);
	
	/* Connect IRQ EXTI Line to GPIO Pin */
	GPIO_EXTILineConfig(KBD_IRQn_PORT_SOURCE, KBD_IRQn_PIN_SOURCE);

	/* Configure CR95HF IRQ EXTI line */
	EXTI_InitStructure.EXTI_Line 		= KBD_IRQn_EXTI_LINE;
	EXTI_InitStructure.EXTI_Mode 		= EXTI_Mode_Interrupt;
	EXTI_InitStructure.EXTI_Trigger 	= EXTI_Trigger_Falling;
	EXTI_InitStructure.EXTI_LineCmd 	= ENABLE;
	EXTI_Init(&EXTI_InitStructure);

	EXTI_ClearITPendingBit(KBD_IRQn_EXTI_LINE);

	xTaskCreate(keyboard_task,"keyboard",256, NULL,tskIDLE_PRIORITY,NULL);
	xTaskCreate(tock_task,"tock",128, NULL,tskIDLE_PRIORITY,&tockhandle);
}

static void keyboard_task(void *par)
{
uint8_t keyb_last,code;

	keyb_last = 0;
	code = 0;

	I2C_RandWrite(KEYBCTRL_ADDR,KBREG_CTRL,1,&keyb_last,1);

	while (!GPIO_ReadInputDataBit(KBD_IRQn_PORT,KBD_IRQn_PIN))
	{
		keyboard_stroke();	
		vTaskDelay(kDec/2);
	}
	xSemaphoreTake(xSemaKeybo,0);

	NVIC_EnableIRQ(KBD_IRQn_EXTI_IRQn);

	for (;;)
	{
		keyboard_irq_eve(portMAX_DELAY);

		if (I2C_RandRead(KEYBCTRL_ADDR,KBREG_MASK,1,&code,1))
		{
			Dprintf(DBGLVL_Keyboard,"keyboard: code=%02X",code);
			keyboard_stroke();
			if (code)
			{
				keyb_last = key_decode(code);
				Dprintf(DBGLVL_Keyboard,"keyboard: %02X",keyb_last);
				if (keyb_last)
				{
					COM_Printf("\r\n*KEYB: %02X,\"%c\"\r\n",code,keyb_last);
					if (!keyboard_irq_eve(KEYPRESSED_TIME))
					{
						// still pressed
						key_putstroke(keyb_last,0);
						tock_signal();
					}
					else
					{
						// released
						keyboard_stroke();
						Dprintf(DBGLVL_Keyboard,"keyboard: invalid pressure time");
						continue;	// already released
					}
				}
				keyboard_waitrelease();
				key_putstroke(keyb_last | KEY_RELEASED,0);
				Dprintf(DBGLVL_Keyboard,"keyboard: released");
			}
		}
	}
}

static void tock_signal(void)
{
	xTaskNotifyGive(tockhandle);
}

#define TOCK_TIME	(kDec/8)
#define NOTOCK_TIME	kDec
static void tock_task(void *par)
{

	for (;;)
	{
		ulTaskNotifyTake(pdFALSE,portMAX_DELAY);

		// buzzer on
		// inserire chiamata a funzione che gestisce le uscite dell'io expander
		IOEXP_set(IOEXP0_BUZZER);
		vTaskDelay(TOCK_TIME);

		// buzzer off
		// inserire chiamata a funzione che gestisce le uscite dell'io expander
		IOEXP_clr(IOEXP0_BUZZER);
		vTaskDelay(NOTOCK_TIME);
	}
}
