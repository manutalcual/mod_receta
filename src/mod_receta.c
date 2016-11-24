// -*- mode: c++; indent-tabs-mode: t; -*-
//
// Program: mod_receta Copyright (c) 2011 Manuel Cano <manutalcual@gmail.com>
// Author: manutalcual@gmail.com
// Date: Wed Nov  9 19:42:43 2011
// Time-stamp: <2012-01-15 20:08:38 manuel>
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
#include "mod_receta.h"

request_rec * rct_request_req = NULL;

module AP_MODULE_DECLARE_DATA receta_module;

typedef struct {
	const char * files_path;
	const char * database;
	const char * dbuser;
	const char * dbpwd;
	const char * data_path;
} modreceta_config;

sql_t sql;

static int mod_receta_method_handler (request_rec * r);
static int x_handler (request_rec *r);

void md_set_sql_data (sql_t * sql, request_rec * r)
{
	modreceta_config * s_cfg = NULL;

	s_cfg = ap_get_module_config(r->server->module_config,
								 &receta_module);
	sql->database = s_cfg->database;
	sql->dbuser = s_cfg->dbuser;
	sql->dbpwd = s_cfg->dbpwd;	
}

static void mod_receta_register_hooks (apr_pool_t * p)
{
	// I think this is the call to make to register a handler for
	// method calls (GET PUT et. al.).
	// We will ask to be last so that the comment has a higher tendency to
	// go at the end.
    ap_hook_handler (x_handler, NULL, NULL, APR_HOOK_MIDDLE);
	ap_hook_handler (mod_receta_method_handler, NULL, NULL, APR_HOOK_LAST);
}

/*
 * This function is from an example and we don't need it.
 *
 */
#if 0
static const char * set_modreceta_string (cmd_parms * parms,
										  void * mconfig,
										  const char * arg)
{
	// get the module configuration (this is the structure created by
	// create_modtut2_config())
	modreceta_config * s_cfg = ap_get_module_config(parms->server->module_config,
													&receta_module);

	// make a duplicate of the argument's value using the command parameters pool.
	/* s_cfg->string = (char *) arg; */

	// success
	return NULL;
}
#endif

static const char * set_modreceta_data_path (cmd_parms * parms,
											 void * mconfig,
											 const char * arg)
{
	/* server_rec * s = parms->server; */
	modreceta_config * s_cfg = ap_get_module_config(parms->server->module_config,
													&receta_module);

	s_cfg->data_path = arg;
	return NULL;
}

static const char * set_modreceta_files_path (cmd_parms * parms,
											  void * mconfig,
											  const char * arg)
{
	/* server_rec * s = parms->server; */
	modreceta_config * s_cfg = ap_get_module_config(parms->server->module_config,
													&receta_module);

	s_cfg->files_path = arg;
	return NULL;
}

static const char * set_modreceta_database (cmd_parms * parms,
											void * mconfig,
											const char * arg)
{
	server_rec * s = parms->server;
	modreceta_config * s_cfg = ap_get_module_config(s->module_config,
													&receta_module);

	s_cfg->database = arg;
	sql.database = arg;

	return NULL;
}

static const char * set_modreceta_dbuser (cmd_parms * parms,
											  void * mconfig,
											  const char * arg)
{
	server_rec * s = parms->server;
	modreceta_config * s_cfg = ap_get_module_config(s->module_config,
													&receta_module);

	s_cfg->dbuser = arg;
	sql.dbuser = arg;

	return NULL;
}

static const char * set_modreceta_dbpwd (cmd_parms * parms,
											  void * mconfig,
											  const char * arg)
{
	server_rec * s = parms->server;
	modreceta_config * s_cfg = ap_get_module_config(s->module_config,
													&receta_module);

	s_cfg->dbpwd = arg;
	sql.dbpwd = arg;

	return NULL;
}

