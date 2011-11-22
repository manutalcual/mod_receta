//
// Clase: sql Copyright (c) 2011 Manuel Cano <manutalcual@gmail.com>
// Autor: Manuel Cano Muñoz
// Fecha: Wed Nov 16 20:18:05 2011

// Time-stamp: <2011-11-17 20:51:20 manuel>
//
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
#include "sql.h"

int my_init (sql_t * obj)
{
    MYSQL_RES * res = NULL;
    MYSQL_ROW row;
    int nf = 0; /* num fields */
    int nr = 0; /* num rows */
    int r = 0; /* row index */
    int f = 0; /* field index */

    logp ("Llamada la funcion 'init' %x.", obj);

    mysql_init (&obj->my_sql);

    /* mysql_options (&mysql, MYSQL_READ_DEFAULT_GROUP, "apache2"); */
    if (! mysql_real_connect(&obj->my_sql,
			     "localhost",
			     "md",
			     "c0ntr4sen14",
			     "md",
			     0,
			     NULL,
			     0))
    {
	logp ("Failed to connect to database: Error: %s\n",
	      mysql_error(&obj->my_sql));
	return 0;
    }

    if (mysql_query(&obj->my_sql,
		    "select * from recipes"))
    {
	logp ("Error al realizar la consulta.");
	goto end;
    }

    res = mysql_store_result(&obj->my_sql);

    if (! res) {
	logp ("La consulta no tiene datos.");
	goto end;
    }

    nf = mysql_num_fields(res);

    if (nf <= 0) {
	logp ("Numfields ha devuelto < 0!!!");
	goto end;
    }

    nr = mysql_num_rows(res);

    if (nf <= 0) {
	logp ("Numrows ha devuelto < 0!!!");
	goto end;
    }

    logp ("Comenzamos bucle.");
    for (r = 0; r < nr; ++r) {
	row = mysql_fetch_row(res);
	if (row) {
	    for (f = 0; f < nf; ++f) {
		ap_rprintf (rct_request_req, "%s", row[f]); // just to see
	    }
	    ap_rprintf (rct_request_req, "\n");
	}
    }

end:
    mysql_close (&obj->my_sql);

    return 0;
}
