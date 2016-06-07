#ifndef __DATETIME_H
#define __DATETIME_H

extern const char *MonthNames[];
extern const char *WeekDayNames[];

void tzone_load(void);
char const * __getzone(void);

#endif
