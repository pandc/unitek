#include <string.h>
#include "stm32f10x.h"

#include "freertos.h"
#include "task.h"
#include "semphr.h"

#include "irqprio.h"

#include "types.h"
#include "bsp.h"
#include "io.h"
#include "com.h"
#include "i2c.h"

#define DEFAULT_I2C_FREQ	MAX_I2C_FREQ

#define SEMA_DELAY	kSec

static xSemaphoreHandle xSemaphoreI2C_Mutex,xSemaphoreI2C_Work;

#define I2Cx_PORT					GPIOB
#define I2Cx_SCL_PIN				GPIO_Pin_6
#define I2Cx_SDA_PIN				GPIO_Pin_7

#define I2Cx						I2C1
#define I2Cx_CLK					RCC_APB1Periph_I2C1
#define I2Cx_EV_IRQn				I2C1_EV_IRQn
#define I2Cx_ER_IRQn				I2C1_ER_IRQn
#define I2Cx_EV_IRQHandler			I2C1_EV_IRQHandler
#define I2Cx_ER_IRQHandler			I2C1_ER_IRQHandler
#define I2Cx_EV_IRQ_PRIO			I2C1_EV_IRQ_PRIO
#define I2Cx_ER_IRQ_PRIO			I2C1_ER_IRQ_PRIO

#define I2C_EVENT_MASTER_REC_BYTE_QUEUED      ((uint32_t)(((I2C_SR2_MSL|I2C_SR2_BUSY)<< 16)|I2C_SR1_BTF|I2C_SR1_RXNE)) /* BUSY, MSL, RXNE and BTF flags*/

void I2Cx_EV_Irq(void);
void I2Cx_ER_Irq(void);

static volatile uint8_t *i2c_pb,i2c_addr,i2c_err,i2c_done,i2c_oper;
volatile uint16_t i2c_error_flags;
static volatile uint16_t i2c_cntr,i2c_len;
static uint32_t i2c_freq = DEFAULT_I2C_FREQ;

struct i2c_job_st {
	uint8_t dir;
	uint16_t len;
	uint8_t *buf;
	uint8_t last;
};

static volatile struct i2c_job_st *job;

static int I2C_isBusy(void)
{
int i;

	if (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
		return FALSE;

	for (i = 0; i < 10; i++)
	{
		vTaskDelay(1);
		if (!I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY))
			return FALSE;
	}
	return TRUE;
} 

void I2Cx_ER_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (I2Cx->SR1 & I2C_SR1_BERR)	// Bus error
	{
		I2Cx->SR1 &= ~I2C_SR1_BERR;
		i2c_error_flags |= I2C_SR1_BERR;
	}
	if (I2Cx->SR1 & I2C_SR1_ARLO)	// Arbitration lost
	{
		I2Cx->SR1 &= ~I2C_SR1_ARLO;
		i2c_error_flags |= I2C_SR1_ARLO;
	}
	if (I2Cx->SR1 & I2C_SR1_AF)		// Acknowledge fail
	{
		I2Cx->SR1 &= ~I2C_SR1_AF;
		I2C_GenerateSTOP(I2Cx,ENABLE);	// setting stop bit
		i2c_error_flags |= I2C_SR1_AF;
	}
	if (I2Cx->SR1 & I2C_SR1_OVR)		// Overrun
	{
		I2Cx->SR1 &= ~I2C_SR1_OVR;
		i2c_error_flags |= I2C_SR1_OVR;
	}
	if (I2Cx->SR1 & I2C_SR1_PECERR)		// PEC error
	{
		I2Cx->SR1 &= ~I2C_SR1_PECERR;
		i2c_error_flags |= I2C_SR1_PECERR;
	}
	if (I2Cx->SR1 & I2C_SR1_TIMEOUT)	// SMBus Timeout
	{
		I2Cx->SR1 &= ~I2C_SR1_TIMEOUT;
		i2c_error_flags |= I2C_SR1_TIMEOUT;
	}
	if (I2Cx->SR1 & I2C_SR1_SMBALERT)	// SMBus alert
	{
		I2Cx->SR1 &= ~I2C_SR1_SMBALERT;
		i2c_error_flags |= I2C_SR1_SMBALERT;
	}
	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);
	i2c_err = TRUE;
	xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void I2Cx_EV_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;
