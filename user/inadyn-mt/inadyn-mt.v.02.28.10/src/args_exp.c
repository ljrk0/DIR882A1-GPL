/*
Copyright (C) 2015 Bryan Hoover (bhoover@wecs.com)

This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 3
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
        Variable/identifier parsing.

        Author:	bhoover@wecs.com
        Date:	May 2015

        History:
                - first implemetnation
*/

#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "errorcode.h"
#include "safe_mem.h"
#include "args_exp.h"
#include "debug_if.h"
#include "unicode_util.h"

#define MODULE_TAG "ARGS_EXP: "

static RC_TYPE push_in_buffer(char* p_src, int src_len, char *p_buffer, int* p_act_len, int max_len)
{
	if (*p_act_len + src_len > max_len)

		return RC_FILE_IO_OUT_OF_BUFFER;

	memcpy(p_buffer + *p_act_len, p_src, src_len);

	*p_act_len += src_len;

	return RC_OK;
}

typedef enum
{
    DATA,
    PAREN_OPEN,
    VAR,
    ESCAPE
} PARSER_STATE;

typedef struct
{
	FILE		*p_file;
	PARSER_STATE	state;
} OPTION_FILE_PARSER;

static RC_TYPE parser_init(OPTION_FILE_PARSER *p_cfg, FILE *p_file)
{
	memset(p_cfg, 0, sizeof(*p_cfg));
	p_cfg->state = DATA;
	p_cfg->p_file = p_file;
	return RC_OK;
}

static RC_TYPE parser_read(OPTION_FILE_PARSER *p_cfg,
                           char *p_buffer,
                           int maxlen,
                           char *ch,
                           int *count,
                           int *parse_end,
                           EXP_HANDLER arg_to_val,
                           void *p_context)
{

	RC_TYPE rc = RC_OK;


	switch (p_cfg->state)
	{

	case DATA:

		if (!(strcmp(ch,"\\")))
		{
			p_cfg->state = ESCAPE;

			return RC_OK;
		}

		if (!(strcmp(ch,"("))) {

			p_cfg->state = PAREN_OPEN;

			return RC_OK;
		}

		/*actual data*/

		return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);

	case PAREN_OPEN:

		if (!(strcmp(ch,"\\")))
		{
			p_cfg->state = ESCAPE;

			return rc;
		}

		if (strcmp(ch,"$")) {

			p_cfg->state = DATA;

			return push_in_buffer(ch, strlen(ch), p_buffer, count, maxlen);
		}
		else {
			p_cfg->state = VAR;

			*parse_end=(!(0==strlen(p_buffer)));

			return RC_OK;
		}

	case VAR:

		if (strcmp(ch,")"))

			return push_in_buffer(ch,strlen(ch),p_buffer,count,maxlen);

		{
			char	*p_var_val=arg_to_val(p_context,p_buffer);
			RC_TYPE	rc;

			*parse_end=1;
			p_cfg->state=DATA;

			memset(p_buffer,0,maxlen);

			*count=0;

			rc=push_in_buffer(p_var_val,strlen(p_var_val),p_buffer,count,maxlen);

			free(p_var_val);

			return rc;
		}

	case ESCAPE:

		p_cfg->state = DATA;

		if (!(strcmp(ch,"n")))

			return push_in_buffer("\n",1,p_buffer,count,maxlen);

		if (!(strcmp(ch,"r")))

			return push_in_buffer("\r",1,p_buffer,count,maxlen);

		if (!(strcmp(ch,"\\")))

			rc = push_in_buffer("\\",1,p_buffer,count,maxlen);
		else

			rc = push_in_buffer(ch,strlen(ch),p_buffer,count, maxlen);

		return rc;

	default:

		return rc;
	}
}

/*Map string identifier to corresponding value as defined by client EXP_HANDLER, pf_arg_to_val.

  input:  a simple string, no format specs, no cr/lf, possible a variable symbol (token prefaced with '$'), to be mapped to a value.

  output:  in_str with variable tokens replaced with their corresponding expansions (values).
*/
char *vars_expanded_str(EXP_HANDLER pf_arg_to_val,void *p_context,char **s_exp,char *in_str)
{
	RC_TYPE			rc;
	int			parse_end=0;
	int			count=0;
	char			ch[7];
	OPTION_FILE_PARSER	p_cfg;
	char			*p_buffer;
	int			max_len=1023;


	if (!(in_str))

		return NULL;

	if (!(strlen(in_str)))

		return NULL;

	*s_exp=safe_malloc(1);

	p_buffer=safe_malloc(max_len+1);

	parser_init(&p_cfg,NULL);


	while(*in_str) {

		memset(ch,0,7);

		utf_8_get_char(ch,utf_8_code_point(in_str));

		in_str=utf_next_char(in_str,0);


		if (!(RC_OK==(rc=parser_read(&p_cfg,p_buffer,max_len,ch,&count,&parse_end,pf_arg_to_val,p_context)))) {

			DBG_PRINTF((LOG_ERR,"E:" MODULE_TAG "Error parsing in vars_expanded_str:  %s...\n",errorcode_get_name(rc)));

			free(*s_exp);
			*s_exp=NULL;

			break;
		}
		else {
			if (parse_end || !(*in_str)) {

				int	s_size=strlen(*s_exp)+1;


				DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "reallocing:  s_exp:  %s| p_buffer:  %s|...\n",*s_exp,p_buffer));

				*s_exp=safe_realloc(*s_exp,s_size+strlen(p_buffer));
				strcat(*s_exp,p_buffer);

				DBG_PRINTF((LOG_INFO,"I:" MODULE_TAG "after reallocing:  s_exp:  %s| p_buffer:  %s|\n",*s_exp,p_buffer));

				count=0;
				parse_end=0;
				memset(p_buffer,0,max_len);
			}
		}
	}

	free(p_buffer);

	return *s_exp;
}

