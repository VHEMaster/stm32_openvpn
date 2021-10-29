/*
 *  OpenVPN -- An application to securely tunnel IP networks
 *             over a single TCP/UDP port, with support for SSL/TLS-based
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#elif defined(_MSC_VER)
#include "config-msvc.h"
#endif

#include "syshead.h"

#include "status.h"
#include "perf.h"
#include "misc.h"
#include "fdmisc.h"

#include "memdbg.h"

/*
 * printf-style interface for outputting status info
 */

static const char *
print_status_mode(unsigned int flags)
{
    switch (flags)
    {
        case STATUS_OUTPUT_WRITE:
            return "WRITE";

        case STATUS_OUTPUT_READ:
            return "READ";

        case STATUS_OUTPUT_READ|STATUS_OUTPUT_WRITE:
            return "READ/WRITE";

        default:
            return "UNDEF";
    }
}

struct status_output *
status_open(const char *filename,
            const int refresh_freq,
            const int msglevel,
            const struct virtual_output *vout,
            const unsigned int flags)
{
    struct status_output *so = NULL;
    if (filename || msglevel >= 0 || vout)
    {
				FRESULT fres;
        ALLOC_OBJ_CLEAR(so, struct status_output);
        so->flags = flags;
        so->msglevel = msglevel;
        so->vout = vout;
        buf_reset(&so->read_buf);
        event_timeout_clear(&so->et);
        if (filename)
        {
            switch (so->flags)
            {
                case STATUS_OUTPUT_WRITE:
                    fres = f_open(&so->fd, filename, FA_CREATE_ALWAYS | FA_WRITE);
                    break;

                case STATUS_OUTPUT_READ:
                    fres = f_open(&so->fd, filename, FA_READ);
                    break;

                case STATUS_OUTPUT_READ|STATUS_OUTPUT_WRITE:
                    fres = f_open(&so->fd, filename, FA_CREATE_NEW | FA_WRITE | FA_READ);
                    break;

                default:
                    ASSERT(0);
            }
            if (fres == FR_OK)
            {
                so->filename = string_alloc(filename, NULL);
                //set_cloexec(&so->fd);

                /* allocate read buffer */
                if (so->flags & STATUS_OUTPUT_READ)
                {
                    so->read_buf = alloc_buf(512);
                }
            }
            else
            {
                msg(M_WARN, "Note: cannot open %s for %s", filename, print_status_mode(so->flags));
                so->errors = true;
            }
        }
        else
        {
            so->flags = STATUS_OUTPUT_WRITE;
        }

        if ((so->flags & STATUS_OUTPUT_WRITE) && refresh_freq > 0)
        {
            event_timeout_init(&so->et, refresh_freq, 0);
        }
    }
    return so;
}

bool
status_trigger(struct status_output *so)
{
    if (so)
    {
        struct timeval null;
        CLEAR(null);
        return event_timeout_trigger(&so->et, &null, ETT_DEFAULT);
    }
    else
    {
        return false;
    }
}

bool
status_trigger_tv(struct status_output *so, struct timeval *tv)
{
    if (so)
    {
        return event_timeout_trigger(&so->et, tv, ETT_DEFAULT);
    }
    else
    {
        return false;
    }
}

void
status_reset(struct status_output *so)
{
    if (so)
    {
        f_lseek(&so->fd, (off_t)0);
    }
}

void
status_flush(struct status_output *so)
{
    if (so && (so->flags & STATUS_OUTPUT_WRITE))
    {
				f_sync(&so->fd);

        /* clear read buffer */
        if (buf_defined(&so->read_buf))
        {
            ASSERT(buf_init(&so->read_buf, 0));
        }
    }
}

/* return false if error occurred */
bool
status_close(struct status_output *so)
{
    bool ret = true;
    if (so)
    {
        if (so->errors)
        {
            ret = false;
        }
            if (f_close(&so->fd) != FR_OK)
            {
                ret = false;
            }
        
        if (so->filename)
        {
            openvpn_free(so->filename);
        }
        if (buf_defined(&so->read_buf))
        {
            free_buf(&so->read_buf);
        }
        openvpn_free(so);
    }
    else
    {
        ret = false;
    }
    return ret;
}

#define STATUS_PRINTF_MAXLEN 512

void
status_printf(struct status_output *so, const char *format, ...)
{
    if (so && (so->flags & STATUS_OUTPUT_WRITE))
    {
        char buf[STATUS_PRINTF_MAXLEN+2]; /* leave extra bytes for CR, LF */
        va_list arglist;
        int stat;

        va_start(arglist, format);
        stat = vsnprintf(buf, STATUS_PRINTF_MAXLEN, format, arglist);
        va_end(arglist);
        buf[STATUS_PRINTF_MAXLEN - 1] = 0;

        if (stat < 0 || stat >= STATUS_PRINTF_MAXLEN)
        {
            so->errors = true;
        }

        if (so->msglevel >= 0 && !so->errors)
        {
            msg(so->msglevel, "%s", buf);
        }

        if (!so->errors)
        {
            uint32_t len;
						uint32_t chk;
            strcat(buf, "\n");
            len = strlen(buf);
            if (len > 0)
            {
                if (f_write(&so->fd, buf, len, &chk) != FR_OK || chk != len)
                {
                    so->errors = true;
                }
            }
        }

        if (so->vout && !so->errors)
        {
            chomp(buf);
            (*so->vout->func)(so->vout->arg, so->vout->flags_default, buf);
        }
    }
}

bool
status_read(struct status_output *so, struct buffer *buf)
{
    bool ret = false;

    if (so && (so->flags & STATUS_OUTPUT_READ))
    {
        ASSERT(buf_defined(&so->read_buf));
        ASSERT(buf_defined(buf));
        while (true)
        {
            const int c = buf_read_u8(&so->read_buf);

            /* read more of file into buffer */
            if (c == -1)
            {
                uint32_t len;

                ASSERT(buf_init(&so->read_buf, 0));
                if (f_read(&so->fd, BPTR(&so->read_buf), BCAP(&so->read_buf), &len) != FR_OK || len <= 0)
                {
                    break;
                }

                ASSERT(buf_inc_len(&so->read_buf, len));
                continue;
            }

            ret = true;

            if (c == '\r')
            {
                continue;
            }

            if (c == '\n')
            {
                break;
            }

            buf_write_u8(buf, c);
        }

        buf_null_terminate(buf);
    }

    return ret;
}