uint32_t lasteve;

	i2c_cntr++;
	switch(i2c_oper)
	{
	case I2C_Direction_Transmitter:
		lasteve = I2C_GetLastEvent(I2Cx);
		switch(lasteve)
		{
		case I2C_EVENT_MASTER_MODE_SELECT:
		/* If SB = 1, I2Cx master sent a START on the bus: EV5) */
			// Send the slave address for transmssion or for reception
			// according to the configured value in the write master write routine
			I2C_Send7bitAddress(I2Cx, i2c_addr, job->dir);
			break;
	//---------------------------------------------------------------------------
	//--- TX States -------------------------------------------------------------
	//---------------------------------------------------------------------------
		case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
			/* Initialize the Transmit counter */
			i2c_pb = job->buf;
			i2c_len = job->len;
			/* Write the first data in the data register */
			I2Cx->DR = *i2c_pb++;
			/* If no further data to be sent, disable the I2C BUF IT
			in order to not have a TxE  interrupt */
			if (--i2c_len == 0)
			{
				if (job->last || (job[1].dir != I2C_Direction_Transmitter))
				{
					/* Disable the BUF IT in order to wait the BTF event */
					I2C_ITConfig(I2Cx,I2C_IT_BUF,DISABLE);
				}
				else
				{
					job++;
					i2c_pb = job->buf;
					i2c_len = job->len;
				}
			}
			break;
		case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
			/* Master transmits the remaing data: from data2 until the last one.  */
			/* If TXE is set */
			/* If there is still data to write */
			if (i2c_len)
			{
				/* Write the data in DR register */
				I2Cx->DR = *i2c_pb++;
				// If  no data remains to write, disable the BUF IT in order
				// to not have again a TxE interrupt
				if (--i2c_len == 0)
				{
					if (job->last || (job[1].dir != I2C_Direction_Transmitter))
					{
						/* Disable the BUF IT */
						I2C_ITConfig(I2Cx,I2C_IT_BUF,DISABLE);
					}
					else
					{
						job++;
						i2c_pb = job->buf;
						i2c_len = job->len;
					}
				}
			}
			break;
		case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
			/* If BTF and TXE are set (EV8_2), program the STOP */
			if (job->last)
			{
				/* Program the STOP */
				I2C_GenerateSTOP(I2Cx,ENABLE);
				/* Disable EVT IT In order to not have again a BTF IT */
				I2C_ITConfig(I2Cx,I2C_IT_EVT | I2C_IT_BUF,DISABLE);
				i2c_done = TRUE;
				xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
			}
			else
			{
				job++;
				i2c_oper = job->dir;
				/* Send START condition */
				I2C_GenerateSTART(I2Cx, ENABLE);
			}
			break;
		}
		break;
	//---------------------------------------------------------------------------
	//--- RX States -------------------------------------------------------------
	//---------------------------------------------------------------------------
	case I2C_Direction_Receiver:
		if (I2C_GetITStatus(I2Cx,I2C_IT_SB) == SET)
		{
			/* Send slave Address for read */
			I2C_Send7bitAddress(I2Cx, i2c_addr, I2C_Direction_Receiver);
			i2c_pb = job->buf;
			i2c_len = job->len;
			if (i2c_len == 3)
			{
				/* Disable buffer Interrupts */
				I2C_ITConfig(I2Cx, I2C_IT_BUF , DISABLE);
			}
			else
			{
				/* Enable buffer Interrupts */
				I2C_ITConfig(I2Cx, I2C_IT_BUF , ENABLE);
			}
		}
		else if (I2C_GetITStatus(I2Cx, I2C_IT_ADDR) == SET)
		{
			if (i2c_len == 1)
			{
				I2C_AcknowledgeConfig(I2Cx, DISABLE);
			}
			/* Clear ADDR Register */
			(void)(I2Cx->SR1);
			(void)(I2Cx->SR2);
			if (i2c_len == 1)
			{
				I2C_GenerateSTOP(I2Cx, ENABLE);
			}
			else if (i2c_len == 2)
			{
				I2C_AcknowledgeConfig(I2Cx, DISABLE);
				I2C_NACKPositionConfig(I2Cx, I2C_NACKPosition_Next);
				/* Disable buffer Interrupts */
				I2C_ITConfig(I2Cx, I2C_IT_BUF , DISABLE);
			}
		}
		else if ((I2C_GetITStatus(I2Cx, I2C_IT_RXNE) == SET) && (I2C_GetITStatus(I2Cx, I2C_IT_BTF) == RESET))
		{
			/* Store I2C received data */
			*i2c_pb++ = I2C_ReceiveData (I2Cx);
			i2c_len--;

			if (i2c_len == 3)
			{
				/* Disable buffer Interrupts */
				I2C_ITConfig(I2Cx, I2C_IT_BUF , DISABLE);
			}
			else if (i2c_len == 0)
			{
				/* Disable Error, Event and Buffer Interrupts */
				I2C_ITConfig(I2Cx,I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
				i2c_done = TRUE;
				xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
			}
		}    
		/* BUSY, MSL and RXNE flags */
		else if (I2C_GetITStatus(I2Cx, I2C_IT_BTF) == SET)
		{
			/* if Three bytes remaining for reception */
			if (i2c_len == 3)
			{
				I2C_AcknowledgeConfig(I2Cx, DISABLE);
				/* Store I2C received data */
				*i2c_pb++ = I2C_ReceiveData(I2Cx);
				i2c_len--;
			} 
			else if (i2c_len == 2)
			{           
				I2C_GenerateSTOP(I2Cx, ENABLE);    

				/* Store I2C received data */
				*i2c_pb++ = I2C_ReceiveData(I2Cx);
				i2c_len--;
				/* Store I2C received data */
				*i2c_pb++ = I2C_ReceiveData(I2Cx);
				i2c_len--;
				/* Disable Error, Event and Buffer Interrupts */
				I2C_ITConfig(I2Cx,I2C_IT_EVT | I2C_IT_BUF | I2C_IT_ERR, DISABLE);
				i2c_done = TRUE;
				xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
			}
			else 
			{
				/* Store I2C received data */
				*i2c_pb++ = I2C_ReceiveData(I2Cx);
				i2c_len--;
			}
		}
		break;
	}
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

static void i2c_dummy(void)
{
	i2c_cntr++;
}

static void i2c_semidelay(void)
{
uint16_t j;

	for (j = 0; j < 10; j++)
		i2c_dummy();
}

static void i2c_delay(void)
{
	i2c_semidelay();
	i2c_semidelay();
}

static void i2c_stop(void)
{
	GPIO_ResetBits(I2Cx_PORT,I2Cx_SCL_PIN);	/* SCL -> 0 */
	GPIO_ResetBits(I2Cx_PORT,I2Cx_SDA_PIN);	/* SDA->0 set bit 4 of P3 to 0 */
	i2c_delay();
	GPIO_SetBits(I2Cx_PORT,I2Cx_SCL_PIN);		/* SCL -> 1 */
	i2c_delay();
	GPIO_SetBits(I2Cx_PORT,I2Cx_SDA_PIN);		/* SDA 0->1 set bit 4 of P3 to 1 */
}

static void i2c_reset(void)
{
uint16_t i;

	/* Configure I2Cx pins: SCL and SDA */
	/* Output value must be set to 1 to not drive lines low... We set */
	/* SCL first, to ensure it is high before changing SDA. */
	GpioInit(I2Cx_PORT,I2Cx_SCL_PIN | I2Cx_SDA_PIN,GPIO_Mode_Out_OD,1);

	/* In some situations (after a reset during an I2C transfer), the slave */
	/* device may be left in an unknown state. Send 9 clock pulses just in case. */
	for (i = 0; i < 9; i++)
	{
		/*
		* TBD: Seems to be clocking at appr 80kHz-120kHz depending on compiler
		* optimization when running at 14MHz. A bit high for standard mode devices,
		* but DVK only has fast mode devices. Need however to add some time
		* measurement in order to not be dependable on frequency and code executed.
		*/
		i2c_semidelay();
		GPIO_ResetBits(I2Cx_PORT,I2Cx_SCL_PIN);	/* SCL -> 0 */
		i2c_semidelay();
		GPIO_SetBits(I2Cx_PORT,I2Cx_SCL_PIN);		/* SCL -> 1 */
	}
	i2c_stop();
}

uint32_t I2C_GetFreq(void)
{
	return i2c_freq;
}

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : I2C Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define I2Cx_SLAVE_ADDRESS7    0xA0
void I2C_Open(uint32_t freq)
{
I2C_InitTypeDef  I2C_InitStructure; 

	if (freq != 0)
	{
		if ((freq < MIN_I2C_FREQ) || (freq > MAX_I2C_FREQ))
			return;
		if (xSemaphoreI2C_Mutex)
			xSemaphoreTake(xSemaphoreI2C_Mutex,portMAX_DELAY);
		i2c_freq = freq;
		if (!xSemaphoreI2C_Mutex)
			freq = 0;	// don't release mutex on exit
	}
	NVIC_DisableIRQ(I2Cx_EV_IRQn);
	NVIC_DisableIRQ(I2Cx_ER_IRQn);

	if (!xSemaphoreI2C_Mutex)
		xSemaphoreI2C_Mutex = xSemaphoreCreateMutex();
	if (!xSemaphoreI2C_Work)
	{
		vSemaphoreCreateBinary(xSemaphoreI2C_Work);
		xSemaphoreTake(xSemaphoreI2C_Work,0);		// clear it
	}

	/* Enable peripherals I2C clocks */
	RCC_APB1PeriphClockCmd(I2Cx_CLK, ENABLE); // value for the EVAL BOARD

	/* Disable I2Cx event and buffer interrupts */
	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	i2c_reset();
	GpioInit(I2Cx_PORT,I2Cx_SCL_PIN | I2Cx_SDA_PIN,GPIO_Mode_AF_OD,0);

	/* Enable I2Cx reset state */
	RCC_APB1PeriphResetCmd(I2Cx_CLK, ENABLE);
	/* Release I2Cx from reset state */
	RCC_APB1PeriphResetCmd(I2Cx_CLK, DISABLE);

	NVIC_SetPriority(I2Cx_EV_IRQn,I2Cx_EV_IRQ_PRIO);
	NVIC_SetPriority(I2Cx_ER_IRQn,I2Cx_ER_IRQ_PRIO);

	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = I2Cx_SLAVE_ADDRESS7;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = i2c_freq;

	/* I2C Peripheral Enable */
	I2C_Cmd(I2Cx, ENABLE);
	/* Apply I2C configuration after enabling it */
	I2C_Init(I2Cx, &I2C_InitStructure);

	xSemaphoreI2C_Mutex = xSemaphoreCreateMutex();
	vSemaphoreCreateBinary(xSemaphoreI2C_Work);
	xSemaphoreTake(xSemaphoreI2C_Work,0);		// clear it
	NVIC_EnableIRQ(I2Cx_EV_IRQn);
	NVIC_EnableIRQ(I2Cx_ER_IRQn);
	if (freq)
		xSemaphoreGive(xSemaphoreI2C_Mutex);
}

int I2C_RandWrite(uint8_t slave,uint16_t addr,uint8_t addrsize,const void *pbuffer, uint16_t len)
{
struct i2c_job_st i2c_jobs[2];
uint8_t buf_offset[2];

	xSemaphoreTake(xSemaphoreI2C_Mutex,portMAX_DELAY);

	if (I2C_isBusy())
	{
		i2c_error_flags = I2C_SR1_SB;
		xSemaphoreGive(xSemaphoreI2C_Mutex);
		return FALSE;
	}

	i2c_cntr = 0;
	i2c_oper = I2C_Direction_Transmitter;
	job = i2c_jobs;
	i2c_addr = slave << 1;
	memset(i2c_jobs,0,sizeof(i2c_jobs));
	if (addrsize == 1)
		buf_offset[0] = (uint8_t)addr;
	else
	{
		buf_offset[0] = (uint8_t)(addr >> 8);
		buf_offset[1] = (uint8_t)(addr & 0xff);
	}
	i2c_jobs[0].buf = buf_offset;
	i2c_jobs[0].len = addrsize;
	i2c_jobs[0].dir = I2C_Direction_Transmitter;

	i2c_jobs[1].buf = (uint8_t *)pbuffer;
	i2c_jobs[1].len = len;
	i2c_jobs[1].dir = I2C_Direction_Transmitter;
	i2c_jobs[1].last = TRUE;

	i2c_error_flags = 0;
	i2c_done = i2c_err = FALSE;

	// clear the semaphore
	while (xSemaphoreTake(xSemaphoreI2C_Work,0));

	I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);
	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,ENABLE);

	/* Send START condition */
	I2C_GenerateSTART(I2Cx, ENABLE);

	while (!i2c_done && !i2c_err)
	{
		if (!xSemaphoreTake(xSemaphoreI2C_Work,SEMA_DELAY))
		{
			I2C_Open(0);
			i2c_err = TRUE;
			break;
		}
	}

	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	xSemaphoreGive(xSemaphoreI2C_Mutex);
	return !i2c_err;
}

