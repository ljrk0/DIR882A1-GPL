
/*
*/

#ifndef __FOTA_API_H__
#define __FOTA_API_H__

#define REGISTER_URI "/oauth2/v1/token"
#define REFRESH_TOKEN_URI "/oauth2/v1/token"
#define UPDATE_DEVICE_INFO_URI "/device/v1/me"

#define FOTA_CONFIG_TOOL_PATH "/bin/fota_config"
#define FOTA_OUTPUT_TOOL_PATH "/bin/fota_output"

#define CA_CERT_FILE "/etc_ro/cacert.pem"

#define UNREGISTERED 0
#define REGISTERED 1

extern int iVerbose;
extern char * pCacert;

enum fota_error_code
{
	CODE_INVALID_REFRESH_TOKEN = 101,
	CODE_INVALID_ACCESS_TOKEN = 111,
	CODE_MAX
};

enum fota_request_type
{
	TYPE_REFRESH_TOKEN = 1,
	TYPE_REGISTER = 2,
	TYPE_MAX
};

struct fota_message
{
	const char * pMsg_code;
	const char * pMsg_message;
	const char * pMsg_description;
};

struct DID
{
	char * pDevice_id;
};

struct version
{
	char sMajor[8];
	char sMinor[8];
	char sRevision[8];
};

struct fota_client_info
{
	char sModel_name[32];
	char sSeries_name[32];
	char sHardware_version[32];
	char sGrant_type[32];
	char sUser_name[32];
	char sLanguage[32];
	char sServer_domain[128];
	char sNtp_server[128];
	char sTZ_location[128];
	char * pClient_id;
	char * pClient_secret;
	char * pRefresh_token;
	char * pAccess_token;
	char * pFw_variant;
	int iNtp_enable;
	long iLifetime_of_access_token;
	long iReference_time;
	struct DID password;
	struct version fw_version;
};

/*
*/
struct fota_client_info_2
{
	char sSeries_name[32];
	char sModel_name[32];
	char sHardware_version[32];
	char sFw_variant[32];
	char sLanguage[32];
	char sServer_domain[128];
	struct version fw_version;
};

enum http_chunk_type
{
	/* save http data to dynamic memory */
	CHUNK_TYPE_MEM = 1,
	/* save http data to file */
	CHUNK_TYPE_FILE
};

struct http_chunk
{
	enum http_chunk_type iType;
	int iStatus_code;
	size_t iSize;
	void * pData;
};

enum fota_option_type
{
	OPTION_TYPE_STRING,
	OPTION_TYPE_INT,
	OPTION_TYPE_STRING2,
	OPTION_TYPE_FILE,
	OPTION_TYPE_MAX
};

struct fota_option
{
	const char * pName;
	enum fota_option_type iType;
	void * pAddr;
	void * * pAddr2;
	size_t iBuf_len;
};

#define CONSTRUCT_FOTA_CLIENT_OPTION(config_option, pClient_config) \
struct fota_option config_option[] = \
{ \
	/* configuration of fota client */ \
\
	{"factory-mac", OPTION_TYPE_STRING, \
		pClient_config->sUser_name, \
		NULL, sizeof (pClient_config->sUser_name)}, \
	{"model-name", OPTION_TYPE_STRING, \
		pClient_config->sModel_name, \
		NULL, sizeof (pClient_config->sModel_name)}, \
	{"language", OPTION_TYPE_STRING, \
		pClient_config->sLanguage, \
		NULL, sizeof (pClient_config->sLanguage)}, \
	{"server-domain", OPTION_TYPE_STRING, \
		pClient_config->sServer_domain, \
		NULL, sizeof (pClient_config->sServer_domain)}, \
	{"ntp-enable", OPTION_TYPE_INT, \
		&pClient_config->iNtp_enable, \
		NULL}, \
	{"ntp-server", OPTION_TYPE_STRING, \
		pClient_config->sNtp_server, \
		NULL, sizeof (pClient_config->sNtp_server)}, \
	{"TZ-location", OPTION_TYPE_STRING, \
		pClient_config->sTZ_location, \
		NULL, sizeof (pClient_config->sTZ_location)}, \
	{"client-id", OPTION_TYPE_STRING2, NULL, \
		(void * *)&pClient_config->pClient_id}, \
	{"client-secret", OPTION_TYPE_STRING2, NULL, \
		(void * *)&pClient_config->pClient_secret}, \
	{"refresh-token", OPTION_TYPE_FILE, NULL, \
		(void * *)&pClient_config->pRefresh_token}, \
	{"access-token", OPTION_TYPE_FILE, NULL, \
		(void * *)&pClient_config->pAccess_token}, \
	{"fw-variant", OPTION_TYPE_STRING2, NULL, \
		(void * *)&pClient_config->pFw_variant}, \
	{"device-id", OPTION_TYPE_STRING2, NULL, \
		(void * *)&pClient_config->password.pDevice_id}, \
	{"fw-major-version", OPTION_TYPE_STRING, \
		pClient_config->fw_version.sMajor, \
		NULL, sizeof (pClient_config->fw_version.sMajor)}, \
	{"fw-minor-version", OPTION_TYPE_STRING, \
		pClient_config->fw_version.sMinor, \
		NULL, sizeof (pClient_config->fw_version.sMinor)}, \
	{"fw-revision-version", OPTION_TYPE_STRING, \
		pClient_config->fw_version.sRevision, \
		NULL, sizeof (pClient_config->fw_version.sRevision)}, \
\
	/* configuration only for unencrypted firmware */ \
\
	{"series-name", OPTION_TYPE_STRING, \
		pClient_config->sSeries_name, \
		NULL, sizeof (pClient_config->sSeries_name)}, \
	{"hardware-version", OPTION_TYPE_STRING, \
		pClient_config->sHardware_version, \
		NULL, sizeof (pClient_config->sHardware_version)}, \
	{NULL, 0, NULL, NULL, 0} \
}

int get_http_response_code(int * pCode,
							struct http_chunk * pHttp_header);
int get_http_error_message(int * pCode, char * * pMsg,
							struct http_chunk * pHttp_data);

size_t handle_recv_data(void * pPtr,
							size_t iSize,
							size_t iNmemb,
							void * pUser_info);

enum fota_request_type action_of_error_code(
						enum fota_error_code iError_code);
int get_registration_status(struct fota_client_info * pDevice_info);
int register_to_fota_server(struct fota_client_info * pDevice_info);
int refresh_access_token(struct fota_client_info * pDevice_info);
int upload_device_info(struct fota_client_info * pDevice_info);
int fota_handle_token_expire(
						enum fota_request_type iType,
						struct fota_client_info * pDevice_info);

int fota_parse_config_file(
						const char * pConfig_file,
						struct fota_option * pConfig);
int free_device_info(struct fota_client_info * pDevice_info);

#endif

