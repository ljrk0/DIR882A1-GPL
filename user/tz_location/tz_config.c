
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <getopt.h>

#include "tz.h"

#define OPTION_STRING "a:c:Dhp:v"

static void tz_help(const char * pApp_name)
{
	RETURN_IF_NULL(pApp_name, );

	printf( "%s [options]\n"
			"	-a, --action=<action> \t The action will be execute, include:\n"
										"\t\t\t\tsync/daemon.\n"
			"	-c, --config-file=<file> The filename of configuration file.\n"
			"	    --cacert=<file> \t CA certificate to verify fota server, "
						"default: %s.\n"
			"	-D, --daemonize \t Daemonize after startup.\n"
			"	    --debug\n"
					"\t\t\tNot redirect stdio,stdout,stderr when in daemon mode.\n"
			"	-h, --help \t\t Show this help.\n"
			"	-p, --pid-file=<FILE> \t Save pid into FILE.\n"
			"	-v, --verbose \t Make the operation more talkative.\n"
			"	    --version \t Show version number and quit.\n",
			pApp_name,
			CA_CERT_FILE
			);
}

static int tz_parse_cmd(struct tz_config * c, int argc, char * * argv)
{
	int o = -1;
	int iLong_index = 0;

	static const struct option long_options[] =
	{
		{"action", required_argument, NULL, 'a' },
		{"config-file", required_argument, NULL, 'c' },
		{"cacert", required_argument, NULL, 'c' },
		{"daemonize", no_argument, NULL, 'D' },
		{"debug", no_argument, NULL, 'D' },
		{"help", no_argument, NULL, 'h' },
		{"pid-file", required_argument, NULL, 'p' },
		{"verbose", no_argument, NULL, 'v' },
		{"version", no_argument, NULL, 'v' },
		{NULL, 0, NULL, 0 }
	};

	RETURN_IF_NULL(c, -1);

	o = getopt_long(argc, argv, OPTION_STRING,
		long_options, &iLong_index);
	while (o >= 0)
	{
		switch (o)
		{
			case 'a':
				c->pAction = optarg;
				break;
			case 'c':
				if (0 == iLong_index)
				{
					c->pConfig_file = optarg;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"config-file")
				)
				{
					c->pConfig_file = optarg;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"cacert")
				)
				{
					c->pCacert = optarg;
				}

				break;
			case 'D':
				if (0 == iLong_index)
				{
					c->iDaemonize = 1;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"daemonize")
				)
				{
					c->iDaemonize = 1;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"debug"))
				{
					c->iEnable_stdio = 1;
				}

				break;
			case 'h':
				c->iHelp = 1;
				break;
			case 'p':
				if (0 == iLong_index)
				{
					c->pPid_file = optarg;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"pid-file")
				)
				{
					c->pPid_file = optarg;
				}

				break;
			case 'v':
				if (0 == iLong_index)
				{
					c->iVerbose = 1;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"verbose"))
				{
					c->iVerbose = 1;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"version"))
				{
					printf("fota tz client version: %s.\n", TZ_VERSION);
					exit(EXIT_SUCCESS);
				}

				break;
			default:
				return -1;
		}

		iLong_index = 0;

		o = getopt_long(argc, argv, OPTION_STRING,
			long_options, &iLong_index);
	}

	if (optind < argc)
	{
		DEBUG("Too many arguments.\n");
		return -1;
	}

	return 0;
}

static int tz_auto_generate_config_file(
									const char * pFile_name)
{
	char sFile_name[256] = { 0 };
	char * argv[] =
		{
			FOTA_CONFIG_TOOL_PATH,
			"config",
			NULL,
			NULL,
			NULL
		};
	int iResult_code = -1;

	RETURN_IF_NULL(pFile_name, -1);

	snprintf(sFile_name, sizeof (sFile_name), "%s", pFile_name);
	argv[2] = sFile_name;

	argv[3] = "fw-encrypt";

	if (execute_action(FOTA_CONFIG_TOOL_PATH, argv,
		&iResult_code) < 0)
	{
		DEBUG("execute %s fail!\n", FOTA_CONFIG_TOOL_PATH);
		return -1;
	}

	if (0 != iResult_code)
	{
		DEBUG("Child fail! Exit code %d.\n", iResult_code);
		return -1;
	}

	return 0;
}

int tz_init_config(int argc, char * * argv,
	struct tz_config * pConfig)
{
	CONSTRUCT_FOTA_CLIENT_OPTION(client_option,
		(&pConfig->client_info));

	RETURN_IF_NULL(pConfig, -1);

	memset(pConfig, '\0', sizeof (struct tz_config));

	if (tz_parse_cmd(pConfig, argc, argv) < 0)
	{
		DEBUG("parse command line fail.\n");
		return -1;
	}

	if (pConfig->iHelp == 1)
	{
		tz_help(argv[0]);
		exit(EXIT_SUCCESS);
	}

	iVerbose = pConfig->iVerbose;

	if (!pConfig->pAction)
	{
		DEBUG("You must set the action to be performed.\n");
		return -1;
	}

	if (!pConfig->pCacert)
	{
		DEBUG("CA certificate is not set, use default file: %s.\n",
			CA_CERT_FILE);
		pConfig->pCacert = CA_CERT_FILE;
	}
	pCacert = pConfig->pCacert;

	if (0 != access(pConfig->pCacert, F_OK))
	{
		DEBUG("access fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	if (pConfig->pConfig_file)
	{
		if (0 != access(pConfig->pConfig_file, F_OK))
		{
			DEBUG("access fail, %s, errno=%d.\n",
				strerror(errno), errno);
			return -1;
		}
	}
	else
	{
		if (tz_auto_generate_config_file(TZ_CONFIG_FILE) < 0)
		{
			DEBUG("tz_auto_generate_config_file fail!\n");
			unlink(TZ_CONFIG_FILE);
			return -1;
		}

		pConfig->pConfig_file = TZ_CONFIG_FILE;
	}
	

	if (fota_parse_config_file(pConfig->pConfig_file, client_option) < 0)
	{
		DEBUG("fota_parse_config_file fail.\n");
		unlink(pConfig->pConfig_file);
		return -1;
	}

	unlink(pConfig->pConfig_file);

	if (pConfig->iDaemonize && daemon(0, pConfig->iEnable_stdio) < 0)
	{
		DEBUG("daemon fail.\n");
		return -1;
	}

	return 0;
}

int tz_free_config(struct tz_config * pConfig)
{
	RETURN_IF_NULL(pConfig, -1);

	free_device_info(&pConfig->client_info);

	return 0;
}