int I2C_RandRead(uint8_t slave,uint16_t addr,uint8_t addrsize,void *pbuffer,uint16_t len)
{
struct i2c_job_st i2c_jobs[2];
uint8_t buf_offset[2];

	xSemaphoreTake(xSemaphoreI2C_Mutex,portMAX_DELAY);

	if (I2C_isBusy())
	{
		i2c_error_flags = I2C_SR1_SB;
		xSemaphoreGive(xSemaphoreI2C_Mutex);
		return FALSE;
	}

	i2c_cntr = 0;
	i2c_oper = I2C_Direction_Transmitter;
	job = i2c_jobs;
	i2c_addr = slave << 1;
	memset(i2c_jobs,0,sizeof(i2c_jobs));
	if (addrsize == 1)
		buf_offset[0] = (uint8_t)addr;
	else
	{
		buf_offset[0] = (uint8_t)(addr >> 8);
		buf_offset[1] = (uint8_t)(addr & 0xff);
	}

	i2c_jobs[0].buf = buf_offset;
	i2c_jobs[0].len = addrsize;
	i2c_jobs[0].dir = I2C_Direction_Transmitter;

	i2c_jobs[1].buf = (uint8_t *)pbuffer;
	i2c_jobs[1].len = len;
	i2c_jobs[1].dir = I2C_Direction_Receiver;
	i2c_jobs[1].last = TRUE;

	i2c_error_flags = 0;
	i2c_done = i2c_err = FALSE;

	// clear the semaphore
	while (xSemaphoreTake(xSemaphoreI2C_Work,0));

	I2C_AcknowledgeConfig(I2Cx,ENABLE);
	I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);
	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,ENABLE);

	/* Send START condition */
	I2C_GenerateSTART(I2Cx, ENABLE);

	while (!i2c_done && !i2c_err)
	{
		if (!xSemaphoreTake(xSemaphoreI2C_Work,SEMA_DELAY))
		{
			I2C_Open(0);
			i2c_err = TRUE;
			break;
		}
	}

	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	xSemaphoreGive(xSemaphoreI2C_Mutex);
	return !i2c_err;
}

