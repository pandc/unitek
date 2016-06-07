#include <string.h>
#include "bsp.h"

#include "freertos.h"
#include "task.h"
#include "semphr.h"

#include "irqprio.h"

#include "io.h"
#include "i2c.h"

static xSemaphoreHandle xSemaphoreI2C_Mutex,xSemaphoreI2C_Work;

#define I2C_PORT		GPIOB
#define I2C_SCL_PIN		GPIO_Pin_6
#define I2C_SDA_PIN		GPIO_Pin_7

#define I2C_EVENT_MASTER_REC_BYTE_QUEUED      ((uint32_t)(((I2C_SR2_MSL|I2C_SR2_BUSY)<< 16)|I2C_SR1_BTF|I2C_SR1_RXNE)) /* BUSY, MSL, RXNE and BTF flags*/

void I2C1_EV_Irq(void);
void I2C1_ER_Irq(void);

static volatile uint8_t *i2c_pb,i2c_addr,i2c_err,i2c_done;
volatile uint16_t i2c_cntr;

struct i2c_job_st {
	uint8_t dir;
	uint16_t len;
	uint8_t *buf;
	uint8_t last;
} i2c_jobs[2];

static volatile struct i2c_job_st *job;

#define Transmitter		0x00
#define Receiver		0x01

void I2C1_ER_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	if (I2C1->SR1 & I2C_SR1_BERR)	// Bus error
		I2C1->SR1 &= ~I2C_SR1_BERR;
	if (I2C1->SR1 & I2C_SR1_ARLO)	// Arbitration lost
		I2C1->SR1 &= ~I2C_SR1_ARLO;
	if (I2C1->SR1 & I2C_SR1_AF)		// Acknowledge fail
	{
		I2C1->SR1 &= ~I2C_SR1_AF;
		I2C_GenerateSTOP(I2C1,ENABLE);	// setting stop bit
	}
	if (I2C1->SR1 & I2C_SR1_OVR)		// Overrun
		I2C1->SR1 &= ~I2C_SR1_OVR;
	if (I2C1->SR1 & I2C_SR1_PECERR)		// PEC error
		I2C1->SR1 &= ~I2C_SR1_PECERR;
	if (I2C1->SR1 & I2C_SR1_TIMEOUT)	// SMBus Timeout
		I2C1->SR1 &= ~I2C_SR1_TIMEOUT;
	if (I2C1->SR1 & I2C_SR1_SMBALERT)	// SMBus alert
		I2C1->SR1 &= ~I2C_SR1_SMBALERT;
	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);
	i2c_err = TRUE;
	xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
	portEND_SWITCHING_ISR(xHigherPriorityTaskWoken);
}

