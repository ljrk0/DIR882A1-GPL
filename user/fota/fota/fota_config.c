
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <syslog.h>
#include <getopt.h>

#include <fota_common.h>
#include <fota_version.h>
#include "fota.h"

#define OPTION_STRING "a:c:Dhp:v"

static void fota_help(const char * pApp_name)
{
	RETURN_IF_NULL(pApp_name, );

	printf( "%s [options]\n"
			"	-a, --action=<action> \t The action will be execute, include:\n"
										"\t\t\t\tauto/query/download/notify.\n"
			"	-c, --config-file=<file> The filename of configuration file.\n"
			"	    --cacert=<file> \t CA certificate to verify fota server, "
						"default: %s.\n"
			"	-D, --daemonize \t Daemonize after startup.\n"
			"	    --debug\n"
					"\t\t\tNot redirect stdio,stdout,stderr when in daemon mode.\n"
			"	    --fw-encrypt \t Firmware image enable encryption.\n"
			"	    --fw-path=<path> \t Save firmware into path, default: %s.\n"
			"	    --fw-url=<url> \t Firmware url in server.\n"
			"	-h, --help \t\t Show this help.\n"
			"	-p, --pid-file=<FILE> \t Save pid into FILE.\n"
			"	    --progress-type=<percent/time>\n"
					"\t\tThe type to update the progress, default: percent.\n"
			"	    --progress-percent=<percent>\n"
					"\t\tUpdate the progress by per 'percent' percentage, "
					"range: 1-100, default: 1.\n"
			"	    --progress-time=<seconds>\n"
					"\t\tUpdate the progress by per 'seconds' seconds, default: 2.\n"
			"	    --progress-file=<FILE> \t Save real-time progress into FILE.\n"
			"	-v, --verbose \t Make the operation more talkative.\n"
			"	    --version \t Show version number and quit.\n",
			pApp_name,
			CA_CERT_FILE,
			TMP_FW
			);
}