int I2C_Read(uint8_t addr,void *pbuffer,uint16_t len)
{
struct i2c_job_st i2c_job;

	xSemaphoreTake(xSemaphoreI2C_Mutex,portMAX_DELAY);

	if (I2C_isBusy())
	{
		i2c_error_flags = I2C_SR1_SB;
		xSemaphoreGive(xSemaphoreI2C_Mutex);
		return FALSE;
	}

	i2c_cntr = 0;
	i2c_oper = I2C_Direction_Receiver;
	job = &i2c_job;
	i2c_addr = addr << 1;
	i2c_job.buf = (uint8_t *)pbuffer;
	i2c_job.len = len;
	i2c_job.dir = I2C_Direction_Receiver;
	i2c_job.last = TRUE;

	i2c_done = i2c_err = FALSE;

	// clear the semaphore
	while (xSemaphoreTake(xSemaphoreI2C_Work,0));

	if ((i2c_oper == I2C_Direction_Receiver) && (len == 2))
		I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Next);
	else
		I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);
	I2C_AcknowledgeConfig(I2Cx,ENABLE);
	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,ENABLE);

	/* Send START condition */
	I2C_GenerateSTART(I2Cx, ENABLE);

	while (!i2c_done && !i2c_err)
	{
		if (!xSemaphoreTake(xSemaphoreI2C_Work,SEMA_DELAY))
		{
			I2C_Open(0);
			i2c_err = TRUE;
			break;
		}
	}

	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	xSemaphoreGive(xSemaphoreI2C_Mutex);
	return !i2c_err;
}