void I2C1_EV_IRQHandler(void)
{
portBASE_TYPE xHigherPriorityTaskWoken = pdFALSE;

	i2c_cntr++;
	switch(I2C_GetLastEvent(I2C1))
	{
	case I2C_EVENT_MASTER_MODE_SELECT:
	/* If SB = 1, I2C1 master sent a START on the bus: EV5) */
		// Send the slave address for transmssion or for reception
		// according to the configured value in the write master write routine
		I2C_Send7bitAddress(I2C1, i2c_addr, job->dir);
		break;
//---------------------------------------------------------------------------
//--- TX States -------------------------------------------------------------
//---------------------------------------------------------------------------
	case I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED:
		/* Write the first data in case the Master is Transmitter */
		if (job->dir != I2C_Direction_Transmitter)
		{
			I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);
			i2c_err = TRUE;
			xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
			break;
		}
		/* Initialize the Transmit counter */
		i2c_pb = job->buf;
		/* Write the first data in the data register */
		I2C1->DR = *i2c_pb++;
		/* If no further data to be sent, disable the I2C BUF IT
		in order to not have a TxE  interrupt */
		if (--job->len == 0)
		{
			++job;
			i2c_pb = job->buf;
			if (!job->len)
			{
				/* Disable the BUF IT */
				I2C_ITConfig(I2C1,I2C_IT_BUF,DISABLE);
			}
		}
		break;
	case I2C_EVENT_MASTER_BYTE_TRANSMITTING:
		/* Master transmits the remaing data: from data2 until the last one.  */
		/* If TXE is set */
		/* If there is still data to write */
		if (job->len)
		{
			/* Write the data in DR register */
			I2C1->DR = *i2c_pb++;
			// If  no data remains to write, disable the BUF IT in order
			// to not have again a TxE interrupt
			if (--job->len == 0)
			{
				if (job->last)
				{
					/* Disable the BUF IT */
					I2C_ITConfig(I2C1,I2C_IT_BUF,DISABLE);
				}
				else
				{
					job++;
					i2c_pb = job->buf;
				}
			}
		}
		break;
	case I2C_EVENT_MASTER_BYTE_TRANSMITTED:
		/* If BTF and TXE are set (EV8_2), program the STOP */
		/* Program the STOP */
		I2C_GenerateSTOP(I2C1,ENABLE);
		/* Disable EVT IT In order to not have again a BTF IT */
		I2C_ITConfig(I2C1,I2C_IT_EVT | I2C_IT_BUF,DISABLE);
		i2c_done = TRUE;
		xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
		break;
//---------------------------------------------------------------------------
//--- RX States -------------------------------------------------------------
//---------------------------------------------------------------------------
	case I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED:
		if (job->dir != I2C_Direction_Receiver)
		{
			I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);
			i2c_err = TRUE;
			xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
			break;
		}
		/* Initialize Receive counter */
		i2c_pb = job->buf;
		/* At this stage, ADDR is cleared because both SR1 and SR2 were read.*/
		/* EV6_1: used for single byte reception. The ACK disable and the STOP
		Programming should be done just after ADDR is cleared. */
		switch(job->len)
		{
		case 1:
			/* Clear ACK */
			I2C_AcknowledgeConfig(I2C1,DISABLE);
			/* Program the STOP */
			I2C_GenerateSTOP(I2C1,ENABLE);
			break;
		case 2:
			/* Clear ACK */
			I2C_AcknowledgeConfig(I2C1,DISABLE);
			/* Disable the ITBUF in order to have only the BTF interrupt */
			I2C_ITConfig(I2C1,I2C_IT_BUF,DISABLE);
			break;
		}
		break;
	case I2C_EVENT_MASTER_BYTE_RECEIVED:
		// RXNE is set
		if (job->len != 3)
		{
			*i2c_pb++ = I2C1->DR;
			if (--job->len == 3)
				I2C_ITConfig(I2C1,I2C_IT_BUF,DISABLE);
			else if (job->len == 0)
			{
				I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);
				i2c_done = TRUE;
				xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
			}
		}
		break;
	case I2C_EVENT_MASTER_REC_BYTE_QUEUED:
		// RXNE and BTF
		switch(job->len)
		{
		case 3:
			I2C_ITConfig(I2C1,I2C_IT_BUF,ENABLE);
			I2C_AcknowledgeConfig(I2C1,DISABLE);
			// Read data N-2
			*i2c_pb++ = I2C1->DR;
			I2C_GenerateSTOP(I2C1,ENABLE);
			// Read data N-1
			*i2c_pb++ = I2C1->DR;
			job->len -= 2;
			// ready to read last byte on next EV7
			break;
		case 2:
			I2C_GenerateSTOP(I2C1,ENABLE);
			// Read data N-1
			*i2c_pb++ = I2C1->DR;
			// Read data N
			*i2c_pb++ = I2C1->DR;
			job->len = 0;
			I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);
			i2c_done = TRUE;
			xSemaphoreGiveFromISR(xSemaphoreI2C_Work,&xHigherPriorityTaskWoken);
			break;
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
	GPIO_ResetBits(I2C_PORT,I2C_SCL_PIN);	/* SCL -> 0 */
	GPIO_ResetBits(I2C_PORT,I2C_SDA_PIN);	/* SDA->0 set bit 4 of P3 to 0 */
	i2c_delay();
	GPIO_SetBits(I2C_PORT,I2C_SCL_PIN);		/* SCL -> 1 */
	i2c_delay();
	GPIO_SetBits(I2C_PORT,I2C_SDA_PIN);		/* SDA 0->1 set bit 4 of P3 to 1 */
}

static void i2c_reset(void)
{
uint16_t i;

	/* Configure I2C1 pins: SCL and SDA */
	/* Output value must be set to 1 to not drive lines low... We set */
	/* SCL first, to ensure it is high before changing SDA. */
	GpioInit(I2C_PORT,I2C_SCL_PIN | I2C_SDA_PIN,GPIO_Mode_Out_OD,1);

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
		GPIO_ResetBits(I2C_PORT,I2C_SCL_PIN);	/* SCL -> 0 */
		i2c_semidelay();
		GPIO_SetBits(I2C_PORT,I2C_SCL_PIN);		/* SCL -> 1 */
	}
	i2c_stop();
}

