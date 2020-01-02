/*
Copyright (C) 2003-2004 Narcis Ilisei

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
*/
/*
	Inplementation functions for CMD options parsing system
	Author: Narcis Ilisei
	Date: May 2003
	History:
	  - may 15 2003 : first version.
        Basic, table driven option line parsing.
        Nov 2007
        - Changed parser to accomodate, and allow handlers
          to set default options in absence of command
          options.
        - added parser error handling handler -- original
          interface remains, passing NULL handler to new
          interface.
*/

#define MODULE_TAG "GETCMD: "

#ifndef EXCLUDE_CONFIG_H

#include "config.h"

#endif

#include <string.h>
#include <stdlib.h>
#include "get_cmd.h"
#include "debug_if.h"
#include "errorcode.h"
#include "safe_mem.h"

static CMD_DESCRIPTION_TYPE * opt_search(CMD_DESCRIPTION_TYPE *p_table, char *p_opt)
{
	CMD_DESCRIPTION_TYPE *it = p_table;

	while( it->p_option != NULL)
	{
		if (strcmp(p_opt, it->p_option) == 0)
		{
			return it;
			break;
		}
		++it;
	}
	return NULL;
}

/**
	 Init the CMD_DATA
*/
RC_TYPE cmd_init(CMD_DATA *p_cmd)
{
	if (!p_cmd)
	{
		return RC_INVALID_POINTER;
	}
	memset(p_cmd, 0, sizeof(*p_cmd));
	return RC_OK;
}

RC_TYPE cmd_destruct(CMD_DATA *p_cmd)
{
	if (!p_cmd)
	{
		return RC_INVALID_POINTER;
	}
	if (p_cmd->argv)
	{
		int i;
		for (i = 0; i < p_cmd->argc; ++i)
		{
			if (p_cmd->argv[i])
			{
				free(p_cmd->argv[i]);
			}
		}
		free(p_cmd->argv);
	}
	return RC_OK;
}

/** Adds a new option (string) to the command line
*/
RC_TYPE cmd_add_val(CMD_DATA *p_cmd, char *p_val, CMD_DESCRIPTION_TYPE *p_cmd_descr)
{
	RC_TYPE rc = RC_OK;

	if (!p_cmd || !p_val)
	{
		return RC_INVALID_POINTER;
	}
	do
	{
		{
			char **pp = (char **) safe_realloc(p_cmd->argv, (p_cmd->argc + 1) * sizeof(char*));

			if (!pp)
			{
				rc = RC_OUT_OF_MEMORY;
				break;
			}
			p_cmd->argv = pp;
		}
		{
			int	p_val_len=strlen(p_val);
			char	*p = (char*) safe_malloc(p_val_len + 1);

			/*convert short option to corresponding long option
			  so that inadyn-mt can use named option parameters
			  preceeded by a single hyphen (-ip_type ip6, as param
			  to --alias, for instance).*/

			if (2==p_val_len)

				if ('-'==p_val[0]) {

					CMD_DESCRIPTION_TYPE	*p_option_entry;

					/*find this short option - corresponding long option is prev in command table*/
					if ((p_option_entry=opt_search(p_cmd_descr,p_val))) {

						p_option_entry--;

						if (p_option_entry->p_option) {

							p_val=p_option_entry->p_option;

							p=safe_realloc(p,strlen(p_val)+1);
						}
					}
				}

			if (!p)
			{
				rc = RC_OUT_OF_MEMORY;
				break;
			}

			strcpy(p, p_val);
			p_cmd->argv[p_cmd->argc] = p;
			p_cmd->argc ++;
		}
	}
	while(0);

	return rc;
}

RC_TYPE cmd_insert_val_after(CMD_DATA *p_cmd, char *p_val,int in_index)
{
	int	dest_pos;
	int	src_pos;

	p_cmd->argv=safe_realloc(p_cmd->argv,(p_cmd->argc + 1) * sizeof(char*));

	for (dest_pos=p_cmd->argc,src_pos=p_cmd->argc-1;src_pos>in_index;dest_pos--,src_pos--) {

		p_cmd->argv[dest_pos]=p_cmd->argv[src_pos];
	}

	p_cmd->argv[dest_pos]=safe_malloc(strlen(p_val)+1);
	strcpy(p_cmd->argv[dest_pos],p_val);

	p_cmd->argc++;

	return RC_OK;
}

/** Creates a struct of argvals from the given command line.
    Action:
        copy the argv from the command line to the given CMD_DATA struct
        set the data val of the list element to the current argv
*/
RC_TYPE cmd_add_vals_from_argv(CMD_DATA *p_cmd, char **argv, int argc, CMD_DESCRIPTION_TYPE *p_cmd_descr)
{
	RC_TYPE rc = RC_OK;
	if (!p_cmd || !argv || !argc)
	{
		return RC_INVALID_POINTER;
	}
	do
	{
		int i;
		for (i = 0; i < argc; ++i)
		{
			rc = cmd_add_val(p_cmd, argv[i], p_cmd_descr);
		}
	}
	while(0);
	return rc;
}

