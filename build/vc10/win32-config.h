#pragma once
#pragma warning(disable: 4996) /* The POSIX name for this item is deprecated */ 
#pragma warning(disable: 4250) /* inherits via dominance */ 
#pragma warning(disable: 4244)
#pragma warning(disable: 4267) /* possible loss of data */

#define VERSION "0.8.0-dev"

#define NOMINMAX
#define HAVE_PTHREAD_H

//#define WIN_XP

#ifdef WIN_XP
//Because WSAPoll is not implemented for winxp
#	define WINVER 0x0502
#	define _WIN32_WINNT 0x0502
#	define _WIN32_WINDOWS 0x0502
#	define USE_SELECT_NOT_POLL
#endif //WIN_XP

#include <WinSock2.h>
#include <Windows.h>
#include <io.h>
#include <ws2tcpip.h>

#include <xstddef>
#include <stdint.h>
#include <time.h>
#include <errno.h>

#undef gai_strerror
#define gai_strerror gai_strerrorA

#undef errno
#undef EINTR
#undef EINPROGRESS
#undef ECONNRESET
#undef ENOTCONN
#undef ETIMEDOUT
#undef EWOULDBLOCK
#undef EAGAIN
#undef EPIPE
#define errno ::WSAGetLastError()
#define EINPROGRESS WSAEINPROGRESS
#define EAGAIN WSAEWOULDBLOCK
#define EINTR WSAEINTR
#define ECONNRESET WSAECONNRESET
#define ENOTCONN WSAENOTCONN
#define ETIMEDOUT WSAETIMEDOUT
#define EWOULDBLOCK WSAEWOULDBLOCK
#define EPIPE WSAECONNRESET

#define ctime_r( _clock, _buf ) \
        ( strcpy( (_buf), ctime( (_clock) ) ),  \
          (_buf) )

#define O_NONBLOCK 1

enum {
	F_GETFL,
	F_SETFL,
};

extern int fcntl(int fd, int cmd, int flags);

typedef  int         socklen_t;
typedef  ptrdiff_t   ssize_t;

#define HAVE_STRUCT_TIMESPEC 1
struct timespec {
        long tv_sec;
        long tv_nsec;
};

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

#ifndef USE_SELECT_NOT_POLL
//Answer: Works, but only on Windows 7 and Windows Server 2008 version od Winsocks2 DLL
// see https://issues.apache.org/jira/browse/THRIFT-1123
#   define poll(poll_array, count, timeout) \
        WSAPoll(poll_array, count, timeout)
#endif //USE_SELECT_NOT_POLL

#define usleep(ms) Sleep(ms)

#define SOCKOPT_CAST_T char
#define AF_LOCAL AF_INET
