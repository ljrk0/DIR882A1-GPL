
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <sys/types.h>
#include <unistd.h>

#include <nvram.h>
#include <fota_debug.h>
#include <fota_common.h>

#include "../fota_credential.h"

#define FOTA_ACCESS_TOKEN_FILE "/tmp/fota_access_token"
#define FOTA_REFRESH_TOKEN_FILE "/tmp/fota_refresh_token"

struct fota_config_action
{
	const char * pAction_name;
	int iDebug;
	int (*action)(int, char * *);
};

static int fota_get_factory_mac(char * pMac, size_t iLen)
{
	unsigned char mac_addr[6] = { 0 };
	int iItem_num = 0;
	int iResult_code = -1;
	FILE * fp = NULL;

	RETURN_IF_NULL(pMac, -1);

	fp = popen("eth_mac r lan", "r");
	if (!fp)
	{
		DEBUG("popen fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	iItem_num = fscanf(fp,
		"%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX",
		&mac_addr[0], &mac_addr[1], &mac_addr[2],
		&mac_addr[3], &mac_addr[4], &mac_addr[5]);
	if (iItem_num != 6)
	{
		DEBUG("Get factory mac fail.\n");
		if (iItem_num < 0)
		{
			DEBUG("fscanf fail, %s, errno=%d.\n", strerror(errno), errno);
		}
		goto done;
	}
	else
	{
		if (mac_addr[0] == 0xff)
		{
			goto done;
		}

		snprintf(pMac, iLen,
			"%02hhX:%02hhX:%02hhX:%02hhX:%02hhX:%02hhX",
			mac_addr[0], mac_addr[1], mac_addr[2],
			mac_addr[3], mac_addr[4], mac_addr[5]);
	}

	iResult_code = 0;

done:

	if (pclose(fp) < 0)
	{
		DEBUG("pclose fail, %s, errno=%d.\n", strerror(errno), errno);
	}

	return iResult_code;
}

static int check_cfg_decipherable(char * pCfg_name)
{
	const char * pCfg_value = NULL;
	char * pDecrypt_value = NULL;
	int iRet = 0;

	RETURN_IF_NULL(pCfg_name, -1);

	/* add by zengqinghuang 20181022
	*   非产测配置应该写入到config分区而非config2，否则容易导致config2配置丢失
	*/
	if(strcmp(pCfg_name, "fota_access_token") == 0 || strcmp(pCfg_name, "fota_refresh_token") == 0)
		pCfg_value = cfg_get(pCfg_name);
	else
		pCfg_value = cfg_get_permanent(pCfg_name);
	if (pCfg_value && 0 != strcmp(pCfg_value, ""))
	{
		iRet = aes_decrypt(FOTA_AES_KEY, pCfg_value,
			&pDecrypt_value);
		if (iRet != 0)
		{
			DEBUG("Decrypt %s<%s> fail, clear it.\n",
				pCfg_name, pCfg_value);
			FREE_POINTER(pDecrypt_value);
			return -2;
		}
	}

	FREE_POINTER(pDecrypt_value);

	return 0;
}

static int fota_check_config(void)
{
	int iRet = 0;
	int iConfig_damage = 1;

	DEBUG("Now check fota configuration.\n");

	/* client id */
	iRet = check_cfg_decipherable("fota_client_id");
	GOTO_IF_EQUAL(iRet, -2, done);

	/* client secret */
	iRet = check_cfg_decipherable("fota_client_secret");
	GOTO_IF_EQUAL(iRet, -2, done);

	/* DID */
	iRet = check_cfg_decipherable("fota_device_id");
	GOTO_IF_EQUAL(iRet, -2, done);

	/* access token */
	iRet = check_cfg_decipherable("fota_access_token");
	GOTO_IF_EQUAL(iRet, -2, done);

	/* refresh token */
	iRet = check_cfg_decipherable("fota_refresh_token");
	GOTO_IF_EQUAL(iRet, -2, done);

	iConfig_damage = 0;

done:

	if (iConfig_damage)
	{
		iRet = cfg_set_permanent("fota_client_id", "");
		iRet = cfg_set_permanent("fota_client_secret", "");
		iRet = cfg_set_permanent("fota_device_id", "");
		iRet = cfg_set("fota_access_token", "");
		iRet = cfg_set("fota_refresh_token", "");

		iRet = cfg_save();
	}

	return 0;
}

static int fota_init_device_id(char * pKey,
							const char * pData,
							char * * pDevice_id)
{
	char * pDevice_id_unencrypt = NULL;
	int iRet = 0;
	int iResult_code = -1;

	RETURN_IF_NULL(pKey, -1);
	RETURN_IF_NULL(pData, -1);
	RETURN_IF_NULL(pDevice_id, -1);

	iRet = aes_encrypt(pKey, pData, &pDevice_id_unencrypt);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = aes_encrypt(FOTA_AES_KEY, pDevice_id_unencrypt, pDevice_id);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pDevice_id_unencrypt);

	return iResult_code;
}

