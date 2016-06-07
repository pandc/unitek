#include <string.h>
#include <time.h>
#include <bsp.h>
#include "tools.h"
#include "config.h"

#define DEFAULT_TIMEZONE	":CET:CEST:0100:040102-0:110102-0"

const char *MonthNames[] = {"Jan","Feb","Mar","Apr","May","Jun","Jul","Aug","Sep","Oct","Nov","Dec"};
const char *WeekDayNames[] = {"Sun","Mon","Tue","Wed","Thu","Fri","Sat"};

__time32_t __time32(__time32_t *tod)
{
__time32_t t;

	if (!tod) tod = &t;
	*tod = RTC_GetCounter();
	return *tod;
}

char const * __getzone(void)
{
	return DEFAULT_TIMEZONE;
}
