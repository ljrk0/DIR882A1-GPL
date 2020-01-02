
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/socket.h>
#include <netdb.h>

#include <fota_debug.h>
#include <fota_common.h>

#define MAX_CHECK_NETWORK_RETRY_INTERVAL 300

int execute_action(const char * pFile_name, char * const argv[],
					int * pExit_code)
{
	pid_t pid;
	int iChild_status = 0;

	RETURN_IF_NULL(pFile_name, -1);
	RETURN_IF_NULL(argv, -1);

	pid = vfork();

	if (0 == pid)
	{
		execvp(pFile_name, argv);

		DEBUG("execvp fail, %s, errno=%d.\n", strerror(errno), errno);

		exit(EXIT_FAILURE);
	}
	else if (pid > 0)
	{
		if (waitpid(pid, &iChild_status, 0) < 0)
		{
			DEBUG("waitpid fail, %s, errno=%d.\n", strerror(errno), errno);
			return -1;
		}
		else
		{
			if (WIFEXITED(iChild_status))
			{
				if (pExit_code)
				{
					*pExit_code = WEXITSTATUS(iChild_status);
				}
			}
			else
			{
				DEBUG("Child didn't terminated normally!\n");
				return -1;
			}
		}
	}
	else
	{
		DEBUG("vfork fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	return 0;
}

int fota_output_to_file(const char * pFile_name,
						const void * pAddr,
						size_t iLen)
{
	int iRet = 0;
	int iResult_code = -1;
	FILE * fp = NULL;

	RETURN_IF_NULL(pFile_name, -1);
	RETURN_IF_NULL(pAddr, -1);
	RETURN_IF_EQUAL(iLen, 0, -1);

	fp = fopen(pFile_name, "w");
	if (!fp)
	{
		DEBUG("fopen fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	iRet = fwrite(pAddr, iLen, 1, fp);
	if (iRet != 1)
	{
		DEBUG("fwrite fail, %s, errno=%d.\n", strerror(errno), errno);
		if (ferror(fp))
		{
			DEBUG("Write %s encountered problems.\n", pFile_name);
		}
		goto done;
	}

	iResult_code = 0;

done:

	if (fp && fclose(fp) < 0)
	{
		DEBUG("fclose fail, %s, errno=%d.\n", strerror(errno), errno);
	}

	return iResult_code;
}

int fota_get_file_content(const char * pFile_name,
							void * * pBuf_addr,
							size_t * iLen)
{
	int iResult_code = -1;
	long iFile_offset = 0;
	FILE * fp = NULL;

	RETURN_IF_NULL(pFile_name, -1);
	RETURN_IF_NULL(pBuf_addr, -1);

	fp = fopen(pFile_name, "r");
	if (!fp)
	{
		DEBUG("fopen fail, %s, errno=%d.\n",
			strerror(errno), errno);
		return -1;
	}

	if (fseek(fp, 0, SEEK_END) < 0)
	{
		DEBUG("fseek fail, %s, errno=%d.\n",
			strerror(errno), errno);
		goto done;
	}

	iFile_offset = ftell(fp);
	if (iFile_offset < 0)
	{
		DEBUG("ftell fail, %s, errno=%d.\n",
			strerror(errno), errno);
		goto done;
	}
	else if (0 == iFile_offset)
	{
		DEBUG("The content of %s is empty.\n", pFile_name);
		goto done;
	}

	if (iLen)
	{
		*iLen = iFile_offset;
	}

	if (fseek(fp, 0, SEEK_SET) < 0)
	{
		DEBUG("fseek fail, %s, errno=%d.\n",
			strerror(errno), errno);
		goto done;
	}

	*pBuf_addr = calloc(1, iFile_offset + 1);
	if (!*pBuf_addr)
	{
		DEBUG("calloc fail, %s, errno=%d.\n",
			strerror(errno), errno);
		goto done;
	}

	if (1 != fread(*pBuf_addr, iFile_offset, 1, fp))
	{
		DEBUG("fread fail, %s, errno=%d.\n", strerror(errno), errno);
		if (ferror(fp))
		{
			DEBUG("Read %s encountered problems.\n", pFile_name);
		}
		goto done;
	}

	iResult_code = 0;

done:

	if (fp && fclose(fp) < 0)
	{
		DEBUG("fclose fail, %s, errno=%d.\n", strerror(errno), errno);
	}

	return iResult_code;
}

int execute_pause(int iSeconds)
{
	struct timespec ts = {0, 0};

	if (iSeconds <= 0)
	{
		DEBUG("Invalid parameter, the number of seconds is negative: %d!\n",
			iSeconds);
		return -1;
	}

	ts.tv_sec = iSeconds;

	while (nanosleep(&ts, &ts) < 0 && errno == EINTR);

	return 0;
}

int dns_query(const char * pName, int iFamily)
{
	struct addrinfo hints;
	struct addrinfo * pHints = NULL;
	struct addrinfo * pResults = NULL;
	int iRet = 0;

	RETURN_IF_NULL(pName, -1);

	if (iFamily >= 0)
	{
		memset(&hints, 0, sizeof (hints));
		hints.ai_family = iFamily;

		pHints = &hints;
	}

	iRet = getaddrinfo(pName, NULL, pHints, &pResults);
	if (iRet != 0)
	{
		DEBUG("getaddrinfo fail: %s, errno=%d\n", gai_strerror(iRet), iRet);
		return -1;
	}

	freeaddrinfo(pResults);
	return 0;
}

int check_network_status(const char * pDomain, int iFamily)
{
	RETURN_IF_NULL(pDomain, -1);

	if (0 == dns_query(pDomain, iFamily))
	{
		DEBUG("The network is ready.\n");
		return 0;
	}

	DEBUG("The network is not ready yet.\n");

	return -1;
}

int wait_network_accessible(const char * pDomain, int iFamily)
{
	int iCheck_network_retry_interval = 1;

	RETURN_IF_NULL(pDomain, -1);

	while (0 != dns_query(pDomain, iFamily))
	{
		iCheck_network_retry_interval = (2 * iCheck_network_retry_interval)
			% MAX_CHECK_NETWORK_RETRY_INTERVAL;
		DEBUG("Check network fail! Wait %d seconds and try again.\n",
				iCheck_network_retry_interval);

		execute_pause(iCheck_network_retry_interval);
	}

	DEBUG("The network is ready.\n");

	return 0;
}

