#ifndef __DISPLAY_INTERFACE_H
#define __DISPLAY_INTERFACE_H

#define CHIP_RESET()		GPIO_ResetBits(DISP_RSTn_PORT,DISP_RSTn_PIN)
#define CHIP_UNRESET()		GPIO_SetBits(DISP_RSTn_PORT,DISP_RSTn_PIN)
#define CHIP_VCC_ON()		GPIO_SetBits(DISP_VCC_ON_PORT,DISP_VCC_ON_PIN)
#define CHIP_VCC_OFF()		GPIO_ResetBits(DISP_VCC_ON_PORT,DISP_VCC_ON_PIN)

void LCD_Init_Spi(void);
void LCD_Send_Command_Bytes(const uint8_t *d,uint16_t len);
void LCD_Send_Command(uint8_t cmd,uint8_t data);
void LCD_Send_Reg(uint8_t r);
void LCD_Send_Data(const uint8_t *d,uint16_t c,uint8_t inc);

#endif