static void * create_modreceta_config (apr_pool_t * p, server_rec * s)
{
	modreceta_config * newcfg;

	// allocate space for the configuration structure from the provided pool p.
	newcfg = (modreceta_config *) apr_pcalloc(p, sizeof(modreceta_config));

	// set the default value for the error string.
	newcfg->files_path = DEFAULT_MODRECETA_FILES_PATH;
	newcfg->database = DEFAULT_MODRECETA_DATABASE;
	newcfg->dbuser = DEFAULT_MODRECETA_DBUSER;
	newcfg->dbpwd = DEFAULT_MODRECETA_DBPWD;
	newcfg->data_path = DEFAULT_MODRECETA_DATA_PATH;

	// return the new server configuration structure.
	return (void *) newcfg;
}

static const command_rec mod_receta_cmds[] =
{
	AP_INIT_TAKE1(
		"FilesPath",
		set_modreceta_files_path,
		NULL,
		RSRC_CONF,
		"FilesPath <string> -- the path where to look for files."
	),
	AP_INIT_TAKE1(
		"Database",
		set_modreceta_database,
		NULL,
		RSRC_CONF,
		"Database <string> -- the database name."
	),
	AP_INIT_TAKE1(
		"DatabaseUser",
		set_modreceta_dbuser,
		NULL,
		RSRC_CONF,
		"DatabaseUser <string> -- user name to be used for the "
		"database connection."
	),
	AP_INIT_TAKE1(
		"DatabasePwd",
		set_modreceta_dbpwd,
		NULL,
		RSRC_CONF,
		"DatabasePwd <string> -- password to be used for the "
		"database connection."
	),
	AP_INIT_TAKE1(
		"DataPath",
		set_modreceta_data_path,
		NULL,
		RSRC_CONF,
		"DataPath <string> -- Path where data files are stored."
	),
	{NULL}
};

