/* -*- mode: c++; indent-tabs-mode: t; -*-
//
// Program: mod_receta Copyright (c) 2011 Manuel Cano <manutalcual@gmail.com>
// Author: manutalcual@gmail.com
// Date: Wed Nov  9 19:42:43 2011
// Time-stamp: <2011-11-09 20:00:06 manuel>
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
*/
#include "main.h"


static int mod_tut1_method_handler (request_rec *r)
{
	// Send a message to stderr (apache redirects this to the error log)
	fprintf(stderr,"apache2_mod_receta: A request was made.\n");

	// We need to flush the stream so that the message appears right away.
	// Performing an fflush() in a production system is not good for
	// performance - don't do this for real.
	fflush(stderr);

	// Return DECLINED so that the Apache core will keep looking for
	// other modules to handle this request.  This effectively makes
	// this module completely transparent.
	return DECLINED;
}

static void mod_receta_register_hooks (apr_pool_t *p)
{
	// I think this is the call to make to register a handler for method calls (GET PUT et. al.).
	// We will ask to be last so that the comment has a higher tendency to
	// go at the end.
	ap_hook_handler(mod_tut1_method_handler, NULL, NULL, APR_HOOK_LAST);
}


module AP_MODULE_DECLARE_DATA receta_module =
{
	// Only one callback function is provided.  Real
	// modules will need to declare callback functions for
	// server/directory configuration, configuration merging
	// and other tasks.
	STANDARD20_MODULE_STUFF,
	NULL,
	NULL,
	NULL,
	NULL,
	NULL,
	mod_receta_register_hooks,			/* callback for registering hooks */
};
