#pragma once
#pragma warning(disable: 4996) /* The POSIX name for this item is deprecated */ 
#pragma warning(disable: 4250) /* inherits via dominance */ 

#define NOMINMAX

#include <xstddef>

extern char* ctime_r(const time_t*, char*);
extern int fcntl(int, int, int);
enum {
	F_GETFL,
	F_SETFL,
	O_NONBLOCK,
};

typedef  int         socklen_t;
typedef  ptrdiff_t   ssize_t;

#define HAVE_STRUCT_TIMESPEC 1
struct timespec {
        long tv_sec;
        long tv_nsec;
};

#include <time.h>

#define HAVE_GETTIMEOFDAY
 
#if defined(_MSC_VER) || defined(_MSC_EXTENSIONS)
#   define DELTA_EPOCH_IN_MICROSECS  11644473600000000Ui64
#else
#   define DELTA_EPOCH_IN_MICROSECS  11644473600000000ULL
#endif 
 
struct timezone
{
    int  tz_minuteswest; /* minutes W of Greenwich */
    int  tz_dsttime;     /* type of dst correction */
};

int gettimeofday(struct timeval * tv, struct timezone * tz);

#include <stdint.h>

//Answer: Works, but only on Windows 7 and Windows Server 2008 version od Winsocks2 DLL
// see https://issues.apache.org/jira/browse/THRIFT-1123
#   define poll(poll_array, count, timeout) \
        WSAPoll(poll_array, count, timeout)

#define usleep(ms) Sleep(ms)
