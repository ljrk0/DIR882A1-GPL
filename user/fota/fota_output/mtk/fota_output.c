
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>

#include <nvram.h>
#include <fota_debug.h>
#include <fota_common.h>

static int handle_register_info(void)
{
	char * pAccess_token = NULL;
	char * pRefresh_token = NULL;
	char * pRegister_status = NULL;
	char * pAccess_token_encrypt = NULL;
	char * pRefresh_token_encrypt = NULL;
	int iRet = 0;
	int iResult_code = -1;

	pAccess_token = getenv("FOTA_ACCESS_TOKEN");
	pRefresh_token = getenv("FOTA_REFRESH_TOKEN");
	pRegister_status = getenv("FOTA_REGISTER_STATUS");

	if (pAccess_token)
	{
		iRet = aes_encrypt(FOTA_AES_KEY, pAccess_token,
			&pAccess_token_encrypt);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);

		/* add by zengqinghuang 20181022
		*	非产测配置应该写入到config分区而非config2，否则容易导致config2配置丢失
		*/
		iRet = cfg_set("fota_access_token",
			pAccess_token_encrypt);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}

	if (pRefresh_token)
	{
		iRet = aes_encrypt(FOTA_AES_KEY, pRefresh_token,
			&pRefresh_token_encrypt);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);

		iRet = cfg_set("fota_refresh_token",
			pRefresh_token_encrypt);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}

	if (pRegister_status)
	{
		iRet = cfg_set("fota_register_status",
			pRegister_status);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}

	iRet = cfg_save();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pAccess_token_encrypt);
	FREE_POINTER(pRefresh_token_encrypt);

	return iResult_code;
}

static int handle_action_auto(void)
{
	char * pUpdate_done_notify = NULL;
	char * pFota_action_result = NULL;
	int iRet = 0;
	int iResult_code = -1;

	iRet = handle_register_info();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	pUpdate_done_notify = getenv("FOTA_UPDATE_DONE_NOTIFY");
	GOTO_IF_NULL(pUpdate_done_notify, done);

	pFota_action_result = getenv("FOTA_ACTION_RESULT");
	GOTO_IF_NULL(pFota_action_result, done);

	if (0 != strcmp(pFota_action_result, "OK"))
	{
		DEBUG("The result of fota auto action is not ok.\n");
		goto done;
	}

	iRet = cfg_set_permanent("fota_update_done_notify",
		pUpdate_done_notify);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_save_permanent();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	return iResult_code;
}

static int handle_action_notify(void)
{
	char * pUpdate_done_notify = NULL;
	char * pFota_action_result = NULL;
	int iRet = 0;
	int iResult_code = -1;

	iRet = handle_register_info();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	pUpdate_done_notify = getenv("FOTA_UPDATE_DONE_NOTIFY");
	GOTO_IF_NULL(pUpdate_done_notify, done);

	pFota_action_result = getenv("FOTA_ACTION_RESULT");
	GOTO_IF_NULL(pFota_action_result, done);

	if (0 != strcmp(pFota_action_result, "OK"))
	{
		DEBUG("The result of fota notify action is not ok.\n");
		goto done;
	}

	iRet = cfg_set_permanent("fota_update_done_notify",
		pUpdate_done_notify);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_save_permanent();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	return iResult_code;
}

