/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef _THRIFT_WINDOWS_CONFIG_H_
#define _THRIFT_WINDOWS_CONFIG_H_ 1

#if defined(_MSC_VER) && (_MSC_VER > 1200)
#pragma once
#endif // _MSC_VER

#ifndef _WIN32
#error This is a MSVC header only.
#endif

#pragma warning(disable: 4996) /* The POSIX name for this item is deprecated */ 
#pragma warning(disable: 4250) /* inherits via dominance */ 

//#define WIN_XP

#ifdef WIN_XP
//Because WSAPoll is not implemented for winxp
#	define WINVER 0x0502
#	define _WIN32_WINNT 0x0502
#	define _WIN32_WINDOWS 0x0502
#endif //WIN_XP

#define VERSION "0.8.0-dev"

#define NOMINMAX

#include "TargetVersion.h"
#include "GetTimeOfDay.h"
#include "Operators.h"
#include "TWinsockSingleton.h"
#include "Fcntl.h"
#include "SocketPair.h"

// boost
#include <boost/cstdint.hpp>

#define HAVE_PTHREAD_H
#define HAVE_GETTIMEOFDAY

typedef boost::int64_t  int64_t;
typedef boost::uint32_t uint32_t;
typedef boost::uint8_t  uint8_t;

// windows
#include <Winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

// ctime_r, timespec...
#include <pthread.h>

typedef  ptrdiff_t   ssize_t;

// Missing functions.
#define usleep(ms) Sleep(ms)

#if WINVER <= 0x0502
#   define poll(fds, nfds, timeout) \
    select(0, NULL, fds, NULL, timeout)
#else
#   define poll(fds, nfds, timeout) \
    WSAPoll(fds, nfds, timeout)
#endif // WINVER

inline void close(SOCKET socket)
{
    ::closesocket(socket);
}

#endif // _THRIFT_WINDOWS_CONFIG_H_