/*******************************************************************************
* Function Name  : I2C_Configuration
* Description    : I2C Configuration
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
#define I2C_Speed              400000
#define I2C1_SLAVE_ADDRESS7    0xA0
void I2C_Open(void)
{
I2C_InitTypeDef  I2C_InitStructure; 

	NVIC_DisableIRQ(I2C1_EV_IRQn);
	NVIC_DisableIRQ(I2C1_ER_IRQn);
	/* Enable peripherals I2C clocks */
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE); // value for the EVAL BOARD

	/* Disable I2C1 event and buffer interrupts */
	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	i2c_reset();
	GpioInit(I2C_PORT,I2C_SCL_PIN | I2C_SDA_PIN,GPIO_Mode_AF_OD,0);

	/* Enable I2C1 reset state */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	/* Release I2C1 from reset state */
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);

	NVIC_SetPriority(I2C1_EV_IRQn,I2C1_EV_IRQ_PRIO);
	NVIC_SetPriority(I2C1_ER_IRQn,I2C1_ER_IRQ_PRIO);

	/* I2C configuration */
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = I2C1_SLAVE_ADDRESS7;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_Speed;

	/* I2C Peripheral Enable */
	I2C_Cmd(I2C1, ENABLE);
	/* Apply I2C configuration after enabling it */
	I2C_Init(I2C1, &I2C_InitStructure);

	xSemaphoreI2C_Mutex = xSemaphoreCreateMutex();
	vSemaphoreCreateBinary(xSemaphoreI2C_Work);
	xSemaphoreTake(xSemaphoreI2C_Work,0);		// clear it
	NVIC_EnableIRQ(I2C1_EV_IRQn);
	NVIC_EnableIRQ(I2C1_ER_IRQn);
}

