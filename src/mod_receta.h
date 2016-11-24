// -*- mode: c++; indent-tabs-mode: t; -*-
//
// Program: mod_receta Copyright (c) 2011 Manuel Cano <manutalcual@gmail.com>
// Author: manutalcual@gmail.com
// Date: Wed Nov  9 19:42:43 2011
// Time-stamp: <2012-01-15 20:09:07 manuel>
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
//
//
// Includes
//
#include <sys/types.h>
#include <mysql/mysql.h>

#include <httpd.h>
#include <http_log.h>
#include <http_protocol.h>
#include <http_config.h>
#include <apr_strings.h>

#undef PACKAGE_BUGREPORT
#undef PACKAGE_NAME
#undef PACKAGE_STRING
#undef PACKAGE_TARNAME
#undef PACKAGE_VERSION

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "logp.h"
#include "sql.h"

#if !defined(DEFAULT_MODRECETA_FILES_PATH) && !defined(FILES_PATH)
#error Needed values for mod_receta configuration are not defined.
#endif

#ifndef DEFAULT_MODRECETA_FILES_PATH
#define DEFAULT_MODRECETA_FILES_PATH FILES_PATH
#endif

#ifndef DEFAULT_MODRECETA_DATABASE
#define DEFAULT_MODRECETA_DATABASE "md"
#endif

#ifndef DEFAULT_MODRECETA_DBUSER
#define DEFAULT_MODRECETA_DBUSER "md"
#endif

#ifndef DEFAULT_MODRECETA_DBPWD
#define DEFAULT_MODRECETA_DBPWD "c0ntr4sen14"
#endif

#ifndef DEFAULT_MODRECETA_DATA_PATH
#define DEFAULT_MODRECETA_DATA_PATH "/var/lib/mod_receta"
#endif

typedef struct {
	char * name;
	char * value;
} pair_t;

enum {
	error = -1,
	false = 0,
	login,
	err,
	opts
};

int md_value_exists (const char * name, pair_t ** pairs);
int md_make_value_index (int index, pair_t ** pairs);
int md_make_get_params (request_rec * r, pair_t *** pairs);
int md_login (pair_t ** vars);
int md_wrong_login (request_rec * r);
int md_login_ok (request_rec * r);
const char * md_get_getvalue (const char * name, pair_t ** vars);

int md_get_error (request_rec * r, pair_t ** pairs);
void md_set_sql_data (sql_t * sql, request_rec * r);

int md_create_type_response (request_rec * r, sql_t * sql,
							 const char * type,
							 pair_t ** pairs);
int md_parse_data_file (request_rec * r,
						sql_t * sql,
						apr_file_t * f,
						apr_off_t fsize,
						pair_t ** pairs);
int md_get_sql_command (request_rec * r,
						const char * buf,
						char ** var,
						pair_t ** pairs);
int md_get_options (request_rec * r,
					const char * buf,
					char ** var,
					pair_t ** pairs);
int md_get_data (request_rec * r,
				 const char * buf,
				 char ** var,
				 pair_t ** pairs);


int md_stat_file (request_rec * r, const char * fname, apr_off_t * size);
int md_line_return_to_space (char * str);