static int handle_action_query(void)
{
	char * pFw_version_major = NULL;
	char * pFw_version_minor = NULL;
	char * pFw_version_revision = NULL;
	char * pFw_url = NULL;
	char * pFw_date = NULL;
	char * pEnforced_version_exist = NULL;
	char * pFota_action_result = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int version_change = 0;
	char *old_enforced[16] = {0};
	strcpy(old_enforced,cfg_get("fota_enforced_version_exist"));
	
	iRet = handle_register_info();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	pFw_version_major = getenv("FOTA_MANUAL_FW_VERSION_MAJOR");
	GOTO_IF_NULL(pFw_version_major, done);

	pFw_version_minor = getenv("FOTA_MANUAL_FW_VERSION_MINOR");
	GOTO_IF_NULL(pFw_version_minor, done);

	pFw_version_revision = getenv("FOTA_MANUAL_FW_VERSION_REVISION");
	GOTO_IF_NULL(pFw_version_revision, done);

	pFw_url = getenv("FOTA_MANUAL_FW_URL");
	GOTO_IF_NULL(pFw_url, done);

	pFw_date = getenv("FOTA_MANUAL_FW_DATE");
	GOTO_IF_NULL(pFw_date, done);

	pEnforced_version_exist = getenv("FOTA_ENFORCED_VERSION_EXIST");

	pFota_action_result = getenv("FOTA_ACTION_RESULT");
	GOTO_IF_NULL(pFota_action_result, done);

	if (0 != strcmp(pFota_action_result, "OK"))
	{
		DEBUG("The result of fota query action is not ok.\n");
		goto done;
	}
	
	if(strcmp(pFw_version_major,cfg_get("fota_manual_fw_version_major") )!= 0 || strcmp(pFw_version_minor,cfg_get("fota_manual_fw_version_minor")) != 0)
	{
		iRet = cfg_set("fota_manual_fw_version_major",
		pFw_version_major);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);

		iRet = cfg_set("fota_manual_fw_version_minor",
		pFw_version_minor);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);

		version_change = 1;
	}
	

	iRet = cfg_set("fota_manual_fw_version_revision",
		pFw_version_revision);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set("fota_manual_fw_url", pFw_url);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set("fota_manual_fw_date", pFw_date);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);
	DEBUG("pEnforced_version_exist:%s\n",pEnforced_version_exist);
	
	if (pEnforced_version_exist)
	{
		
		iRet = cfg_set("fota_enforced_version_exist",pEnforced_version_exist);
		
		DEBUG("-----old enforce flag:%s,new flag:%s-----\n",old_enforced,pEnforced_version_exist);
		if(strcmp(old_enforced,pEnforced_version_exist) !=0 || version_change == 1)
		{
			/*flag change, restart mdns*/
			notify_rc("restart_mdns");
		}
		
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}
	else
	{
		iRet = cfg_set("fota_enforced_version_exist","0");
		if(strcmp(old_enforced,"0") !=0 || version_change == 1)
		{
			/*flag change, restart mdns*/
			notify_rc("restart_mdns");
		}
	}

	iResult_code = 0;

done:

	if (0 != iResult_code)
	{
		iRet += cfg_set("fota_manual_fw_version_major", "");
		iRet += cfg_set("fota_manual_fw_version_minor", "");
		iRet += cfg_set("fota_manual_fw_version_revision", "");
		iRet += cfg_set("fota_manual_fw_url", "");
	}

	return iResult_code;
}

static int handle_action_time_sync(void)
{
	char * pTime_sync_done = NULL;
	char * pFota_action_result = NULL;
	int iRet = 0;
	int iResult_code = -1;

	iRet = handle_register_info();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	pTime_sync_done = getenv("TIME_SYNC_DONE");
	GOTO_IF_NULL(pTime_sync_done, done);

	pFota_action_result = getenv("FOTA_ACTION_RESULT");
	GOTO_IF_NULL(pFota_action_result, done);

	if (0 != strcmp(pFota_action_result, "OK"))
	{
		DEBUG("The result of fota time sync action is not ok.\n");
		goto done;
	}

	iRet = cfg_set("time_sync_done", pTime_sync_done);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_save();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	return iResult_code;
}

int main(int argc, char * * argv)
{
	const char * pFota_action = NULL;
	const char * pFw_encrypt = NULL;
	int iFw_encrypt_flag = 0;
	int iRet = 0;
	int iResult_code = -1;

	openlog("FOTA_OUTPUT", LOG_PID, LOG_DAEMON);

	if (2 != argc)
	{
		DEBUG("Usage: %s <fw-encrypt/fw-unencrypt>.\n", argv[0]);
		goto done;
	}

	pFw_encrypt = argv[1];

	if (!strcmp(pFw_encrypt, "fw-encrypt"))
	{
		iFw_encrypt_flag = 1;
	}
	else if (!strcmp(pFw_encrypt, "fw-unencrypt"))
	{
		iFw_encrypt_flag = 0;
	}
	else
	{
		DEBUG("Unknown option: %s.\n", pFw_encrypt);
		goto done;
	}

	pFota_action = getenv("FOTA_ACTION");
	GOTO_IF_NULL(pFota_action, done);

	DEBUG("Now processing the result of ACTION: %s.\n", pFota_action);

	if (!strcmp(pFota_action, "auto") && 1 == iFw_encrypt_flag)
	{
		iRet = handle_action_auto();
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}
	else if (!strcmp(pFota_action, "notify") && 1 == iFw_encrypt_flag)
	{
		iRet = handle_action_notify();
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}
	else if (!strcmp(pFota_action, "query"))
	{
		iRet = handle_action_query();
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}
	else if (!strcmp(pFota_action, "time_sync"))
	{
		iRet = handle_action_time_sync();
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}
	else
	{
		DEBUG("Unknown action.\n");
	}

	DEBUG("Done.\n");

	iResult_code = 0;

done:

	closelog();

	return iResult_code;
}