static int x_handler (request_rec * r)
{
	apr_bucket_brigade * bb = NULL;
	int seen_eos = 0;
	int child_stopped_reading = 0;
	apr_status_t rv;
	modreceta_config * s_cfg = NULL;
	pair_t ** pairs = 0; /* unfortunate name, GET params (name, value pairs) */

    if (strcmp(r->handler, "receta-handler")) {
        return DECLINED;
    }

    /*
     * We're about to start sending content, so we need to force the HTTP
     * headers to be sent at this point.  Otherwise, no headers will be sent
     * at all.  We can set any we like first, of course.  **NOTE** Here's
     * where you set the "Content-type" header, and you do so by putting it in
     * r->content_type, *not* r->headers_out("Content-type").  If you don't
     * set it, it will be filled in with the server's default type (typically
     * "text/plain").  You *must* also ensure that r->content_type is lower
     * case.
     *
     * We also need to start a timer so the server can know if the connexion
     * is broken.
     */
    ap_set_content_type(r, "text/html");

    /*
     * If we're only supposed to send header information (HEAD request), we're
     * already there.
     */
    if (r->header_only) {
        return OK;
    }

	s_cfg = ap_get_module_config(r->server->module_config,
								 &receta_module);

	rct_request_req = r;

	md_set_sql_data (&sql, r);

	bb = apr_brigade_create(r->pool, r->connection->bucket_alloc);

	logp ("== Begin =============================================");

	logp ("Request-method:  %s", r->method);
	logp ("URI:             %s", r->uri);
	logp ("                 %s", r->unparsed_uri);
	logp ("Data files path: %s", s_cfg->files_path);
	logp ("Database:        %s", s_cfg->database);
	logp ("Db User:         %s", s_cfg->dbuser);
	logp ("Db Pwd:          %s", s_cfg->dbpwd);
	logp ("Data path:       %s", s_cfg->data_path);

	if (r->args) {
		/* GET */
		logp ("There are GET parameters");

		if (md_make_get_params(r, &pairs) == error) {
			logp ("Can't create GET params");
		}
	} else {
		/* POST */
		logp ("----------------------------------------------------");

		do {
			apr_bucket * bucket = NULL;
   
			logp ("Just entered in the loop.");

			rv = ap_get_brigade(r->input_filters,
								bb,
								AP_MODE_READBYTES,
								APR_BLOCK_READ,
								HUGE_STRING_LEN);

			if (rv != APR_SUCCESS) {
				logp ("Error setting 'brigade'.");
				return HTTP_INTERNAL_SERVER_ERROR;
			}

			for (bucket = APR_BRIGADE_FIRST(bb);
				 bucket != APR_BRIGADE_SENTINEL(bb);
				 bucket = APR_BUCKET_NEXT(bucket))
			{
				apr_size_t len;
				const char *data;

				logp ("Within the 'for'.");

				if (APR_BUCKET_IS_EOS(bucket)) {
					logp ("Found EOS bucket.");
					seen_eos = 1;
					break;
				}
             
				if (APR_BUCKET_IS_FLUSH(bucket)) {
					logp ("Flush, continue.");
					continue;
				}
  
				if (child_stopped_reading) {
					logp ("Stop reading, continue.");
					continue;
				}

				logp ("Reading bucket");
				rv = apr_bucket_read(bucket, &data, &len, APR_BLOCK_READ);

				if (rv != APR_SUCCESS) {
					logp ("Hop! Read did not success.");
					child_stopped_reading = 1;
				} else {
					logp ("Yup. Something readed.");
				}

				if (len) {
					/* just to see something */
					logp ("Datas: %s", data);
				} else {
					logp ("Hmm. This doesn't works.");
				}
			}

			logp ("Exited from 'for'");

			apr_brigade_cleanup (bb);

		} while (! seen_eos);
	}

	logp ("Verify 'type' exists.");
	int vp = md_value_exists("type", pairs);

	if (vp != -1) {
		switch (md_make_value_index(vp, pairs)) {
		case login:
			logp ("Switch 'login'");
			if (md_login(pairs) == -1)
				md_wrong_login (r);
			else
				md_login_ok (r);
			break;
		case err:
			logp ("Switch 'err'.");
			md_get_error (r, pairs);
			break;
		case opts:
			logp ("Switch 'opts'");
			md_create_type_response (r, &sql, pairs[vp]->value, pairs);
			break;
		default:
			logp ("Switch 'default'");
			ap_rprintf (r, "ERR_UNKNOWN_TYPE_OPTION");
			break;
		}
	}
	
	logp ("== Finished ========================================");
	
	/*
	 * We're all done, so cancel the timeout we set.  Since this is probably
     * the end of the request we *could* assume this would be done during
     * post-processing - but it's possible that another handler might be
     * called and inherit our outstanding timer.  Not good; to each its own.
     */

    /*
     * We did what we wanted to do, so tell the rest of the server we
     * succeeded.
     */
    return OK;
}

/**
 * Wrong login.
 *
 */
int md_wrong_login (request_rec * r)
{
	logf ();
	ap_rprintf (r, "ERR_LOGINFAILED");
	return 0;
}

/**
 * Login ok.
 *
 */
int md_login_ok (request_rec * r)
{
	logf ();
	ap_rprintf (r, "SESSION_ID");
	return 0;
}

/**
 *
 *
 *
 */
int md_login (pair_t ** vars)
{
	logf ();
	//sql_t sql;
	char * query = NULL;
	const char * query_fmt = "select * from users where "
		"user_name = '%s' and password = '%s' and disabled != 1";
	const char * user = md_get_getvalue("u", vars);
	const char * pwd = md_get_getvalue("p", vars);

	if (!user || !pwd) {
		return -1;
	}

	query = (char *)apr_palloc(rct_request_req->pool,
							   strlen(user)
							   + strlen(pwd)
							   + strlen(query_fmt)
							   + 1 /* null */);

	if (! query) {
		return -1;
	}

	sprintf (query, query_fmt, user, pwd);

	my_init (&sql);
	if (my_query (&sql, query) == -1)
		return -1;

	if (my_get_results(&sql) == -1)
		return -1;

	if (my_get_next_row(&sql) == -1)
		return -1;

	logp ("Login: %s, %s", sql.row[0], sql.row[1]);

	return 0;
}

