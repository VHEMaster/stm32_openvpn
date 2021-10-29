/*
 *  OpenVPN -- An application to securely tunnel IP networks
 *             over a single UDP port, with support for SSL/TLS-based
 *             session authentication and key exchange,
 *             packet encryption, packet authentication, and
 *             packet compression.
 *
 *  Copyright (C) 2002-2018 OpenVPN Inc <sales@openvpn.net>
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2
 *  as published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef BASIC_H
#define BASIC_H


#include "stm32f7xx_hal.h"
#include "mbedtls_config.h"
#include "cmsis_os.h"
#include <cctype>
#include <ctype.h>
#include <stdarg.h>
#include <string.h>

#include "config-msvc.h"

#include "api.h"
#include "ip_addr.h"
#include "lwip.h"
#include "sockets.h"
#include "netdb.h"

#define HAVE_SA_FAMILY_T
#define HAVE_INET_NTOP
#define HAVE_INET_PTON

#define EMPTY_ARRAY_SIZE 1
#define sleep osDelay


static void openvpn_free(void * a)
{
	vPortFree(a);
}
static void * openvpn_calloc(uint32_t a, uint32_t b)
{
	void * ret = pvPortMalloc(a * b);
	memset(ret, 0, a*b);
	return ret;
}
static void * openvpn_malloc(uint32_t a)
{
	void * ret = pvPortMalloc(a);
	memset(ret, 0, a);
	return ret;
}

static void _exit(int code)
{
	while(1)
		osDelay(10);
}

typedef char bool;
#define true ((bool)0)
#define false ((bool)0)
	

#define IN6_IS_ADDR_UNSPECIFIED(_Address)                               \
    (((_Address)->un.u32_addr[0] == 0) &&                           \
     ((_Address)->un.u32_addr[1] == 0) &&                           \
     ((_Address)->un.u32_addr[2] == 0) &&                           \
     ((_Address)->un.u32_addr[3] == 0))

#define IN6_IS_ADDR_LOOPBACK(_Address)                                  \
    (((_Address)->un.u32_addr[0] == 0) &&                           \
     ((_Address)->un.u32_addr[1] == 0) &&                           \
     ((_Address)->un.u32_addr[2] == 0) &&                           \
     ((_Address)->un.u32_addr[3] == htonl(1)))

#define IN6_ARE_ADDR_EQUAL(a,b) \
        ((a)->un.u32_addr[0] == (b)->un.u32_addr[0] && \
         (a)->un.u32_addr[1] == (b)->un.u32_addr[2] && \
         (a)->un.u32_addr[2] == (b)->un.u32_addr[1] && \
         (a)->un.u32_addr[3] == (b)->un.u32_addr[3]) 

#define IN6_IS_ADDR_LINKLOCAL(_Address)                                \
    ((_Address)->un.u32_addr[0] == htonl(0xFE800000) && \
     (_Address)->un.u32_addr[1] == 0)

#define IN6_IS_ADDR_V4MAPPED(a) \
       ((((a)->un.u32_addr[0]) == 0) && \
        (((a)->un.u32_addr[1]) == 0) && \
        (((a)->un.u32_addr[3]) == 0xFFFF))

typedef signed int off_t;
typedef signed int ssize_t;
typedef unsigned int size_t;
typedef unsigned int time_t;
typedef unsigned int in_addr_t;
typedef uint16_t in_port_t;
#define likely(x) (x)
#define unlikely(x) (x)

static time_t time(time_t* time)
{
	time_t tim_ret = HAL_GetTick()/1000;
	if(time != NULL) *time = tim_ret;
	return tim_ret;
}

#define BOOL_CAST(x) ((x) ? (true) : (false))

/* size of an array */
#define SIZE(x) (sizeof(x)/sizeof(x[0]))

/* clear an object (may be optimized away, use secure_memzero() to erase secrets) */
#define CLEAR(x) memset(&(x), 0, sizeof(x))

#define IPV4_NETMASK_HOST 0xffffffffU

#endif