int I2C_RandWrite(uint8_t slave,uint16_t addr,uint8_t addrlen,uint8_t *pbuffer, uint16_t len)
{
uint8_t buf_addr[2];

	if (slave & 0x80)
		return FALSE;
	if (addrlen == 1)
		*buf_addr = addr;
	else
	{
		*buf_addr = (uint8_t)(addr >> 8);
		buf_addr[1] = (uint8_t)(addr & 0xff);
	}
	xSemaphoreTake(xSemaphoreI2C_Mutex,portMAX_DELAY);

	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
	{
		xSemaphoreGive(xSemaphoreI2C_Mutex);
		return FALSE;
	}

	i2c_cntr = 0;
	job = i2c_jobs;
	i2c_addr = slave << 1;
	memset(i2c_jobs,0,sizeof(i2c_jobs));
	i2c_jobs[0].buf = buf_addr;
	i2c_jobs[0].len = addrlen;
	i2c_jobs[0].dir = I2C_Direction_Transmitter;

	i2c_jobs[1].buf = pbuffer;
	i2c_jobs[1].len = len;
	i2c_jobs[1].dir = I2C_Direction_Transmitter;
	i2c_jobs[1].last = TRUE;
		
	i2c_done = i2c_err = FALSE;

	// clear the semaphore
	while (xSemaphoreTake(xSemaphoreI2C_Work,0));

	I2C_NACKPositionConfig(I2C1,I2C_NACKPosition_Current);
	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,ENABLE);

	/* Send START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	while (!i2c_done && !i2c_err)
		xSemaphoreTake(xSemaphoreI2C_Work,portMAX_DELAY);

	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	xSemaphoreGive(xSemaphoreI2C_Mutex);
	return !i2c_err;
}

int I2C_RandRead(uint8_t slave,uint16_t addr,uint8_t addrlen,uint8_t *pbuffer,uint16_t len)
{
uint8_t buf_addr[2];

	if (slave & 0x80)
		return FALSE;
	if (addrlen == 1)
		*buf_addr = addr;
	else
	{
		*buf_addr = (uint8_t)(addr >> 8);
		buf_addr[1] = (uint8_t)(addr & 0xff);
	}
	xSemaphoreTake(xSemaphoreI2C_Mutex,portMAX_DELAY);

	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
	{
		xSemaphoreGive(xSemaphoreI2C_Mutex);
		return FALSE;
	}

	memset(i2c_jobs,0,sizeof(i2c_jobs));
	i2c_cntr = 0;
	job = i2c_jobs;
	i2c_addr = slave << 1;
	job->buf = buf_addr;
	job->len = addrlen;
	job->dir = I2C_Direction_Transmitter;
	job->last = TRUE;

	i2c_done = i2c_err = FALSE;

	// clear the semaphore
	while (xSemaphoreTake(xSemaphoreI2C_Work,0));

	I2C_NACKPositionConfig(I2C1,I2C_NACKPosition_Current);
	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,ENABLE);

	/* Send START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	while (!i2c_done && !i2c_err)
		xSemaphoreTake(xSemaphoreI2C_Work,portMAX_DELAY);

	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);
	if (i2c_err)
	{
		xSemaphoreGive(xSemaphoreI2C_Mutex);
		return FALSE;
	}

	memset(i2c_jobs,0,sizeof(i2c_jobs));
	i2c_cntr = 0;
	job = i2c_jobs;
	job->buf = pbuffer;
	job->len = len;
	job->dir = I2C_Direction_Receiver;
	job->last = TRUE;

	i2c_done = i2c_err = FALSE;

	// clear the semaphore
	while (xSemaphoreTake(xSemaphoreI2C_Work,0));

	if (len == 2)
		I2C_NACKPositionConfig(I2C1,I2C_NACKPosition_Next);
	else
		I2C_NACKPositionConfig(I2C1,I2C_NACKPosition_Current);
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,ENABLE);

	/* Send START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	while (!i2c_done && !i2c_err)
		xSemaphoreTake(xSemaphoreI2C_Work,portMAX_DELAY);

	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	xSemaphoreGive(xSemaphoreI2C_Mutex);
	return !i2c_err;
}

int I2C_Read(uint8_t slave,uint8_t *pbuffer,uint16_t len)
{
struct i2c_job_st i2c_job;

	xSemaphoreTake(xSemaphoreI2C_Mutex,portMAX_DELAY);

	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
	{
		xSemaphoreGive(xSemaphoreI2C_Mutex);
		return FALSE;
	}

	i2c_cntr = 0;
	job = &i2c_job;
	i2c_addr = slave << 1;
	i2c_job.buf = pbuffer;
	i2c_job.len = len;
	i2c_job.dir = I2C_Direction_Receiver;
	i2c_job.last = TRUE;

	i2c_done = i2c_err = FALSE;

	// clear the semaphore
	while (xSemaphoreTake(xSemaphoreI2C_Work,0));

	if (len == 2)
		I2C_NACKPositionConfig(I2C1,I2C_NACKPosition_Next);
	else
		I2C_NACKPositionConfig(I2C1,I2C_NACKPosition_Current);
	I2C_AcknowledgeConfig(I2C1,ENABLE);
	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,ENABLE);

	/* Send START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	while (!i2c_done && !i2c_err)
		xSemaphoreTake(xSemaphoreI2C_Work,portMAX_DELAY);

	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	xSemaphoreGive(xSemaphoreI2C_Mutex);
	return !i2c_err;
}

int I2C_Write(uint8_t slave,uint8_t *pbuffer, uint16_t len)
{
struct i2c_job_st i2c_job;

	xSemaphoreTake(xSemaphoreI2C_Mutex,portMAX_DELAY);

	if (I2C_GetFlagStatus(I2C1, I2C_FLAG_BUSY))
	{
		xSemaphoreGive(xSemaphoreI2C_Mutex);
		return FALSE;
	}

	i2c_cntr = 0;
	job = &i2c_job;
	i2c_addr = slave << 1;
	i2c_job.buf = pbuffer;
	i2c_job.len = len;
	i2c_job.dir = I2C_Direction_Transmitter;
	i2c_job.last = TRUE;
		
	i2c_done = i2c_err = FALSE;

	// clear the semaphore
	while (xSemaphoreTake(xSemaphoreI2C_Work,0));

	I2C_NACKPositionConfig(I2C1,I2C_NACKPosition_Current);
	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,ENABLE);

	/* Send START condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	while (!i2c_done && !i2c_err)
		xSemaphoreTake(xSemaphoreI2C_Work,portMAX_DELAY);

	I2C_ITConfig(I2C1,I2C_IT_BUF | I2C_IT_ERR | I2C_IT_EVT,DISABLE);

	xSemaphoreGive(xSemaphoreI2C_Mutex);
	return !i2c_err;
}