static int fota_get_credential(char * pCfg_name, char * * pCfg_value)
{
	const char * pCfg_encrypt = NULL;
	int iRet = 0;

	RETURN_IF_NULL(pCfg_name, -1);
	RETURN_IF_NULL(pCfg_value, -1);

	if(strcmp(pCfg_name, "fota_access_token") == 0 || strcmp(pCfg_name, "fota_refresh_token") == 0)
		pCfg_encrypt = cfg_get(pCfg_name);
	else
		pCfg_encrypt = cfg_get_permanent(pCfg_name);
	if (!pCfg_encrypt || !strcmp(pCfg_encrypt, ""))
	{
		DEBUG("%s is not found.\n", pCfg_name);
		return -1;
	}
	else
	{
		iRet = aes_decrypt(FOTA_AES_KEY, pCfg_encrypt,
			pCfg_value);
		if (iRet < 0)
		{
			DEBUG("Decrypt %s fail.\n", pCfg_name);
			return -1;
		}
	}

	return 0;
}

static int fota_set_credential(char * pCfg_name,
						const char * pCfg_value)
{
	char * pCfg_encrypt = NULL;
	int iRet = 0;
	int iResult_code = -1;

	RETURN_IF_NULL(pCfg_name, -1);
	RETURN_IF_NULL(pCfg_value, -1);

	iRet = aes_encrypt(FOTA_AES_KEY, pCfg_value,
		&pCfg_encrypt);
	if (iRet < 0)
	{
		DEBUG("Encrypt %s<%s> fail.\n", pCfg_name, pCfg_value);
		goto done;
	}

	iRet = cfg_set_permanent(pCfg_name, pCfg_encrypt);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pCfg_encrypt);

	return iResult_code;
}

static int fota_init_client_info(void)
{
	char sMac[32] = { 0 };
	char * pClient_id = NULL;
	char * pClient_secret = NULL;
	char * pDevice_id = NULL;
	char * pDevice_id_encrypt = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iConfig_change = 0;

	iRet = fota_get_factory_mac(sMac, sizeof (sMac));
	if (iRet < 0)
	{
		DEBUG("Get factory mac fail.\n");
		return -1;
	}

	/* client id */
	iRet = fota_get_credential("fota_client_id", &pClient_id);
	if (iRet != 0)
	{
		iRet = fota_set_credential("fota_client_id", FOTA_CLIENT_ID);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
		iConfig_change = 1;
	}

	/* client secret */
	iRet = fota_get_credential("fota_client_secret", &pClient_secret);
	if (iRet != 0)
	{
		FREE_POINTER(pClient_secret);
		pClient_secret = strdup(FOTA_CLIENT_SECRET);
		if (!pClient_secret)
		{
			DEBUG("strdup fail, %s, errno=%d.\n", strerror(errno), errno);
			goto done;
		}

		iRet = fota_set_credential("fota_client_secret", FOTA_CLIENT_SECRET);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
		iConfig_change = 1;
	}

	/* DID */
	iRet = fota_get_credential("fota_device_id", &pDevice_id);
	if (iRet != 0)
	{
		iRet = fota_init_device_id(pClient_secret, sMac,
			&pDevice_id_encrypt);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);

		iRet = cfg_set_permanent("fota_device_id",
			pDevice_id_encrypt);
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
		iConfig_change = 1;
	}

	if (iConfig_change)
	{
		iRet = cfg_save_permanent();
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}

	iResult_code = 0;

