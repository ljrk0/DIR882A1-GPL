
/*
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <syslog.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>

#include <fota_debug.h>
#include <fota_common.h>

#define AES_KEY_SIZE 16
#define AES_ENCRYPT_INPUT_FILE "/tmp/aes_encrypt_input"
#define AES_ENCRYPT_OUTPUT_FILE "/tmp/aes_encrypt_output"
#define AES_DECRYPT_INPUT_FILE "/tmp/aes_decrypt_input"
#define AES_DECRYPT_OUTPUT_FILE "/tmp/aes_decrypt_output"

/*
  * 1. Used to generate a unique temporary file name.
  * 2018-05-30 --liushenghui
*/
inline unsigned long generate_unique_file_id(void)
{
	static unsigned long iMax_id = 0;

	return ++iMax_id;
}

static int binary_data_to_hex_string(
								const void * pAddr,
								size_t iLen,
								char * * pBuf)
{
	int iIndex = 0;

	RETURN_IF_NULL(pAddr, -1);
	RETURN_IF_NULL(pBuf, -1);
	RETURN_IF_EQUAL(iLen, 0, -1);

	*pBuf = malloc(2 * iLen + 1);
	if (!*pBuf)
	{
		DEBUG("malloc fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	for (iIndex = 0 ; iIndex < iLen ; ++iIndex)
	{
		snprintf((*pBuf) + 2 * iIndex, 3, "%02hhx",
			((const char *)pAddr)[iIndex]);
	}

	return 0;
}

static int hex_string_to_binary_data(
								const char * pHex_string,
								void * * pBuf,
								size_t * pBuf_len)
{
	char sByte_buf[8] = { 0 };
	int iResult_code = -1;
	int iIndex = 0;
	size_t iString_len = 0;
	long iTmp = 0;

	RETURN_IF_NULL(pHex_string, -1);
	RETURN_IF_NULL(pBuf, -1);
	RETURN_IF_NULL(pBuf_len, -1);

	iString_len = strlen(pHex_string);
	RETURN_IF_EQUAL(iString_len, 0, -1);
	if (0 != iString_len % 2)
	{
		DEBUG("The number of characters in the hex string is odd."
			" It is %u.\n", iString_len);
		return -1;
	}

	*pBuf_len = iString_len / 2;
	*pBuf = malloc(*pBuf_len);
	if (!*pBuf)
	{
		DEBUG("malloc fail, %s, errno=%d.\n", strerror(errno), errno);
		return -1;
	}

	for (iIndex = 0 ; iIndex < iString_len / 2 ; ++iIndex)
	{
		sByte_buf[0] = pHex_string[2 * iIndex];
		sByte_buf[1] = pHex_string[2 * iIndex + 1];

		iTmp = strtol(sByte_buf, NULL, 16);

		if (LONG_MIN == iTmp
			|| LONG_MAX == iTmp
		)
		{
			DEBUG("strtol fail, %s, errno=%d, iTmp=%ld.\n",
				strerror(errno), errno, iTmp);
			goto done;
		}

		((char *)*pBuf)[iIndex] = (char)iTmp;
	}

	iResult_code = 0;

done:

	return iResult_code;
}

static int openssl_aes_128_ecb_encrypt(
							char * pKey_hex,
							const void * pInput_data,
							size_t iInput_data_len,
							void * * pOutput_data,
							size_t * pOutput_data_len)
{
	char sInput_filename[128] = { 0 };
	char sOutput_filename[128] = { 0 };
	char * const argv[] =
		{
			"openssl",
			"enc",
			"-e",
			"-aes-128-ecb",
			"-K",
			pKey_hex,
			"-in",
			sInput_filename,
			"-out",
			sOutput_filename,
			NULL
		};
	int iRet = 0;
	int iResult_code = -1;
	int iOpenssl_code = -1;
	size_t iOutput_data_len = 0;
	unsigned long iUnique_file_id = 0;
	pid_t pid;

	RETURN_IF_NULL(pKey_hex, -1);
	RETURN_IF_NULL(pInput_data, -1);
	RETURN_IF_EQUAL(iInput_data_len, 0, -1);
	RETURN_IF_NULL(pOutput_data, -1);
	RETURN_IF_NULL(pOutput_data_len, -1);

	pid = getpid();
	iUnique_file_id = generate_unique_file_id();

	snprintf(sInput_filename, sizeof (sInput_filename), "%s.%d.%lu",
		AES_ENCRYPT_INPUT_FILE, (int)pid, iUnique_file_id);
	snprintf(sOutput_filename, sizeof (sOutput_filename), "%s.%d.%lu",
		AES_ENCRYPT_OUTPUT_FILE, (int)pid, iUnique_file_id);

	iRet = fota_output_to_file(sInput_filename,
		pInput_data, iInput_data_len);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = execute_action("openssl", argv, &iOpenssl_code);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);
	GOTO_IF_NOT_EQUAL(iOpenssl_code, 0, done);

	iOutput_data_len =
		AES_KEY_SIZE * (iInput_data_len / AES_KEY_SIZE + 1);

	iRet = fota_get_file_content(sOutput_filename,
		pOutput_data, pOutput_data_len);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	if (*pOutput_data_len != iOutput_data_len)
	{
		DEBUG("The length of encrypt data is %u, expect: %u.\n",
			*pOutput_data_len, iOutput_data_len);
		goto done;
	}

	iResult_code = 0;

done:

	unlink(sInput_filename);
	unlink(sOutput_filename);

	return iResult_code;
}

static int openssl_aes_128_ecb_decrypt(
							char * pKey_hex,
							const void * pInput_data,
							size_t iInput_data_len,
							void * * pOutput_data,
							size_t * pOutput_data_len)
{
	char sInput_filename[128] = { 0 };
	char sOutput_filename[128] = { 0 };
	char * const argv[] =
		{
			"openssl",
			"enc",
			"-d",
			"-aes-128-ecb",
			"-K",
			pKey_hex,
			"-in",
			sInput_filename,
			"-out",
			sOutput_filename,
			NULL
		};
	int iRet = 0;
	int iResult_code = -1;
	int iOpenssl_code = -1;
	unsigned long iUnique_file_id = 0;
	pid_t pid;

	RETURN_IF_NULL(pKey_hex, -1);
	RETURN_IF_NULL(pInput_data, -1);
	RETURN_IF_EQUAL(iInput_data_len, 0, -1);
	RETURN_IF_NULL(pOutput_data, -1);
	RETURN_IF_NULL(pOutput_data_len, -1);

	if (0 == iInput_data_len
		|| 0 != iInput_data_len % AES_KEY_SIZE)
	{
		return -1;
	}

	pid = getpid();
	iUnique_file_id = generate_unique_file_id();

	snprintf(sInput_filename, sizeof (sInput_filename), "%s.%d.%lu",
		AES_DECRYPT_INPUT_FILE, (int)pid, iUnique_file_id);
	snprintf(sOutput_filename, sizeof (sOutput_filename), "%s.%d.%lu",
		AES_DECRYPT_OUTPUT_FILE, (int)pid, iUnique_file_id);

	iRet = fota_output_to_file(sInput_filename,
		pInput_data, iInput_data_len);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = execute_action("openssl", argv, &iOpenssl_code);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);
	GOTO_IF_NOT_EQUAL(iOpenssl_code, 0, done);

	iRet = fota_get_file_content(sOutput_filename,
		pOutput_data, pOutput_data_len);
	if (iRet != 0)
	{
		DEBUG("Obtain the result of aes decrypt fail.\n");
		goto done;
	}

	iResult_code = 0;

done:

	unlink(sInput_filename);
	unlink(sOutput_filename);

	return iResult_code;
}

