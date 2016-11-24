//
// Clase: sql Copyright (c) 2011 Manuel Cano <manutalcual@gmail.com>
// Autor: Manuel Cano Muñoz
// Fecha: Wed Nov 16 20:18:05 2011

// Time-stamp: <2012-01-13 20:30:01 manuel>
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
    mysql_init (&obj->my_sql);

    logp ("Trying to connect: user, %s; pwd, %s; db, %s",
	  obj->dbuser,
	  obj->dbpwd,
	  obj->database);

    /* mysql_options (&mysql, MYSQL_READ_DEFAULT_GROUP, "apache2"); */
    if (! mysql_real_connect(&obj->my_sql,
			     "localhost",
			     obj->dbuser,
			     obj->dbpwd,
			     obj->database,
			     0,
			     NULL,
			     0))
    {
	logp ("Failed to connect to database: Error: %s\n",
	      mysql_error(&obj->my_sql));
	return -1;
    }

#if 0
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
    
    ap_rprintf (rct_request_req, "\n\n");

    ap_rprintf (rct_request_req,
		"Path info: %s.\n",
		rct_request_req->path_info);

    FILE * file = popen("ls -lh", "r");

    while (!feof(file)) {
	char ch[256];
	int r = 0;

	r = fread(ch, 1, 254, file);
	ch[r - 1] = '\0';
	logp ("Readed: %d\n", r);
	ap_rprintf (rct_request_req, "%s", ch);
    }

    pclose (file);
#endif
    return 0;
}

int my_query (sql_t * obj, const char * query)
{
    return (mysql_query(&obj->my_sql,
			query) != 0) ? -1 : 0;
}

int my_get_results (sql_t * obj)
{
    obj->res = mysql_store_result(&obj->my_sql);

    if (! obj->res) {
	logp ("La consulta no tiene datos.");
	goto err_get_results;
    }

    obj->num_fields = mysql_num_fields(obj->res);

    if (obj->num_fields <= 0) {
	logp ("Numfields ha devuelto < 0!!!");
	goto err_get_results;
    }

    my_get_fields (obj);

    obj->num_rows = mysql_num_rows(obj->res);

    if (obj->num_rows <= 0) {
	logp ("Numrows ha devuelto < 0!!!");
	goto err_get_results;
    }

    obj->cur_row = 0;

    return obj->num_rows;

err_get_results:
    return -1;
}

/**
 * This function is called by md_get_results, so no need to call it
 * from your main program.
 *
 */
int my_get_fields (sql_t * obj)
{
    obj->fields = mysql_fetch_fields(obj->res);

    return 0;
}

int my_get_next_row (sql_t * obj)
{
    if (! obj->res) {
	logp ("no hay objeto recurso");
	return -1;
    }

    obj->row = mysql_fetch_row(obj->res);
    if (++obj->cur_row > obj->num_rows) {
	mysql_free_result (obj->res);
	obj->res = 0;
	return -1;
    }

    return 0;
}

int my_close (sql_t * obj)
{
    mysql_close (&obj->my_sql);

    memset (obj, 0, sizeof(sql_t));

    return -1;
}
