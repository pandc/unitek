#ifndef __IOEXP_H
#define __IOEXP_H

#define IOEXP0_BUZZER			(1 << 0)
#define IOEXP0_TANK_ALARM		(1 << 1)
#define IOEXP0_MIN_CONC_ALARM	(1 << 2)
#define IOEXP0_PUMP_ENABLE		(1 << 3)
#define IOEXP0_MAX_CONC_ALARM	(1 << 4)
#define IOEXP0_MIN_TEMP_ALARM	(1 << 5)
#define IOEXP0_HEATER_ENABLE	(1 << 6)
#define IOEXP0_MAX_TEMP_ALARM	(1 << 7)

uint8_t IOEXP_get(void);
void IOEXP_set(uint8_t mask);
void IOEXP_clr(uint8_t mask);
void IOEXP_init(void);

#endif