int md_get_error (request_rec * r, pair_t ** vars)
{
	logf ();
	int row_count = 0;
	char * query = NULL;
	const char * query_fmt = "select * from errors where "
		"error_code = '%s'";
	const char * error_code = md_get_getvalue("id", vars);

	logp ("Error code: %s.", error_code);

	query = (char *)apr_palloc(rct_request_req->pool,
							   + strlen(query_fmt)
							   + 1 /* null */);

	if (! query) {
		return -1;
	}

	sprintf (query, query_fmt, error_code);

	logp ("1 ---");

	my_init (&sql);
	if (my_query (&sql, query) == -1)
		goto err_get_error;

	logp ("2 ---");

	if ((row_count = my_get_results(&sql)) == -1)
		goto err_get_error;

	logp ("3 --- Rows: %d", row_count);

	if (my_get_next_row(&sql) == -1) {
		logp ("No se pueden extraer los datos.");
		goto err_get_error;
	}

	logp ("Error: %s, %s", sql.row[1], sql.row[2]);
	ap_rprintf (r, "<strong>%s</strong>", sql.row[2]);

	my_close (&sql);
	return 0;

err_get_error:
	my_close (&sql);
	return -1;
}

/**
 * Create response.
 *
 */
int md_create_type_response (request_rec * r,
							 sql_t * sql,
							 const char * type,
							 pair_t ** pairs)
{
	logf ();
	modreceta_config * s_cfg = NULL;
	int row = 0;
	char * dpath = NULL;
	apr_size_t size = 0;
	apr_file_t * fds = NULL;
	apr_status_t status = 0;
	apr_off_t fsize = 0;

	s_cfg = ap_get_module_config(r->server->module_config,
								 &receta_module);

	size = strlen(s_cfg->data_path) + strlen(type) + 7; /* 6 = .html\0 */
	dpath = apr_palloc(r->pool, size);

	if (! dpath) {
		logp ("Error: can't allocate memory.");
		return -1;
	}

	logp ("Memory allocated.");

	apr_snprintf (dpath, size, "%s/%s.html", s_cfg->data_path, type);

	logp ("File to read: '%s'.", dpath);

	if (md_stat_file(r, dpath, &fsize) == -1) {
		return -1;
	}
	
	status = apr_file_open(&fds, dpath, APR_READ, APR_OS_DEFAULT, r->pool);

	if (status != APR_SUCCESS) {
		logp ("Error: can't open file '%s'.", dpath);
		ap_rprintf (r, "ERR_CANT_FIND_FILE");
		return -1;
	}

	if (md_parse_data_file (r, sql, fds, fsize, pairs) == -1) {
		apr_file_close (fds);
		return -1;
	}

	apr_file_close (fds);

	return 0;

	my_init (sql);

	if (my_query(sql, "select * from menus") == 0) {
		logp ("The query was correct.");
		int i = 0;

		if (my_get_results(sql) == -1) {
			logp ("There was an error obtaining the query results.");
		}

		logp ("Creating the table.");
		ap_rprintf (r, "<table border=\"1\">\n");

		ap_rprintf (r, "<tr>\n");
		for (i = 0; i < sql->num_fields; ++i) {
			ap_rprintf (r, "<th>%s</th>", sql->fields[i].name);
		}
		ap_rprintf (r, "</tr>\n");
				
		for (row = 0; row < sql->num_rows; ++row) {

			my_get_next_row (sql);
				
			ap_rprintf (r, "<tr>\n");
			for (i = 0; i < sql->num_fields; ++i) {
				ap_rprintf (r, "\t<td>%s</td>\n", sql->row[i]);
			}
			ap_rprintf (r, "</tr>");
		}

		ap_rprintf (r, "</table>");
	}
	my_close (sql);

	return 0;
}