int I2C_Write(uint8_t addr,void *pbuffer, uint16_t len)
{
struct i2c_job_st i2c_job;

	xSemaphoreTake(xSemaphoreI2C_Mutex,portMAX_DELAY);

	if (I2C_isBusy())
	{
		i2c_error_flags = I2C_SR1_SB;
		xSemaphoreGive(xSemaphoreI2C_Mutex);
		return FALSE;
	}

	i2c_cntr = 0;
	i2c_oper = I2C_Direction_Transmitter;
	job = &i2c_job;
	i2c_addr = addr << 1;
	i2c_job.buf = (uint8_t *)pbuffer;
	i2c_job.len = len;
	i2c_job.dir = I2C_Direction_Transmitter;
	i2c_job.last = TRUE;
		
	i2c_done = i2c_err = FALSE;

	// clear the semaphore
	while (xSemaphoreTake(xSemaphoreI2C_Work,0));

	I2C_NACKPositionConfig(I2Cx,I2C_NACKPosition_Current);
	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,ENABLE);

	/* Send START condition */
	I2C_GenerateSTART(I2Cx, ENABLE);

	while (!i2c_done && !i2c_err)
	{
		if (!xSemaphoreTake(xSemaphoreI2C_Work,SEMA_DELAY))
		{
			I2C_Open(0);
			i2c_err = TRUE;
			break;
		}
	}

	I2C_ITConfig(I2Cx,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	xSemaphoreGive(xSemaphoreI2C_Mutex);
	return !i2c_err;
}

void I2C_PrintErrorFlags(void)
{
	if (!i2c_error_flags)
	{
		COM_Printf("\r\nNo I2C error\r\n");
		return;
	}
	COM_Printf("\r\nI2C failure: %04X",i2c_error_flags);

	if (i2c_error_flags & I2C_SR1_SB)
		COM_Puts(", Bus busy");
	if (i2c_error_flags & I2C_SR1_BERR)
		COM_Puts(", Bus error");
	if (i2c_error_flags & I2C_SR1_ARLO)
		COM_Puts(", Arbitration lost");
	if (i2c_error_flags & I2C_SR1_AF)
		COM_Puts(", Acknowledge fail");
	if (i2c_error_flags & I2C_SR1_OVR)
		COM_Puts(", Overrun");
	if (i2c_error_flags & I2C_SR1_PECERR)
		COM_Puts(", PEC error");
	if (i2c_error_flags & I2C_SR1_TIMEOUT)
		COM_Puts(", SMBus timeout");
	if (i2c_error_flags & I2C_SR1_SMBALERT)
		COM_Puts(", SMBus alert");
	COM_Puts("\r\n");
}
