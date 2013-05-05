/*
    This file is part of libmicrospdy
    Copyright (C) 2012 Andrey Uzunov

    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

/**
 * @file tls.h
 * @brief  TLS handling. openssl with NPN is used, but as long as the
 * 			functions conform to this interface file, other libraries
 * 			can be used.
 * @author Andrey Uzunov
 */

#ifndef TLS_H
#define TLS_H

#include "platform.h"
#include <openssl/err.h>
#include <openssl/ssl.h>
#include <openssl/rand.h>

/* macros used in other files instead of types.
 * useful in case of changing openssl to something else */
#define SPDYF_TLS_SESSION_CONTEXT SSL
#define SPDYF_TLS_DAEMON_CONTEXT SSL_CTX


/**
 * Used for return code when reading and writing to the TLS socket.
 */
enum SPDY_TLS_ERROR
{
	/**
	 * The connection was closed by the other party.
	 */
	SPDY_TLS_ERROR_CLOSED = 0,
	
	/**
	 * Any kind of error ocurred. The session has to be closed.
	 */
	SPDY_TLS_ERROR_ERROR = -2,
	
	/**
	 * The function had to return without processing any data. The whole
	 * cycle of events has to be called again (SPDY_run) as something
	 * either has to be written or read or the the syscall was
	 * interrupted by a signal.
	 */
	SPDY_TLS_ERROR_AGAIN = -3,
};


/**
 * Global initializing of openssl. Must be called only once in the program.
 *
 */
void
SPDYF_tls_global_init();


/**
 * Global deinitializing of openssl for the whole program. Should be called
 * at the end of the program.
 *
 */
void
SPDYF_tls_global_deinit();


/**
 * Initializing of openssl for a specific daemon.
 * Must be called when the daemon starts.
 *
 * @param daemon SPDY_Daemon for which openssl will be used. Daemon's
 * 				certificate and key file are used.
 * @return SPDY_YES on success or SPDY_NO on error
 */
int
SPDYF_tls_init(struct SPDY_Daemon *daemon);


/**
 * Deinitializing openssl for a daemon. Should be called
 * when the deamon is stopped.
 *
 * @param daemon SPDY_Daemon which is being stopped
 */
void
SPDYF_tls_deinit(struct SPDY_Daemon *daemon);


/**
 * Initializing openssl for a specific connection. Must be called
 * after the connection has been accepted.
 *
 * @param session SPDY_Session whose socket will be used by openssl
 * @return SPDY_NO if some openssl funcs fail. SPDY_YES otherwise
 */
int
SPDYF_tls_new_session(struct SPDY_Session *session);


/**
 * Deinitializing openssl for a specific connection. Should be called
 * closing session's socket.
 *
 * @param session SPDY_Session whose socket is used by openssl
 */
void
SPDYF_tls_close_session(struct SPDY_Session *session);


/**
 * Reading from a TLS socket. Reads available data and put it to the
 * buffer.
 *
 * @param session for which data is received
 * @param buffer where data from the socket will be written to
 * @param size of the buffer
 * @return number of bytes (at most size) read from the TLS connection
 *         0 if the other party has closed the connection
 *         SPDY_TLS_ERROR code on error
 */
int
SPDYF_tls_recv(struct SPDY_Session *session,
				void * buffer,
				size_t size);


/**
 * Writing to a TLS socket. Writes the data given into the buffer to the
 * TLS socket.
 *
 * @param session whose context is used
 * @param buffer from where data will be written to the socket
 * @param size number of bytes to be taken from the buffer
 * @return number of bytes (at most size) from the buffer that has been
 * 			written to the TLS connection
 *         0 if the other party has closed the connection
 *         SPDY_TLS_ERROR code on error
 */
int
SPDYF_tls_send(struct SPDY_Session *session,
				const void * buffer,
				size_t size);


/**
 * Checks if there is data staying in the buffers of the underlying
 * system that waits to be read.
 *
 * @param session which is checked
 * @return SPDY_YES if data is pending or SPDY_NO otherwise
 */
int
SPDYF_tls_is_pending(struct SPDY_Session *session);

#endif
