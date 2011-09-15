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

#include "GetTimeOfDay.h"
#include "Operators.h"

// boost
#include <boost/cstdint.hpp>

typedef boost::int64_t  int64_t;
typedef boost::uint32_t uint32_t;
typedef boost::uint8_t  uint8_t;

// windows
#include <Winsock2.h>
#include <ws2tcpip.h>
#pragma comment(lib, "Ws2_32.lib")

// pthreads
#include <pthread.h>

//"asm/posix_types.h"
typedef unsigned int __kernel_size_t;
typedef int          __kernel_ssize_t;

//"linux/types.h"
typedef __kernel_size_t  size_t;
typedef __kernel_ssize_t ssize_t;

#define __BYTE_ORDER __LITTLE_ENDIAN

#endif // _THRIFT_WINDOWS_CONFIG_H_