done:

	FREE_POINTER(pClient_id);
	FREE_POINTER(pClient_secret);
	FREE_POINTER(pDevice_id);
	FREE_POINTER(pDevice_id_encrypt);

	return iResult_code;
}

static int fota_get_upgrade_config(FILE * fp)
{
	RETURN_IF_NULL(fp, -1);

	fprintf(fp, "event-notification-enable=%d;\n",
		cfg_get_int("fota_event_notification_enable"));
	fprintf(fp, "auto-update-notification-enable=%d;\n",
		cfg_get_int("fota_auto_update_notification_enable"));
	fprintf(fp, "auto-query-enable=%d;\n",
		cfg_get_int("fota_auto_query_enable"));
	fprintf(fp, "auto-download-enable=%d;\n",
		cfg_get_int("fota_auto_download_enable"));
	fprintf(fp, "auto-update-enable=%d;\n",
		cfg_get_int("fota_auto_update_enable"));
	fprintf(fp, "beta-firmware-enable=%d;\n",
		cfg_get_int("fota_beta_firmware_enable"));

	return 0;
}

static int fota_get_fw_version(FILE * fp)
{
	char sMajor[8] = { 0 };
	char sMinor[8] = { 0 };
	char sRevision[8] = { 0 };
	const char * pFw_version = NULL;
	const char * pFw_internal_version = NULL;
	int iItem_num = 0;
	int iResult_code = -1;

	RETURN_IF_NULL(fp, -1);

	pFw_version = cfg_get("sw_ex_version");
	if (!strcmp(pFw_version, ""))
	{
		goto done;
	}

	iItem_num = sscanf(pFw_version + 1,
		"%7[^.\n].%7[^.\n]",
		sMajor, sMinor);
	if (iItem_num != 2)
	{
		DEBUG("Get fw version fail.\n");
		if (iItem_num < 0)
		{
			DEBUG("sscanf fail, %s, errno=%d.\n", strerror(errno), errno);
		}
		goto done;
	}

	fprintf(fp, "fw-major-version=%s;\n", sMajor);
	fprintf(fp, "fw-minor-version=%s;\n", sMinor);

	pFw_internal_version = cfg_get("sw_in_version");
	if (!strcmp(pFw_internal_version, ""))
	{
		iResult_code = 0;
		goto done;
	}

	iItem_num = sscanf(pFw_internal_version + 1,
		"%7[^.\n]", sRevision);
	if (iItem_num != 1)
	{
		DEBUG("Get fw internal version fail.\n");
		if (iItem_num < 0)
		{
			DEBUG("sscanf fail, %s, errno=%d.\n", strerror(errno), errno);
		}
	}
	else
	{
		fprintf(fp, "fw-revision-version=%s;\n", sRevision);
	}

	iResult_code = 0;

done:

	return iResult_code;
}

