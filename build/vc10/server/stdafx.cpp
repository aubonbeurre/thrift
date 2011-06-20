// stdafx.cpp : source file that includes just the standard includes
// server.pch will be the pre-compiled header
// stdafx.obj will contain the pre-compiled type information

#include "stdafx.h"

// TODO: reference any additional headers you need in STDAFX.H
// and not in this file

#include "config.h"

#include <WinSock2.h>

int fcntl(int fd, int cmd, int flags) {
	if(cmd != F_GETFL && cmd != F_SETFL)
		throw -99;
	if(flags != O_NONBLOCK && flags != 0)
		throw -99;
	if(cmd == F_GETFL)
		return 0;
	int res;
	if(flags)
		res = ioctlsocket(fd, FIONBIO, reinterpret_cast<u_long *>(&(flags = 1)));
	else
		res = ioctlsocket(fd, FIONBIO, reinterpret_cast<u_long *>(&(flags = 0)));
	return res;
}

int gettimeofday(struct timeval * tv, struct timezone * tz)
{
    FILETIME         ft;
    unsigned __int64 tmpres(0);
    static int       tzflag;

    if (NULL != tv)
    {
        GetSystemTimeAsFileTime(&ft);

        tmpres |= ft.dwHighDateTime;
        tmpres <<= 32;
        tmpres |= ft.dwLowDateTime;

        /*converting file time to unix epoch*/
        tmpres -= DELTA_EPOCH_IN_MICROSECS; 
        tmpres /= 10;  /*convert into microseconds*/
        tv->tv_sec = (long)(tmpres / 1000000UL);
        tv->tv_usec = (long)(tmpres % 1000000UL);
    }

    if (NULL != tz)
    {
        if (!tzflag)
        {
            _tzset();
            tzflag++;
        }

        long time_zone(0);
        errno_t err(_get_timezone(&time_zone));
        if (err == NO_ERROR)
        {
            tz->tz_minuteswest = time_zone / 60;
        }
        else
        {
            return -1;
        }

        int day_light(0);
        err = (_get_daylight(&day_light));
        if (err == NO_ERROR)
        {
            tz->tz_dsttime = day_light;
            return 0;
        }
        else
        {
            return -1;
        }
    }

    return -1;
}