/**
 * Reads and parses a data file.
 *
 */
int md_parse_data_file (request_rec * r,
						sql_t * sql,
						apr_file_t * f,
						apr_off_t fsize,
						pair_t ** pairs)
{
	logf ();
	char * buf = NULL;
	char * sql_cmd = NULL;
	char * opts = NULL;
	char * data = NULL;

	buf = apr_palloc(r->pool, fsize + 1);

	if (! buf) {
		logp ("Error: can't allocate memory.");
		return -1;
	}

	if (apr_file_read(f, buf, &fsize) != APR_SUCCESS) {
		logp ("Error: can't read the whole data file into memory.");
		ap_rprintf (r, "ERR_SERVER_INTERNAL_ERROR");
		return -1;
	}

	buf[fsize] = '\0';

	logp ("File contents: '%s'.", buf);

	if (md_get_options(r, buf, &opts, pairs) == -1) {
		return -1;
	}

	if (md_get_sql_command(r, buf, &sql_cmd, pairs) == -1) {
		return -1;
	}

	if (md_get_data(r, buf, &data, pairs) == -1) {
		return -1;
	}

	return 0;
}

/**
 * Gets process options from data file.
 *
 */
int md_get_options (request_rec * r, const char * buf, char ** var, pair_t ** pairs)
{
	logf ();
	int f = 0;
	int e = 0;
	char * opts = NULL;

	f = strstr(buf, "<opts>") - buf;
	e = strstr(buf, "</opts>") - buf;
	
	logp ("<opts> begin %d.", f);
	logp ("</opts> end %d.", e);

	if (f < 0 || e < 0) {
		logp ("Error: can't find <opts> or </opts>.");
		ap_rprintf (r, "ERR_WRONG_PAGE_FORMAT");
		return -1;
	}

	logp ("Getting up to %d bytes.", e - (f + 6));
	
	/* 6 is "<opts>" length, wich we must skip */
	opts = apr_pstrndup(r->pool, &buf[f + 6], e - (f + 6));

	if (! opts) {
		logp ("Error: can't allocate memory.");
		ap_rprintf (r, "ERR_SERVER_INTERNAL_ERROR");
		return -1;
	}

	md_line_return_to_space (opts);
	logp ("Options extracted: '%s'.", opts);

	return 0;
}

/**
 * Get SQL command from data file.
 *
 */
int md_get_sql_command (request_rec * r, const char * buf, char ** var, pair_t ** pairs)
{
	logf ();
	return 0;
}

/**
 * Gets html output from data file.
 *
 */
int md_get_data (request_rec * r, const char * buf, char ** var, pair_t ** pairs)
{
	logf ();
	int f = 0;

	logp ("1");
	f = strstr(buf, "<begin>") - buf;
	
	if (f < 0) {
		logp ("Error: can't find <begin>.");
		ap_rprintf (r, "ERR_WRONG_PAGE_FORMAT");
		return -1;
	}

	logp ("Begin found at %d.", f);

	ap_rprintf (r, "%s", &buf[f + 7]);
	
	return 0;
}

/**
 * Convert line return characters into spaces.
 *
 */
int md_line_return_to_space (char * str)
{
	logf ();


	while (str && *str) {
		if (*str == '\n')
			*str = ' ';
		++str;
	}

	return 0;
}

/**
 * Verify if a file exists and if so, get file size.
 *
 */
int md_stat_file (request_rec * r, const char * fname, apr_off_t * size)
{
	logf ();
	apr_finfo_t finfo;

	if (apr_stat(&finfo, fname, APR_FINFO_SIZE, r->pool) != APR_SUCCESS) {
		logp ("Error: can't stat file '%s'.", fname);
		ap_rprintf (r, "ERR_CANT_FIND_FILE");
		return -1;
	}

	*size = finfo.size;

	return 0;
}