static int fota_get_device_info(FILE * fp)
{
	char sUser_name[32] = { 0 };
	char sFile_name[64] = { 0 };
	const char * pModel = NULL;
	const char * pLanguage = NULL;
	const char * pServer_domain = NULL;
	const char * pNtp_server = NULL;
	const char * pTZ_location = NULL;
	char * pClient_id = NULL;
	char * pClient_secret = NULL;
	char * pRefresh_token = NULL;
	char * pAccess_token = NULL;
	const char * pFw_variant = NULL;
	char * pDevice_id = NULL;
	int iRet = 0;
	int iResult_code = -1;
	pid_t pid;

	RETURN_IF_NULL(fp, -1);

	iRet = fota_get_factory_mac(sUser_name,
		sizeof (sUser_name));
	if (iRet < 0)
	{
		DEBUG("Get factory mac fail.\n");
		return -1;
	}
	fprintf(fp, "factory-mac=%s;\n", sUser_name);

	/* model name */
	pModel = cfg_get("model_name");
	if (!strcmp(pModel, ""))
	{
		DEBUG("Get model name fail.\n");
		return -1;
	}
	fprintf(fp, "model-name=%s;\n", pModel);

	/* language */
	pLanguage = cfg_get("fota_language");
	if (0 != strcmp(pLanguage, ""))
	{
		fprintf(fp, "language=%s;\n", pLanguage);
	}

	/* server domain */
	pServer_domain = cfg_get("fota_server_domain");
	if (!strcmp(pServer_domain, ""))
	{
		pServer_domain = "api.fota.dlink.com";
	}
	fprintf(fp, "server-domain=%s;\n", pServer_domain);

	/* ntp enable */
	fprintf(fp, "ntp-enable=%d;\n", cfg_get_int("ntpEnabled"));

	/* ntp server */
	pNtp_server = cfg_get("ntp_server");
	if (!strcmp(pNtp_server, ""))
	{
		pNtp_server = "ntp1.dlink.com";
	}
	fprintf(fp, "ntp-server=%s;\n", pNtp_server);

	/* TZ location */
	pTZ_location = cfg_get("time_TZ_location");
	if (0 != strcmp(pTZ_location, ""))
	{
		fprintf(fp, "TZ-location=%s;\n", pTZ_location);
	}

	/* client id */
	iRet = fota_get_credential("fota_client_id", &pClient_id);
	if (iRet != 0)
	{
		DEBUG("Get client id fail, terminate fota process.\n");
		goto done;
	}
	fprintf(fp, "client-id=%s;\n", pClient_id);

	/* client secret */
	iRet = fota_get_credential("fota_client_secret", &pClient_secret);
	if (iRet != 0)
	{
		DEBUG("Get client secret fail, terminate fota process.\n");
		goto done;
	}
	fprintf(fp, "client-secret=%s;\n", pClient_secret);

	pid = getpid();

	/* refresh token */
	iRet = fota_get_credential("fota_refresh_token", &pRefresh_token);
	if (0 == iRet)
	{
		snprintf(sFile_name, sizeof (sFile_name), "%s.%d",
			FOTA_REFRESH_TOKEN_FILE, (int)pid);

		iRet = fota_output_to_file(sFile_name,
			pRefresh_token, strlen(pRefresh_token));
		if (0 == iRet)
		{
			fprintf(fp, "refresh-token=%s;\n", sFile_name);
		}
	}

	/* access token */
	iRet = fota_get_credential("fota_access_token", &pAccess_token);
	if (0 == iRet)
	{
		snprintf(sFile_name, sizeof (sFile_name), "%s.%d",
			FOTA_ACCESS_TOKEN_FILE, (int)pid);

		iRet = fota_output_to_file(sFile_name,
			pAccess_token, strlen(pAccess_token));
		if (0 == iRet)
		{
			fprintf(fp, "access-token=%s;\n", sFile_name);
		}
	}

	/* fw variant */
	pFw_variant = cfg_get("fota_fw_variant");
	if (!strcmp(pFw_variant, ""))
	{
		pFw_variant = "Default";
	}
	fprintf(fp, "fw-variant=%s;\n", pFw_variant);

	/* DID */
	iRet = fota_get_credential("fota_device_id", &pDevice_id);
	if (iRet != 0)
	{
		DEBUG("Get device id fail, terminate fota process.\n");
		goto done;
	}
	fprintf(fp, "device-id=%s;\n", pDevice_id);

	/* fw version */
	iRet = fota_get_fw_version(fp);
	if (iRet < 0)
	{
		DEBUG("Get fw version fail.\n");
		goto done;
	}

	iResult_code = 0;

done:

	FREE_POINTER(pClient_id);
	FREE_POINTER(pClient_secret);
	FREE_POINTER(pRefresh_token);
	FREE_POINTER(pAccess_token);
	FREE_POINTER(pDevice_id);

	return iResult_code;
}