int aes_encrypt(
					char * pKey_hex,
					const char * pInput_data,
					char * * pOutput_data_hex)
{
	char * pOutput_data = NULL;
	int iRet = 0;
	int iResult_code = -1;
	size_t iOutput_data_len = 0;

	RETURN_IF_NULL(pKey_hex, -1);
	RETURN_IF_NULL(pInput_data, -1);
	RETURN_IF_NULL(pOutput_data_hex, -1);

	iRet = openssl_aes_128_ecb_encrypt(pKey_hex,
		(const void *)pInput_data, strlen(pInput_data),
		(void * *)&pOutput_data, &iOutput_data_len);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = binary_data_to_hex_string((const void *)pOutput_data,
		iOutput_data_len, pOutput_data_hex);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pOutput_data);

	return iResult_code;
}

int aes_decrypt(
					char * pKey_hex,
					const char * pInput_data_hex,
					char * * pOutput_data)
{
	void * pInput_data = NULL;
	int iRet = 0;
	int iResult_code = -1;
	size_t iInput_data_len = 0;
	size_t iOutput_data_len = 0;

	RETURN_IF_NULL(pKey_hex, -1);
	RETURN_IF_NULL(pInput_data_hex, -1);
	RETURN_IF_NULL(pOutput_data, -1);

	iRet = hex_string_to_binary_data(pInput_data_hex,
		&pInput_data, &iInput_data_len);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iRet = openssl_aes_128_ecb_decrypt(pKey_hex,
		pInput_data, iInput_data_len,
		(void * *)pOutput_data, &iOutput_data_len);
	GOTO_IF_NOT_EQUAL(iRet, 0, done);

	iResult_code = 0;

done:

	FREE_POINTER(pInput_data);

	return iResult_code;
}