static int fota_parse_cmd(struct fota_config * c, int argc, char * * argv)
{
	int o = -1;
	int iLong_index = 0;
	int iPercent = 0;
	int iInterval = 0;

	static const struct option long_options[] =
	{
		{"action", required_argument, NULL, 'a' },
		{"config-file", required_argument, NULL, 'c' },
		{"cacert", required_argument, NULL, 'c' },
		{"daemonize", no_argument, NULL, 'D' },
		{"debug", no_argument, NULL, 'D' },
		{"fw-encrypt", no_argument, NULL, 'f' },
		{"fw-path", required_argument, NULL, 'f' },
		{"fw-url", required_argument, NULL, 'f' },
		{"help", no_argument, NULL, 'h' },
		{"pid-file", required_argument, NULL, 'p' },
		{"progress-type", required_argument, NULL, 'p' },
		{"progress-percent", required_argument, NULL, 'p' },
		{"progress-time", required_argument, NULL, 'p' },
		{"progress-file", required_argument, NULL, 'p' },
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
			case 'f':
				if (!strcmp(long_options[iLong_index].name,
					"fw-encrypt")
				)
				{
					c->iFw_encrypt_flag = 1;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"fw-path")
				)
				{
					c->pFw_path = optarg;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"fw-url"))
				{
					c->pFw_url = optarg;
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
				else if (!strcmp(long_options[iLong_index].name,
					"progress-type"))
				{
					if (!strcmp(optarg, "percent"))
					{
						c->progress_config.iProgress_type =
							PROGRESS_TYPE_PERCENT;
					}
					else if (!strcmp(optarg, "time"))
					{
						c->progress_config.iProgress_type =
							PROGRESS_TYPE_TIME;
					}
					else
					{
						DEBUG("Unknown progress type: %s.\n", optarg);
						return -1;
					}
				}
				else if (!strcmp(long_options[iLong_index].name,
					"progress-percent"))
				{
					iPercent = atoi(optarg);
					if (iPercent < 1 || iPercent > 100)
					{
						DEBUG("Invalid percent value: %s, range: 1-100.\n", optarg);
						return -1;
					}
					c->progress_config.iProgress_percent = iPercent;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"progress-time"))
				{
					iInterval = atoi(optarg);
					if (iInterval < 1)
					{
						DEBUG("Invalid time value: %s, must be greater than 0.\n", optarg);
						return -1;
					}
					c->progress_config.iProgress_time = iInterval;
				}
				else if (!strcmp(long_options[iLong_index].name,
					"progress-file"))
				{
					c->progress_config.pProgress_file = optarg;
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
					printf("fota client version: %s.\n", FOTA_VERSION);
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

static int fota_auto_generate_config_file(
									const char * pFile_name,
									int iFw_encrypt_flag)
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

	argv[3] = iFw_encrypt_flag ? "fw-encrypt" : "fw-unencrypt";

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

int fota_init_config(int argc, char * * argv,
	struct fota_config * pConfig)
{
	CONSTRUCT_FOTA_CLIENT_OPTION(client_option,
		(&pConfig->client_info));
	CONSTRUCT_FOTA_UPGRADE_OPTION(upgrade_option,
		(&pConfig->upgrade_config));
	struct fota_progress_config * pProgress_config = NULL;

	RETURN_IF_NULL(pConfig, -1);

	memset(pConfig, '\0', sizeof (struct fota_config));

	if (fota_parse_cmd(pConfig, argc, argv) < 0)
	{
		DEBUG("parse command line fail.\n");
		return -1;
	}

	if (pConfig->iHelp == 1)
	{
		fota_help(argv[0]);
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

	if (1 == pConfig->iFw_encrypt_flag)
	{
		DEBUG("firmware encryption flag is on.\n");
	}
	else
	{
		DEBUG("firmware encryption flag is off.\n");
	}

	if (!pConfig->pFw_path)
	{
		if (!strcmp(pConfig->pAction, "auto")
			|| !strcmp(pConfig->pAction, "download"))
		{
			DEBUG("Local firmware path is not set, use default path: %s.\n",
				TMP_FW);
		}

		pConfig->pFw_path = TMP_FW;
	}

	pProgress_config = &pConfig->progress_config;
	if (pProgress_config->pProgress_file)
	{
		if (0 == pProgress_config->iProgress_type)
		{
			DEBUG("Progress type is not set, use default type: percent.\n");
			pProgress_config->iProgress_type = PROGRESS_TYPE_PERCENT;
		}

		if (PROGRESS_TYPE_PERCENT == pProgress_config->iProgress_type)
		{
			if (0 == pProgress_config->iProgress_percent)
			{
				DEBUG("Progress percent is not set, use default value: 1.\n");
				pProgress_config->iProgress_percent = 1;
			}
		}
		else if (PROGRESS_TYPE_TIME == pProgress_config->iProgress_type)
		{
			if (0 == pProgress_config->iProgress_time)
			{
				DEBUG("Update progress interval is not set, "
					"use default value: 2.\n");
				pProgress_config->iProgress_time = 2;
			}
		}
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
		if (fota_auto_generate_config_file(FOTA_CONFIG_FILE,
			pConfig->iFw_encrypt_flag) < 0)
		{
			DEBUG("fota_auto_generate_config_file fail!\n");
			unlink(FOTA_CONFIG_FILE);
			return -1;
		}

		pConfig->pConfig_file = FOTA_CONFIG_FILE;
	}
	

	if (fota_parse_config_file(pConfig->pConfig_file, client_option) < 0)
	{
		DEBUG("fota_parse_config_file fail.\n");
		unlink(pConfig->pConfig_file);
		return -1;
	}

	if (fota_parse_config_file(pConfig->pConfig_file, upgrade_option) < 0)
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

int fota_free_config(struct fota_config * pConfig)
{
	RETURN_IF_NULL(pConfig, -1);

	free_device_info(&pConfig->client_info);

	return 0;
}