static int fota_get_device_info_2(FILE * fp)
{
	char sSeries_name[32] = { 0 };
	const char * pModel = NULL;
	char * pModel_end = NULL;
	const char * pHardware_version = NULL;
	const char * pFw_variant = NULL;
	const char * pLanguage = NULL;
	const char * pServer_domain = NULL;
	int iRet = 0;

	RETURN_IF_NULL(fp, -1);

	/* model name */
	pModel = cfg_get("model_name");
	if (!strcmp(pModel, ""))
	{
		DEBUG("Get model name fail.\n");
		return -1;
	}
	fprintf(fp, "model-name=%s;\n", pModel);

	/* series name */
	snprintf(sSeries_name, sizeof (sSeries_name), "%s",
		pModel);
	pModel_end = strstr(sSeries_name, "-");
	if (!pModel_end)
	{
		DEBUG("Get series name fail.\n");
		return -1;
	}
	*pModel_end = '\0';
	fprintf(fp, "series-name=%s;\n", sSeries_name);

	/* hardware version */
	pHardware_version = cfg_get("hw_version");
	if (!strcmp(pHardware_version, ""))
	{
		DEBUG("Get hardware version fail.\n");
		return -1;
	}
	fprintf(fp, "hardware-version=%cx;\n", pHardware_version[0]);

	/* fw variant */
	pFw_variant = cfg_get("fota_fw_variant");
	if (!strcmp(pFw_variant, ""))
	{
		pFw_variant = "Default";
	}
	fprintf(fp, "fw-variant=%s;\n", pFw_variant);

	/* language */
	pLanguage = cfg_get("fota_language");
	if (0 != strcmp(pLanguage, ""))
	{
		fprintf(fp, "language=%s;\n", pLanguage);
	}

	/* server domain */
	pServer_domain = cfg_get("fota_server_domain");
	if (!strcmp(pServer_domain, ""))
	{
		pServer_domain = "api.fota.dlink.com";
	}
	fprintf(fp, "server-domain=%s;\n", pServer_domain);

	/* fw version */
	iRet = fota_get_fw_version(fp);
	if (iRet < 0)
	{
		DEBUG("Get fw version fail.\n");
		return -1;
	}

	return 0;
}

