//
// Clase: logp Copyright (c) 2011 Manuel Cano <manutalcual@gmail.com>
// Autor: Manuel Cano Muñoz
// Fecha: Thu Nov 17 19:28:00 2011

// Time-stamp: <2011-12-10 12:59:45 manuel>
//
//   This program is free software; you can redistribute it and/or modify
//   it under the terms of the GNU General Public License as published by
//   the Free Software Foundation; either version 2 of the License, or
//   (at your option) any later version.
//
//   This program is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//   GNU General Public License for more details.
//
//   You should have received a copy of the GNU General Public License
//   along with this program; if not, write to the Free Software
//   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
//   02110-1301	USA

#ifndef logp_hh
#define logp_hh
//
// Includes
//
#include <httpd.h>
#include <http_log.h>
#include <http_protocol.h>
#include <http_config.h>

#define logp(x, args...) ap_log_rerror (APLOG_MARK, APLOG_DEBUG, 0, rct_request_req, x, ## args)
#define logf() ap_log_rerror (APLOG_MARK, APLOG_DEBUG, 0, rct_request_req, "%s", __PRETTY_FUNCTION__)

extern request_rec * rct_request_req;

#endif // logp_hh

