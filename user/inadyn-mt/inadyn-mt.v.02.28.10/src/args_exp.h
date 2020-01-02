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
        Variable/identifier expansion.

        Author: bhoover@wecs.com
        Date:   May 2015

        History:
                - first implemetnation
*/

typedef char* (*EXP_HANDLER)(void *p_context,char *var);

char *vars_expanded_str(EXP_HANDLER pf_arg_to_val,void *p_context,char **s_exp,char *in_str);