static int fota_generate_config_file(const char * pFile_name,
									int iFw_encrypt_flag)
{
	int iResult_code = -1;
	FILE * fp = NULL;

	RETURN_IF_NULL(pFile_name, -1);

	if (1 == iFw_encrypt_flag)
	{
		if (fota_init_client_info() < 0)
		{
			DEBUG("fota_init_client_info fail.\n");
			return -1;
		}
	}

	fp = fopen(pFile_name, "w");
	if (!fp)
	{
		DEBUG("fopen fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	if (fota_get_upgrade_config(fp) < 0)
	{
		DEBUG("fota_get_upgrade_config fail.\n");
		goto done;
	}

	if (1 == iFw_encrypt_flag)
	{
		if (fota_get_device_info(fp) < 0)
		{
			DEBUG("fota_get_device_info fail.\n");
			goto done;
		}
	}
	else
	{
		if (fota_get_device_info_2(fp) < 0)
		{
			DEBUG("fota_get_device_info_2 fail.\n");
			goto done;
		}
	}

	if (ferror(fp))
	{
		DEBUG("Write %s encountered problems, %s, errno=%d.\n",
			pFile_name, strerror(errno), errno);
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

static int fota_generate_device_id(void)
{
	char sMac[32] = { 0 };
	char * pClient_secret = NULL;
	char * pDevice_id_encrypt = NULL;
	int iRet = 0;
	int iResult_code = -1;

	iRet = fota_get_factory_mac(sMac, sizeof (sMac));
	if (iRet < 0)
	{
		DEBUG("Get factory mac fail.\n");
		return -1;
	}

	iRet = fota_get_credential("fota_client_secret", &pClient_secret);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = fota_init_device_id(pClient_secret, sMac,
		&pDevice_id_encrypt);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set_permanent("fota_device_id",
		pDevice_id_encrypt);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_save_permanent();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pClient_secret);
	FREE_POINTER(pDevice_id_encrypt);

	return iResult_code;
}

static int action_config(int argc, char * * argv)
{
	const char * pConfig_file = NULL;
	const char * pFw_encrypt = NULL;
	int iFw_encrypt_flag = 0;
	int iRet = 0;

	if (3 != argc)
	{
		return -1;
	}

	pConfig_file = argv[1];
	pFw_encrypt = argv[2];

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
		return -1;
	}

	DEBUG("Now start generating the configuration file.\n");

	iRet = fota_generate_config_file(pConfig_file, iFw_encrypt_flag);
	if (iRet < 0)
	{
		return -1;
	}

	return 0;
}

static int action_encrypt(int argc, char * * argv)
{
	const char * pData = NULL;
	char * pEncrypted_data = NULL;
	const char * pFile_name = NULL;
	int iRet = 0;
	int iResult_code = -1;

	GOTO_IF_NOT_EQUAL(argc, 3, done);

	pData = argv[1];
	pFile_name = argv[2];

	iRet = aes_encrypt(FOTA_AES_KEY, pData,
		&pEncrypted_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = fota_output_to_file(pFile_name, pEncrypted_data,
		strlen(pEncrypted_data));
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pEncrypted_data);

	return iResult_code;
}

static int action_decrypt(int argc, char * * argv)
{
	const char * pData = NULL;
	char * pDecrypted_data = NULL;
	const char * pFile_name = NULL;
	int iRet = 0;
	int iResult_code = -1;

	GOTO_IF_NOT_EQUAL(argc, 3, done);

	pData = argv[1];
	pFile_name = argv[2];

	iRet = aes_decrypt(FOTA_AES_KEY, pData,
		&pDecrypted_data);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = fota_output_to_file(pFile_name, pDecrypted_data,
		strlen(pDecrypted_data));
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pDecrypted_data);

	return iResult_code;
}

static int action_get(int argc, char * * argv)
{
	const char * pParam_name = NULL;
	const char * pFile_name = NULL;
	char * pCfg_name = NULL;
	char * pCfg_value = NULL;
	int iRet = 0;
	int iResult_code = -1;

	GOTO_IF_NOT_EQUAL(argc, 3, done);

	pParam_name = argv[1];
	pFile_name = argv[2];

	if (!strcmp(pParam_name, "client_id"))
	{
		pCfg_name = "fota_client_id";
	}
	else if (!strcmp(pParam_name, "client_secret"))
	{
		pCfg_name = "fota_client_secret";
	}
	else if (!strcmp(pParam_name, "device_id"))
	{
		pCfg_name = "fota_device_id";
	}
	else
	{
		DEBUG("Unknown param: %s.\n", pParam_name);
		goto done;
	}

	iRet = fota_get_credential(pCfg_name, &pCfg_value);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = fota_output_to_file(pFile_name, pCfg_value,
		strlen(pCfg_value));
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pCfg_value);

	return iResult_code;
}

