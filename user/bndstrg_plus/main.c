/*
 ***************************************************************************
 * MediaTek Inc. 
 *
 * All rights reserved. source code is an unpublished work and the
 * use of a copyright notice does not imply otherwise. This source code
 * contains confidential trade secret material of MediaTek. Any attemp
 * or participation in deciphering, decoding, reverse engineering or in any
 * way altering the source code is stricitly prohibited, unless the prior
 * written consent of MediaTek, Inc. is obtained.
 ***************************************************************************

    Module Name:
    	main.c
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>

#include "bndstrg.h"

extern struct bndstrg_event_ops bndstrg_event_ops;
u8 H5G_channel_thres = H_CHANNEL_BIGGER_THAN;
u8 fg_only_allow_monitor_addr = 0;
struct bndstrg bndstrg;


int usage()
{

	DBGPRINT(DEBUG_OFF, "bndstrg2 -i <interface> [-d <debug level>] [-a <0|1>]\n");
	DBGPRINT(DEBUG_OFF, "-i \tinterface name\n");
	DBGPRINT(DEBUG_OFF, "\t\t-i ra0 -i rai0 -i rae0 for triband\n");
	DBGPRINT(DEBUG_OFF, "\t\t-i ra0 -i rai0 for dbdc\n");
	DBGPRINT(DEBUG_OFF, "-c \thigh band channel\n");
	DBGPRINT(DEBUG_OFF, "\t\t5G channel\n");
	DBGPRINT(DEBUG_OFF, "-a \tonly allow monitor address packet(debug purpose,defalut is 0)\n");
	DBGPRINT(DEBUG_OFF, "\t\t<0|1>\n");
	DBGPRINT(DEBUG_OFF, "-d \tdebug level\n");
	DBGPRINT(DEBUG_OFF, "\t\t<0|1|2|3|4>\n");
	DBGPRINT(DEBUG_OFF, "-h \thelp\n");
	return 0;
}

int process_inf(char *IfName)
{
    struct bndstrg_ctrl_iface *ctrl_iface = &bndstrg.ctrl_iface;
    struct bndstrg_iface *inf = NULL, *new_inf = NULL;
    u8 i, bFound = FALSE;

    if((ctrl_iface->Size <= MAX_INF_NUM) && (strlen(IfName) > 0))
    {
        for(i=0; i<MAX_INF_NUM; i++)
        {
            inf = &ctrl_iface->inf[i];
            if(inf->bValid)
            {
                if(!strcmp(inf->ucIfName, IfName))
                {
                    bFound = TRUE;
                    break;
                }
            }
            else
            {
                if(!new_inf)
                    new_inf = inf;
            }
        }

        if(!bFound)
        {
            new_inf->bValid = TRUE;
            strcpy(new_inf->ucIfName, IfName);
            ctrl_iface->Size++;
        }
    }

    return 0;
}


int process_options(int argc, char *argv[], char *filename,
					int *opmode, int *drv_mode, int *debug_level, int *version)
{
	int c;
	char *cvalue = NULL;
	
	opterr = 0;
	*debug_level = DebugLevel;
	while ((c = getopt(argc, argv, ":d:v:i:a:c:h")) != -1) {
		switch (c) {
		case 'd':
			cvalue = optarg;
			if (os_strcmp(cvalue, "0") == 0)
				*debug_level = DEBUG_OFF;
			else if (os_strcmp(cvalue, "1") == 0)
				*debug_level = DEBUG_ERROR;
			else if (os_strcmp(cvalue, "2") == 0)
				*debug_level = DEBUG_WARN;
			else if (os_strcmp(cvalue, "3") == 0)
				*debug_level = DEBUG_TRACE;
			else if (os_strcmp(cvalue, "4") == 0)
				*debug_level = DEBUG_INFO;
			else {
				DBGPRINT(DEBUG_ERROR, "-d option does not have this debug_level %s\n", cvalue);
				return - 1;
			}
			break;
		case 'f':
			cvalue = optarg;
			os_strcpy(filename, cvalue);
			break;
		case 'v':
			cvalue = optarg;
			*version = atoi(cvalue);
 			break;
        case 'i':
            cvalue = optarg;
            process_inf(cvalue);
            break;
		case 'c':// high band channel definition
            cvalue = optarg;
			if ((atoi(cvalue) < 36) || (atoi(cvalue) > 165)) {
				H5G_channel_thres = H_CHANNEL_BIGGER_THAN;
			} else {
				H5G_channel_thres = atoi(cvalue);
			}
            break;
		case 'a':
            cvalue = optarg;
			if ((atoi(cvalue) > 1) ||
				(atoi(cvalue) < 0))
				fg_only_allow_monitor_addr = 0;
			else
				fg_only_allow_monitor_addr = atoi(cvalue);
            break;
		case 'h':
			return -1;
			break;
		case ':':
			if (isprint(optopt))
				DBGPRINT(DEBUG_OFF, "Option -%c requires an argument\n",optopt);
			return -1;
		case '?':
			if (isprint(optopt))
				DBGPRINT(DEBUG_OFF, "Option -%c is invalid\n",optopt);
			return -1;
		}
	}
	return 0;

}

int main(int argc, char *argv[])
{

	int ret;
	int opmode;
	int drv_mode;
	int debug_level;
	int version = 2;
	char filename[256] = {0}; 
	pid_t child_pid;

	/* default setting */
    memset(&bndstrg.ctrl_iface, 0, sizeof(struct bndstrg_ctrl_iface));

	/* options processing */
	ret = process_options(argc, argv, filename, &opmode, &drv_mode, &debug_level, &version);
	if (ret) {
		usage();
		return -1;
	}

	// init bndstrg state
	bndstrg.state = BNDSTRG_INIT;
    /* default use 2g as main interface for  nvram table */
    if(bndstrg.ctrl_iface.Size == 0){
        process_inf(IFNAME_2G);
    }
	DebugLevel = debug_level;
	
	child_pid = fork();

	if (child_pid == 0) {	
		DBGPRINT(DEBUG_OFF, "Initialize bndstrg\n");
		ret = bndstrg_init(&bndstrg, &bndstrg_event_ops, drv_mode, opmode, version);
	
		if (ret)
			goto error;

		bndstrg_run(&bndstrg);

	} else
		return 0;
#if 0
error0:
	bndstrg_deinit(&hs);
#endif
error:
	
	return ret;

}