/**
 * Makes an array with GET parameters
 *
 */
int md_make_get_params (request_rec * r, pair_t *** putain)
{
	int params = 0;
	int i = 0;
	const char * p = r->args;

	ap_unescape_url (r->args);
	logp ("Args:            %s", r->args);

	/* if we are here, there is at least one parameter (because
	 * r->args != null), so begin this thing with a count of one.
	 */
	for (params = 1; p && *p; ++p) {
		if (*p == '&')
			++params;
	}

	logp ("Args count:      %d", params);

	p = r->args;

	*putain = apr_palloc(r->pool, (params + 2) * sizeof(pair_t *));

	/* memset (, 0, sizeof(pair_t *) * (params + 2)); */

	for (i = 0; i < params; ++i) {
		logp ("Item: %d", i);
		pair_t * pair = apr_palloc(r->pool, sizeof(pair_t));
		char * pairstring = ap_getword(r->pool, &p, '&');
			
		logp ("Visiting: '%s'.", pairstring);

		if (ap_strchr(pairstring, '=')) {
			pair->name = ap_getword(r->pool,
									(const char **) &pairstring,
									'=');
			/*
			 * The second '&' does not exist, so it will stop at
			 * first null (I guess).
			 */
			pair->value = ap_getword(r->pool,
									 (const char **) &pairstring,
									 '&');
		} else {
			pair->name = pairstring;
		}

		logp ("Value: '%s'='%s'",
			  pair->name ? : "(null)",
			  pair->value ? : "(null)"); 

		(*putain)[i] = pair;
		logp ("ok");
	}

	logp ("Processed %d items", i);

	return 0;
}

const char * md_get_getvalue (const char * name, pair_t ** vars)
{
	logf ();

	int i = md_value_exists(name, vars);

	if (i != error)
		return vars[i]->value;

	return NULL;
}

int md_make_value_index (int index, pair_t ** pairs)
{
	int ret = error;

	if (strncasecmp(pairs[index]->value, "login", 1) == 0) {
		ret = login;
	} else if (strncasecmp(pairs[index]->value, "err", 3) == 0) {
		ret = err;
	} else if (strncasecmp(pairs[index]->value, "opts", 4) == 0) {
		ret = opts;
	} else {
		ret = error;
	}

	return ret;
}

int md_value_exists (const char * name, pair_t ** pairs)
{
	logf ();
	int i = 0;

	for (; pairs[i]; ++i) {
		logp ("Pair %s: %s, %s", name, pairs[i]->name, pairs[i]->value);
		if (strcmp(pairs[i]->name, name) == 0)
			return i;
	}

	return -1;
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
	create_modreceta_config,
	NULL,
	mod_receta_cmds,
	mod_receta_register_hooks,			/* callback for registering hooks */
};

sql_t sql =
{
    my_init,
	{ {NULL} },
	NULL,
	NULL,
	NULL,
	0,
	0,
	0,
	NULL,
	NULL,
	NULL
};


/*
 * Old stuff (from the example) we don't use.
 *
 *
 */
static int mod_receta_method_handler (request_rec * r)
{
#if 0
	modreceta_config * s_cfg = ap_get_module_config(r->server->module_config,
													&receta_module);

	// Send a message to stderr (apache redirects this to the error log)
	/* fprintf (stderr, "%s", s_cfg->string); */

	// We need to flush the stream so that the message appears right away.
	// Performing an fflush() in a production system is not good for
	// performance - don't do this for real.
	fflush (stderr);

	// Return DECLINED so that the Apache core will keep looking for
	// other modules to handle this request.  This effectively makes
	// this module completely transparent.
#endif
	return DECLINED;
}
