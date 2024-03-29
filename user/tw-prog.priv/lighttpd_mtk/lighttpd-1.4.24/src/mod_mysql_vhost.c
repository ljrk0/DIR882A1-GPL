#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <strings.h>

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#ifdef HAVE_MYSQL
#include <mysql.h>
#endif

#include "plugin.h"
#include "log.h"

#include "stat_cache.h"
#ifdef DEBUG_MOD_MYSQL_VHOST
#define DEBUG
#endif

/*
 * Plugin for lighttpd to use MySQL
 *   for domain to directory lookups,
 *   i.e virtual hosts (vhosts).
 *
 * Optionally sets fcgi_offset and fcgi_arg
 *   in preparation for fcgi.c to handle
 *   per-user fcgi chroot jails.
 *
 * /ada@riksnet.se 2004-12-06
 */

#ifdef HAVE_MYSQL
typedef struct {
	MYSQL 	*mysql;

	buffer  *mydb;
	buffer  *myuser;
	buffer  *mypass;
	buffer  *mysock;

	buffer  *hostname;
	unsigned short port;

	buffer  *mysql_pre;
	buffer  *mysql_post;
} plugin_config;

/* global plugin data */
typedef struct {
	PLUGIN_DATA;

	buffer 	*tmp_buf;

	plugin_config **config_storage;

	plugin_config conf;
} plugin_data;

/* per connection plugin data */
typedef struct {
	buffer	*server_name;
	buffer	*document_root;
	buffer	*fcgi_arg;
	unsigned fcgi_offset;
} plugin_connection_data;

/* init the plugin data */
INIT_FUNC(mod_mysql_vhost_init) {
	plugin_data *p;

	p = calloc(1, sizeof(*p));

	p->tmp_buf = buffer_init();

	return p;
}

/* cleanup the plugin data */
SERVER_FUNC(mod_mysql_vhost_cleanup) {
	plugin_data *p = p_d;

	UNUSED(srv);

#ifdef DEBUG
	log_error_write(srv, __FILE__, __LINE__, "ss",
		"mod_mysql_vhost_cleanup", p ? "yes" : "NO");
#endif
	if (!p) return HANDLER_GO_ON;

	if (p->config_storage) {
		size_t i;
		for (i = 0; i < srv->config_context->used; i++) {
			plugin_config *s = p->config_storage[i];

			if (!s) continue;

			mysql_close(s->mysql);

			buffer_free(s->mydb);
			buffer_free(s->myuser);
			buffer_free(s->mypass);
			buffer_free(s->mysock);
			buffer_free(s->mysql_pre);
			buffer_free(s->mysql_post);
			buffer_free(s->hostname);

			free(s);
		}
		free(p->config_storage);
	}
	buffer_free(p->tmp_buf);

	free(p);

	return HANDLER_GO_ON;
}

/* handle the plugin per connection data */
static void* mod_mysql_vhost_connection_data(server *srv, connection *con, void *p_d)
{
	plugin_data *p = p_d;
	plugin_connection_data *c = con->plugin_ctx[p->id];

	UNUSED(srv);

#ifdef DEBUG
        log_error_write(srv, __FILE__, __LINE__, "ss",
		"mod_mysql_connection_data", c ? "old" : "NEW");
#endif

	if (c) return c;
	c = calloc(1, sizeof(*c));

	c->server_name = buffer_init();
	c->document_root = buffer_init();
	c->fcgi_arg = buffer_init();
	c->fcgi_offset = 0;

	return con->plugin_ctx[p->id] = c;
}

/* destroy the plugin per connection data */
CONNECTION_FUNC(mod_mysql_vhost_handle_connection_close) {
	plugin_data *p = p_d;
	plugin_connection_data *c = con->plugin_ctx[p->id];

	UNUSED(srv);

#ifdef DEBUG
	log_error_write(srv, __FILE__, __LINE__, "ss",
		"mod_mysql_vhost_handle_connection_close", c ? "yes" : "NO");
#endif

	if (!c) return HANDLER_GO_ON;

	buffer_free(c->server_name);
	buffer_free(c->document_root);
	buffer_free(c->fcgi_arg);
	c->fcgi_offset = 0;

	free(c);

	con->plugin_ctx[p->id] = NULL;
	return HANDLER_GO_ON;
}

