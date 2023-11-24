#include <stdio.h>
#include <time.h>
#include <stdlib.h>


// /etc/localtime - system-wide time (linux)
// /etc/TIMEZONE - ---//--- (solaris)

// /usr/share/zoneinfo (linux)
// /usr/share/lib/zoneinfo (solaris)

// ctime(), localtime(), mktime() -> tzset() ->
// check if TZ was set (if not - /etc/TIMEZONE) (if defined as ""/invalid - UTC) ->
// tzname, int daylight (<0 - ??, 0 - ??, >0 - ??), long timezone - difference between UTC & local in secs


extern char *tzname[]; //f.e. PDT-PST, CET-CEST, CDT-CST
int main()
{
	// if not defined, uses localtime, if defined as empty string, UTC is taken as time zone
	putenv("TZ=America/Los_Angeles"); // global environ (+- as *argv[])

	time_t now;
	struct tm *sp;
	time( &now ); // kernel -> time_t (since epoch)

	printf("%s", ctime( &now ) );

	sp = localtime(&now); //time_t -> struct tm (look at time zone)
	printf("%d/%d/%02d %d:%02d %s\n",
		 sp->tm_mon + 1, sp->tm_mday,
		 sp->tm_year + 1900, sp->tm_hour,
		 sp->tm_min, tzname[sp->tm_isdst]);
	return 0;
}
