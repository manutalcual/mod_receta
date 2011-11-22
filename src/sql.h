//
// Clase: sql Copyright (c) 2011 Manuel Cano <manutalcual@gmail.com>
// Autor: Manuel Cano Muñoz
// Fecha: Wed Nov 16 20:18:05 2011

// Time-stamp: <2011-11-17 20:11:43 manuel>
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

#ifndef sql_hh
#define sql_hh
//
// Includes
//
#include <httpd.h>
#include <http_log.h>
#include <http_protocol.h>
#include <http_config.h>

#include <mysql/mysql.h>

#include "logp.h"

struct _tag_sql_t;

typedef int (*intfunc_t) (struct _tag_sql_t *);

typedef struct _tag_sql_t {
    intfunc_t init;
    MYSQL my_sql;
} sql_t;

extern sql_t sql;

int my_init (sql_t *);

#endif // sql_hh