static int action_set(int argc, char * * argv)
{
	const char * pParam_name = NULL;
	char * pCfg_name = NULL;
	char * pCfg_value = NULL;
	int iRet = 0;
	int iResult_code = -1;

	GOTO_IF_NOT_EQUAL(argc, 3, done);

	pParam_name = argv[1];
	pCfg_value = argv[2];

	if (!strcmp(pParam_name, "client_id"))
	{
		pCfg_name = "fota_client_id";
	}
	else if (!strcmp(pParam_name, "client_secret"))
	{
		pCfg_name = "fota_client_secret";
	}
	else
	{
		DEBUG("Unknown param: %s.\n", pParam_name);
		goto done;
	}

	iRet = fota_set_credential(pCfg_name, pCfg_value);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_save_permanent();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	return iResult_code;
}

static int action_generate(int argc, char * * argv)
{
	const char * pParam_name = NULL;
	int iRet = 0;
	int iResult_code = -1;

	GOTO_IF_NOT_EQUAL(argc, 2, done);

	pParam_name = argv[1];

	if (!strcmp(pParam_name, "device_id"))
	{
		iRet = fota_generate_device_id();
		GOTO_IF_NOT_EQUAL(iRet, 0, done);
	}
	else
	{
		DEBUG("Unknown param: %s.\n", pParam_name);
		goto done;
	}

	iResult_code = 0;

done:

	return iResult_code;
}


static int action_init_fota(int argc, char * * argv)
{
	int iRet = 0;
	int iResult_code = -1;

	fota_check_config();

	iRet = fota_init_client_info();
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set("fota_enforced_version_exist", "0");
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set("time_sync_done", "0");
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set("fota_manual_fw_version_major", "");
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set("fota_manual_fw_version_minor", "");
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set("fota_manual_fw_version_revision", "");
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set("fota_manual_fw_url", "");
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = cfg_set("fota_manual_fw_date", "");
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	return iResult_code;
}

struct fota_config_action config_action[] =
{
	{"config", 1, action_config},
	{"encrypt", 1, action_encrypt},
	{"decrypt", 1, action_decrypt},
	{"get", 0, action_get},
	{"set", 0, action_set},
	{"generate", 0, action_generate},
	{"init", 1, action_init_fota},
	{NULL}
};

int main(int argc, char * * argv)
{
	const char * pAction = NULL;
	int iRet = 0;
	int iResult_code = -1;
	int iIndex = 0;

	openlog("FOTA_CONFIG", LOG_PID, LOG_DAEMON);

	if (4 != argc && 3 != argc && 2 != argc)
	{
		printf("Usage: %s config <config file name>"
			" <fw-encrypt/fw-unencrypt>\n", argv[0]);
		printf("Usage: %s encrypt/decrypt <data>"
			" <output file name>\n", argv[0]);
		printf("Usage: %s get <client_id/client_secret/device_id>"
			" <output file name>\n", argv[0]);
		printf("Usage: %s set <client_id/client_secret>"
			" <value>\n", argv[0]);
		printf("Usage: %s generate device_id\n", argv[0]);
		printf("Usage: %s init\n", argv[0]);
		goto done;
	}

	pAction = argv[1];
	++argv;
	--argc;

	while (config_action[iIndex].pAction_name)
	{
		if (!strcmp(config_action[iIndex].pAction_name, pAction))
		{
			break;
		}

		++iIndex;
	}

	GOTO_IF_NULL(config_action[iIndex].pAction_name, done);
	GOTO_IF_NULL(config_action[iIndex].action, done);

	if (config_action[iIndex].iDebug)
	{
		DEBUG("Execute config ACTION: %s.\n", pAction);
	}

	iRet = config_action[iIndex].action(argc, argv);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	if (config_action[iIndex].iDebug)
	{
		DEBUG("Done.\n");
	}

	iResult_code = 0;

done:

	closelog();

	return iResult_code;
}

