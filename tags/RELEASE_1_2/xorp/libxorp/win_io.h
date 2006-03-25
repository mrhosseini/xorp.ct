/* -*- c-basic-offset: 4; tab-width: 8; indent-tabs-mode: t -*- */

/*
 * Copyright (c) 2001-2005 International Computer Science Institute
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software")
 * to deal in the Software without restriction, subject to the conditions
 * listed in the XORP LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the XORP LICENSE file; the license in that file is
 * legally binding.
 */

/*
 * $XORP: xorp/libxorp/win_io.h,v 1.3 2005/12/21 09:42:58 bms Exp $
 */

#ifndef __LIBXORP_WIN_IO_H__
#define __LIBXORP_WIN_IO_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HOST_OS_WINDOWS

int win_strerror_r(DWORD errnum, char *strerrbuf, size_t buflen);
char *win_strerror(DWORD errnum);

#define WINIO_ERROR_HASINPUT (-2)	/* Data is ready to be read */

ssize_t	win_con_read(HANDLE h, void *buf, size_t bufsize);
ssize_t	win_pipe_read(HANDLE h, void *buf, size_t bufsize);

#endif /* HOST_OS_WINDOWS */

#ifdef __cplusplus
}
#endif

#endif /* __LIBXORP_WIN_IO_H__ */