/*of the number of possible command options for current command,
  return the number of parameters that were NOT given.
*/
static int cmd_default_options(char *argv[],int arg_nr,int curr_arg,int argc)
{

	int  i;


	argv+=curr_arg;


	for (i=0;i<arg_nr;i++) {

		if (!(curr_arg+i<argc))

			break;

		if (!(strncmp(argv[i],"--",2)))

			break;
	}

	return arg_nr-i;
}

/*make sure proper number of option args*/
int is_has_option_args(int curr_arg_nr,CMD_DATA *cmd,CMD_DESCRIPTION_TYPE *p_curr_opt) 
{

	int args_required=p_curr_opt->arg_nr-p_curr_opt->p_handler.num_defaults;


	if (curr_arg_nr + args_required > cmd->argc) {

		return 0;
	}
	else {

		int i;

		for (i=0;i<args_required;i++) { /*if arg has a hyphen ('-') it's a new option*/

			if (0==strncmp("--",cmd->argv[curr_arg_nr+i],2)) {

				return 0;
			}
		}
	}

	return 1;
}

/*
	Parses the incoming argv list.
	Arguments:
		argv, argc,
		cmd description

	Action:
		performs a match for every p_option string in the CMD description.
		checks the number of arguments left
		calls the user handler with the pointer to the correct arguments

	Implementation:
		- for each option in the table
			- find it in the argv list
			- check the required number of arguments
			- call the handler
*/

RC_TYPE get_cmd_parse_data_with_error_handling(char **argv, int argc, CMD_DESCRIPTION_TYPE *p_cmd_descr,
        CMD_OPTION_ERR_HANDLER_FUNC service_cmd_error_handler)

{
	RC_TYPE		rc = RC_OK;
	CMD_DATA	cmd;
	int		curr_arg_nr = 1;

	if (argv == NULL || p_cmd_descr == NULL)
	{
		return RC_INVALID_POINTER;
	}

	do
	{
		rc = cmd_init(&cmd);
		if (rc != RC_OK)
		{
			break;
		}
		rc = cmd_add_vals_from_argv(&cmd, argv, argc, p_cmd_descr);
		if (rc != RC_OK)
		{
			break;
		}
		while(curr_arg_nr < cmd.argc)
		{
			CMD_DESCRIPTION_TYPE *p_curr_opt = opt_search(p_cmd_descr, cmd.argv[curr_arg_nr]);

			++curr_arg_nr;

			if (p_curr_opt == NULL)
			{

				rc = RC_CMD_PARSER_INVALID_OPTION;

				if (!(service_cmd_error_handler)) {

					DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Invalid option name at position %d ('%s')\n",
					            curr_arg_nr, cmd.argv[curr_arg_nr-1]));

					break;

				}
				else {

					int pre_handler_curr_arg_nr=curr_arg_nr;

					rc = service_cmd_error_handler(&cmd, &curr_arg_nr, NULL);

					if (rc != RC_OK) {

						DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Invalid option name at position %d ('%s')\n",
						            pre_handler_curr_arg_nr, cmd.argv[pre_handler_curr_arg_nr-1]));


						break;
					}
				}
			}
			else {

				if (!(p_curr_opt->p_handler.p_func))

					DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Warning parsing option %d ('%s'):  No handler\n",
					            curr_arg_nr, cmd.argv[curr_arg_nr-1]));

				else {

					/*check arg nr required by the current option*/
					if (!(is_has_option_args(curr_arg_nr,&cmd,p_curr_opt)))
					{
						rc = RC_CMD_PARSER_INVALID_OPTION_ARGUMENT;
						DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Missing option value at position %d ('%s')\n",
						            curr_arg_nr + 1, p_curr_opt->p_option));
						break;
					}


					rc = p_curr_opt->p_handler.p_func(&cmd, curr_arg_nr, p_curr_opt->p_handler.p_context);


					if (rc != RC_OK)
					{
						DBG_PRINTF((LOG_WARNING,"W:" MODULE_TAG "Error parsing option %d ('%s')\n",
						            curr_arg_nr, cmd.argv[curr_arg_nr-1]));
						break;
					}
				}

				curr_arg_nr += (p_curr_opt->arg_nr-cmd_default_options(cmd.argv,p_curr_opt->arg_nr,curr_arg_nr,
				                cmd.argc));
			}
		}
	}
	while(0);

	cmd_destruct(&cmd);

	return rc;
}

RC_TYPE get_cmd_parse_data(char **argv, int argc, CMD_DESCRIPTION_TYPE *p_cmd_descr)
{

	return get_cmd_parse_data_with_error_handling(argv,argc,p_cmd_descr,NULL);
}

