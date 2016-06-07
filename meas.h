#ifndef __MEAS_H
#define __MEAS_H

enum CALIB_Enum {
	CALIB_High,
	CALIB_Medium,
	CALIB_Low,
	CALIB_Ptc,
	CALIB_Items
};

#define RES_ADD			300.0

extern const float calib_resist[6];
extern const float calib_condutt[4];

int meas_init(void);
int meas_printInit(void);
int domeas(float *val);
int meas(float *val);
int meas_temp(float *val);
int set_meas_chan(int chan);
float meas_getkcell(void);

#endif