/* set configuration values */
SERVER_FUNC(mod_mysql_vhost_set_defaults) {
	plugin_data *p = p_d;

	char *qmark;
	size_t i = 0;

	config_values_t cv[] = {
		{ "mysql-vhost.db",	NULL, T_CONFIG_STRING, 	T_CONFIG_SCOPE_SERVER },
		{ "mysql-vhost.user",	NULL, T_CONFIG_STRING, 	T_CONFIG_SCOPE_SERVER },
		{ "mysql-vhost.pass",	NULL, T_CONFIG_STRING, 	T_CONFIG_SCOPE_SERVER },
		{ "mysql-vhost.sock",	NULL, T_CONFIG_STRING, 	T_CONFIG_SCOPE_SERVER },
		{ "mysql-vhost.sql",	NULL, T_CONFIG_STRING, 	T_CONFIG_SCOPE_SERVER },
		{ "mysql-vhost.hostname", NULL, T_CONFIG_STRING,T_CONFIG_SCOPE_SERVER },
		{ "mysql-vhost.port",   NULL, T_CONFIG_SHORT,   T_CONFIG_SCOPE_SERVER },
                { NULL,			NULL, T_CONFIG_UNSET,	T_CONFIG_SCOPE_UNSET }
        };

	p->config_storage = calloc(1, srv->config_context->used * sizeof(specific_config *));

	for (i = 0; i < srv->config_context->used; i++) {
		plugin_config *s;
		buffer *sel;


		s = calloc(1, sizeof(plugin_config));
		s->mydb = buffer_init();
		s->myuser = buffer_init();
		s->mypass = buffer_init();
		s->mysock = buffer_init();
		s->hostname = buffer_init();
		s->port   = 0;               /* default port for mysql */
		sel = buffer_init();
		s->mysql = NULL;

		s->mysql_pre = buffer_init();
		s->mysql_post = buffer_init();

		cv[0].destination = s->mydb;
		cv[1].destination = s->myuser;
		cv[2].destination = s->mypass;
		cv[3].destination = s->mysock;
		cv[4].destination = sel;
		cv[5].destination = s->hostname;
		cv[6].destination = &(s->port);

		p->config_storage[i] = s;

        	if (config_insert_values_global(srv,
			((data_config *)srv->config_context->data[i])->value,
			cv)) return HANDLER_ERROR;

		s->mysql_pre = buffer_init();
		s->mysql_post = buffer_init();

		if (sel->used && (qmark = strchr(sel->ptr, '?'))) {
			*qmark = '\0';
			buffer_copy_string(s->mysql_pre, sel->ptr);
			buffer_copy_string(s->mysql_post, qmark+1);
		} else {
			buffer_copy_string_buffer(s->mysql_pre, sel);
		}

		/* required:
		 * - username
		 * - database
		 *
		 * optional:
		 * - password, default: empty
		 * - socket, default: mysql default
		 * - hostname, if set overrides socket
		 * - port, default: 3306
		 */

		/* all have to be set */
		if (!(buffer_is_empty(s->myuser) ||
		      buffer_is_empty(s->mydb))) {
			my_bool reconnect = 1;

			if (NULL == (s->mysql = mysql_init(NULL))) {
				log_error_write(srv, __FILE__, __LINE__, "s", "mysql_init() failed, exiting...");

				return HANDLER_ERROR;
			}

#if MYSQL_VERSION_ID >= 50013
			/* in mysql versions above 5.0.3 the reconnect flag is off by default */
			mysql_options(s->mysql, MYSQL_OPT_RECONNECT, &reconnect);
#endif

#define FOO(x) (s->x->used ? s->x->ptr : NULL)

#if MYSQL_VERSION_ID >= 40100
                        /* CLIENT_MULTI_STATEMENTS first appeared in 4.1 */ 
			if (!mysql_real_connect(s->mysql, FOO(hostname), FOO(myuser), FOO(mypass),
						FOO(mydb), s->port, FOO(mysock), CLIENT_MULTI_STATEMENTS)) {
#else
			if (!mysql_real_connect(s->mysql, FOO(hostname), FOO(myuser), FOO(mypass),
						FOO(mydb), s->port, FOO(mysock), 0)) {
#endif
				log_error_write(srv, __FILE__, __LINE__, "s", mysql_error(s->mysql));

				return HANDLER_ERROR;
			}
#undef FOO

#if 0
			/* set close_on_exec for mysql the hard way */
			/* Note: this only works as it is done during startup, */
			/* otherwise we cannot be sure that mysql is fd i-1 */
			{ int fd;
			if (-1 != (fd = open("/dev/null", 0))) {
				close(fd);
#ifdef FD_CLOEXEC
				fcntl(fd-1, F_SETFD, FD_CLOEXEC);
#endif
			} }
#else
#ifdef FD_CLOEXEC
			fcntl(s->mysql->net.fd, F_SETFD, FD_CLOEXEC);
#endif
#endif
		}
	}

	return HANDLER_GO_ON;
}

#define PATCH(x) \
	p->conf.x = s->x;
static int mod_mysql_vhost_patch_connection(server *srv, connection *con, plugin_data *p) {
	size_t i, j;
	plugin_config *s = p->config_storage[0];

	PATCH(mysql_pre);
	PATCH(mysql_post);
#ifdef HAVE_MYSQL
	PATCH(mysql);
#endif

	/* skip the first, the global context */
	for (i = 1; i < srv->config_context->used; i++) {
		data_config *dc = (data_config *)srv->config_context->data[i];
		s = p->config_storage[i];

		/* condition didn't match */
		if (!config_check_cond(srv, con, dc)) continue;

		/* merge config */
		for (j = 0; j < dc->value->used; j++) {
			data_unset *du = dc->value->data[j];

			if (buffer_is_equal_string(du->key, CONST_STR_LEN("mysql-vhost.sql"))) {
				PATCH(mysql_pre);
				PATCH(mysql_post);
			}
		}

		if (s->mysql) {
			PATCH(mysql);
		}
	}

	return 0;
}
#undef PATCH


/* handle document root request */
CONNECTION_FUNC(mod_mysql_vhost_handle_docroot) {
	plugin_data *p = p_d;
	plugin_connection_data *c;
	stat_cache_entry *sce;

	unsigned  cols;
	MYSQL_ROW row;
	MYSQL_RES *result = NULL;

	/* no host specified? */
	if (!con->uri.authority->used) return HANDLER_GO_ON;

	mod_mysql_vhost_patch_connection(srv, con, p);

	if (!p->conf.mysql) return HANDLER_GO_ON;
	if (0 == p->conf.mysql_pre->used) return HANDLER_GO_ON;

	/* sets up connection data if not done yet */
	c = mod_mysql_vhost_connection_data(srv, con, p_d);

	/* check if cached this connection */
	if (c->server_name->used && /* con->uri.authority->used && */
            buffer_is_equal(c->server_name, con->uri.authority)) goto GO_ON;

	/* build and run SQL query */
	buffer_copy_string_buffer(p->tmp_buf, p->conf.mysql_pre);
	if (p->conf.mysql_post->used) {
	/* escape the uri.authority */
		unsigned long to_len;

		/* 'to' has to be 'from_len * 2 + 1' */
		buffer_prepare_append(p->tmp_buf, (con->uri.authority->used - 1) * 2 + 1);

		to_len = mysql_real_escape_string(p->conf.mysql,
				p->tmp_buf->ptr + p->tmp_buf->used - 1,
				con->uri.authority->ptr, con->uri.authority->used - 1);
		p->tmp_buf->used += to_len;
		
		buffer_append_string_buffer(p->tmp_buf, p->conf.mysql_post);
	}
   	if (mysql_real_query(p->conf.mysql, p->tmp_buf->ptr, p->tmp_buf->used - 1)) {
		log_error_write(srv, __FILE__, __LINE__, "s", mysql_error(p->conf.mysql));
		goto ERR500;
	}
	result = mysql_store_result(p->conf.mysql);
	cols = mysql_num_fields(result);
	row = mysql_fetch_row(result);
	if (!row || cols < 1) {
		/* no such virtual host */
		mysql_free_result(result);
#if MYSQL_VERSION_ID >= 40100
		while (mysql_next_result(p->conf.mysql) == 0);
#endif
		return HANDLER_GO_ON;
	}

	/* sanity check that really is a directory */
	buffer_copy_string(p->tmp_buf, row[0]);
	BUFFER_APPEND_SLASH(p->tmp_buf);

	if (HANDLER_ERROR == stat_cache_get_entry(srv, con, p->tmp_buf, &sce)) {
		log_error_write(srv, __FILE__, __LINE__, "sb", strerror(errno), p->tmp_buf);
		goto ERR500;
	}
        if (!S_ISDIR(sce->st.st_mode)) {
		log_error_write(srv, __FILE__, __LINE__, "sb", "Not a directory", p->tmp_buf);
		goto ERR500;
	}

	/* cache the data */
	buffer_copy_string_buffer(c->server_name, con->uri.authority);
	buffer_copy_string_buffer(c->document_root, p->tmp_buf);

	/* fcgi_offset and fcgi_arg are optional */
	if (cols > 1 && row[1]) {
		c->fcgi_offset = atoi(row[1]);

		if (cols > 2 && row[2]) {
			buffer_copy_string(c->fcgi_arg, row[2]);
		} else {
			c->fcgi_arg->used = 0;
		}
	} else {
		c->fcgi_offset = c->fcgi_arg->used = 0;
	}
	mysql_free_result(result);
#if MYSQL_VERSION_ID >= 40100
	while (mysql_next_result(p->conf.mysql) == 0);
#endif

	/* fix virtual server and docroot */
GO_ON:	buffer_copy_string_buffer(con->server_name, c->server_name);
	buffer_copy_string_buffer(con->physical.doc_root, c->document_root);

#ifdef DEBUG
	log_error_write(srv, __FILE__, __LINE__, "sbbdb",
		result ? "NOT CACHED" : "cached",
		con->server_name, con->physical.doc_root,
		c->fcgi_offset, c->fcgi_arg);
#endif
	return HANDLER_GO_ON;

ERR500:	if (result) mysql_free_result(result);
#if MYSQL_VERSION_ID >= 40100
	while (mysql_next_result(p->conf.mysql) == 0);
#endif
	con->http_status = 500; /* Internal Error */
	con->mode = DIRECT;
	return HANDLER_FINISHED;
}

/* this function is called at dlopen() time and inits the callbacks */
int mod_mysql_vhost_plugin_init(plugin *p);
int mod_mysql_vhost_plugin_init(plugin *p) {
	p->version        = LIGHTTPD_VERSION_ID;
	p->name           = buffer_init_string("mysql_vhost");

	p->init           = mod_mysql_vhost_init;
	p->cleanup        = mod_mysql_vhost_cleanup;
	p->connection_reset = mod_mysql_vhost_handle_connection_close;

	p->set_defaults   = mod_mysql_vhost_set_defaults;
	p->handle_docroot = mod_mysql_vhost_handle_docroot;

	return 0;
}
#else
/* we don't have mysql support, this plugin does nothing */
int mod_mysql_vhost_plugin_init(plugin *p);
int mod_mysql_vhost_plugin_init(plugin *p) {
	p->version     = LIGHTTPD_VERSION_ID;
	p->name        = buffer_init_string("mysql_vhost");

	return 0;
}
#endif
