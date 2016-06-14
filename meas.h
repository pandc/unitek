#ifndef __MEAS_H
#define __MEAS_H

enum CALIB_Enum {
	CALIB_High,
	CALIB_Medium,
	CALIB_Low,
	CALIB_Ptc,
	CALIB_Items
};

#define CALIB_GCable	CALIB_Items
#define RES_ADD			300.0

extern struct meas_calpar_st {
	float ncl,nch,nos,gf,sp;
} mcp[CALIB_Items];
extern uint8_t mcpok;

extern const float calib_resist[6];
extern const float calib_condutt[4];

int meas_init(void);
int meas_printInit(void);
int domeas(float *val,float *pphase);
int meas(float *resist,float *condut,float *conduc);
int meas_temp(float *val);
int set_meas_chan(int chan);
float meas_getkcell(void);

#endif
