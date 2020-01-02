/* vi: set sw=4 ts=4 sts=4: */
/*
 *  station.c -- Station Mode
 *
 *  Copyright (c) Ralink Technology Corporation All Rights Reserved.
 *
 *  $Id: //WIFI_SOC/MP/SDK_5_0_0_0/RT288x_SDK/source/user/goahead/src/station.c#1 $
 */

#include	<sys/ioctl.h>
#include	<arpa/inet.h>
#include	<asm/types.h>
#include	<linux/if.h>
#include	<linux/wireless.h>
#include	<linux/autoconf.h>  //kernel config
#include	<dirent.h>
//#include	"webs.h"
#include	"oid.h"
#include	"stapriv.h"
#include	"nvram.h"
#include	"utils.h"
#include	"stdlib.h"
//#include	"internet.h"

//#define ENHANCED_STA_STATUS_INFO	// Display full Link Status information and detailed BF statistics

/*---------------------------
 * function declaration
 *---------------------------*/
int PasswordHash(char *password, unsigned char *ssid, int ssidlength, unsigned char *output);
#if defined (WPA_SUPPLICANT_SUPPORT)
static void conf_WPASupplicant(char* ssid, RT_WPA_SUPPLICANT_KEY_MGMT keymgmt, RT_WPA_SUPPLICANT_EAP eap, char* identity, char* password, char* cacert, char* clientcert, char* privatekey, char* privatekeypassword, char* wepkey, int keyidx, NDIS_802_11_WEP_STATUS encryp, RT_WPA_SUPPLICANT_TUNNEL tunnel, NDIS_802_11_AUTHENTICATION_MODE auth);
#endif


/*---------------------------
 * Global Variable
 *---------------------------*/
#define TOKEN "\t"

PRT_PROFILE_SETTING selectedProfileSetting = NULL, headerProfileSetting = NULL, currentProfileSetting = NULL;
unsigned char   Active_flag=0, nConfig_flag=0;
unsigned int    m_nSigQua[3] = {0,0,0};
unsigned long   m_lTxCount = 0;
unsigned long   m_lRxCount = 0;
unsigned long   m_lChannelQuality = 0;
char    G_bRadio = 1; //TRUE
char    G_bdBm_ischeck = 0; //false
char    G_staProfileNum = 0;
NDIS_802_11_SSID        G_SSID;
unsigned char			G_Bssid[6];
int        G_ConnectStatus = NdisMediaStateDisconnected;
unsigned char WpaSupplicant_flag = FALSE;

PAIR_CHANNEL_FREQ_ENTRY ChannelFreqTable[] = {
	//channel Frequency
	{1,     2412000},
	{2,     2417000},
	{3,     2422000},
	{4,     2427000},
	{5,     2432000},
	{6,     2437000},
	{7,     2442000},
	{8,     2447000},
	{9,     2452000},
	{10,    2457000},
	{11,    2462000},
	{12,    2467000},
	{13,    2472000},
	{14,    2484000},
	{34,    5170000},
	{36,    5180000},
	{38,    5190000},
	{40,    5200000},
	{42,    5210000},
	{44,    5220000},
	{46,    5230000},
	{48,    5240000},
	{52,    5260000},
	{56,    5280000},
	{60,    5300000},
	{64,    5320000},
	{100,   5500000},
	{104,   5520000},
	{108,   5540000},
	{112,   5560000},
	{116,   5580000},
	{120,   5600000},
	{124,   5620000},
	{128,   5640000},
	{132,   5660000},
	{136,   5680000},
	{140,   5700000},
	{149,   5745000},
	{153,   5765000},
	{157,   5785000},
	{161,   5805000},
	{165,	5825000},
	{167,	5835000},
	{169,	5845000},
	{171,	5855000},
	{173,	5865000},
	{184,	4920000},
	{188,	4940000},
	{192,	4960000},
	{196,	4980000},
	{208,	5040000},	/* Japan, means J08 */
	{212,	5060000},	/* Japan, means J12 */
	{216,	5080000},	/* Japan, means J16 */
};
int G_nChanFreqCount = sizeof (ChannelFreqTable) / sizeof(PAIR_CHANNEL_FREQ_ENTRY);

void freeHeaderProfileSettings(void)
{
	PRT_PROFILE_SETTING list = headerProfileSetting;
	PRT_PROFILE_SETTING next;
	while(list){
		next = list->Next;
		free(list);
		list = next;
		next = list->Next;
	}
}

#if 0
int OidQueryInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}

int OidSetInformation(unsigned long OidQueryCode, int socket_id, char *DeviceName, void *ptr, unsigned long PtrLength)
{
	struct iwreq wrq;

	strcpy(wrq.ifr_name, DeviceName);
	wrq.u.data.length = PtrLength;
	wrq.u.data.pointer = (caddr_t) ptr;
	wrq.u.data.flags = OidQueryCode | OID_GET_SET_TOGGLE;

	return (ioctl(socket_id, RT_PRIV_IOCTL, &wrq));
}
#endif

/*
 * description: convert ascii byte to numeric
 */
unsigned char BtoH(char ch)
{
	if (ch >= '0' && ch <= '9') return (ch - '0');        // Handle numerals
	if (ch >= 'A' && ch <= 'F') return (ch - 'A' + 0xA);  // Handle capitol hex digits
	if (ch >= 'a' && ch <= 'f') return (ch - 'a' + 0xA);  // Handle small hex digits
	return(255);
}

/*
 * description: Converts ascii string to network order hex
 * parameters: src     - pointer to input ascii string
 *             dest    - pointer to output hex
 *             destlen - size of dest
 */
void AtoH(char * src, unsigned char * dest, int destlen)
{
	char * srcptr;

	srcptr = src;
	unsigned char* destTemp = dest;
	while (destlen--) {
		*destTemp = BtoH(*srcptr++) << 4;  // Put 1st ascii byte in upper nibble.
		*destTemp += BtoH(*srcptr++);      // Add 2nd ascii byte to above.
		destTemp++;
	}
}

static void shaHashBlock(A_SHA_CTX *ctx) 
{
	int t;
	unsigned long A,B,C,D,E,TEMP;

#define SHA_ROTL(X,n) ((((X) << (n)) | ((X) >> (32-(n)))) & 0xffffffffL)
	for (t = 16; t <= 79; t++)
		ctx->W[t] = SHA_ROTL(ctx->W[t-3] ^ ctx->W[t-8] ^ ctx->W[t-14] ^ ctx->W[t-16], 1);

	A = ctx->H[0];
	B = ctx->H[1];
	C = ctx->H[2];
	D = ctx->H[3];
	E = ctx->H[4];

	for (t = 0; t <= 19; t++) 
	{
		TEMP = (SHA_ROTL(A,5) + (((C^D)&B)^D)     + E + ctx->W[t] + 0x5a827999L) & 0xffffffffL;
		E = D; D = C; C = SHA_ROTL(B, 30); B = A; A = TEMP;
	}
	for (t = 20; t <= 39; t++) 
	{
		TEMP = (SHA_ROTL(A,5) + (B^C^D)           + E + ctx->W[t] + 0x6ed9eba1L) & 0xffffffffL;
		E = D; D = C; C = SHA_ROTL(B, 30); B = A; A = TEMP;
	}
	for (t = 40; t <= 59; t++) 
	{
		TEMP = (SHA_ROTL(A,5) + ((B&C)|(D&(B|C))) + E + ctx->W[t] + 0x8f1bbcdcL) & 0xffffffffL;
		E = D; D = C; C = SHA_ROTL(B, 30); B = A; A = TEMP;
	}
	for (t = 60; t <= 79; t++) 
	{
		TEMP = (SHA_ROTL(A,5) + (B^C^D)           + E + ctx->W[t] + 0xca62c1d6L) & 0xffffffffL;
		E = D; D = C; C = SHA_ROTL(B, 30); B = A; A = TEMP;
	}

	ctx->H[0] += A;
	ctx->H[1] += B;
	ctx->H[2] += C;
	ctx->H[3] += D;
	ctx->H[4] += E;
}

void A_SHAInit(A_SHA_CTX *ctx) 
{
	int i;

	ctx->lenW = 0;
	ctx->sizeHi = ctx->sizeLo = 0;

	/* Initialize H with the magic constants (see FIPS180 for constants)
	*/
	ctx->H[0] = 0x67452301L;
	ctx->H[1] = 0xefcdab89L;
	ctx->H[2] = 0x98badcfeL;
	ctx->H[3] = 0x10325476L;
	ctx->H[4] = 0xc3d2e1f0L;

	for (i = 0; i < 80; i++)
		ctx->W[i] = 0;
}

void A_SHAUpdate(A_SHA_CTX *ctx, unsigned char *dataIn, int len) 
{
	int i;

	/* Read the data into W and process blocks as they get full */
	for (i = 0; i < len; i++) 
	{
		ctx->W[ctx->lenW / 4] <<= 8;
		ctx->W[ctx->lenW / 4] |= (unsigned long)dataIn[i];
		if ((++ctx->lenW) % 64 == 0) 
		{
			shaHashBlock(ctx);
			ctx->lenW = 0;
		}
		ctx->sizeLo += 8;
		ctx->sizeHi += (ctx->sizeLo < 8);
	}
}

void A_SHAFinal(A_SHA_CTX *ctx, unsigned char hashout[20]) 
{
	unsigned char pad0x80 = 0x80;
	unsigned char pad0x00 = 0x00;
	unsigned char padlen[8];
	int i;

	/* Pad with a binary 1 (e.g. 0x80), then zeroes, then length */
	padlen[0] = (unsigned char)((ctx->sizeHi >> 24) & 255);
	padlen[1] = (unsigned char)((ctx->sizeHi >> 16) & 255);
	padlen[2] = (unsigned char)((ctx->sizeHi >> 8) & 255);
	padlen[3] = (unsigned char)((ctx->sizeHi >> 0) & 255);
	padlen[4] = (unsigned char)((ctx->sizeLo >> 24) & 255);
	padlen[5] = (unsigned char)((ctx->sizeLo >> 16) & 255);
	padlen[6] = (unsigned char)((ctx->sizeLo >> 8) & 255);
	padlen[7] = (unsigned char)((ctx->sizeLo >> 0) & 255);
	A_SHAUpdate(ctx, &pad0x80, 1);
	while (ctx->lenW != 56)
		A_SHAUpdate(ctx, &pad0x00, 1);
	A_SHAUpdate(ctx, padlen, 8);

	/* Output hash
	*/
	for (i = 0; i < 20; i++) 
	{
		hashout[i] = (unsigned char)(ctx->H[i / 4] >> 24);
		ctx->H[i / 4] <<= 8;
	}

	/*
	 *  Re-initialize the context (also zeroizes contents)
	 */
	A_SHAInit(ctx); 
}

static void hmac_sha1(unsigned char *text, int text_len, unsigned char *key, int key_len, unsigned char *digest) 
{ 
	A_SHA_CTX context; 
	unsigned char k_ipad[65]; /* inner padding - key XORd with ipad */ 
	unsigned char k_opad[65]; /* outer padding - key XORd with opad */ 
	int i; 

	/* if key is longer than 64 bytes reset it to key=SHA1(key) */ 
	if (key_len > 64) 
	{ 
		A_SHA_CTX tctx; 

		A_SHAInit(&tctx); 
		A_SHAUpdate(&tctx, key, key_len); 
		A_SHAFinal(&tctx, key); 

		key_len = 20; 
	} 

	/* 
	 * the HMAC_SHA1 transform looks like: 
	 * 
	 * SHA1(K XOR opad, SHA1(K XOR ipad, text)) 
	 * 
	 * where K is an n byte key 
	 * ipad is the byte 0x36 repeated 64 times 
	 * opad is the byte 0x5c repeated 64 times 
	 * and text is the data being protected 
	 */ 

	/* start out by storing key in pads */ 
	memset(k_ipad, 0, sizeof k_ipad); 
	memset(k_opad, 0, sizeof k_opad); 
	memcpy(k_ipad, key, key_len); 
	memcpy(k_opad, key, key_len); 

	/* XOR key with ipad and opad values */ 
	for (i = 0; i < 64; i++) 
	{ 
		k_ipad[i] ^= 0x36; 
		k_opad[i] ^= 0x5c; 
	} 

	/* perform inner SHA1*/ 
	A_SHAInit(&context); /* init context for 1st pass */ 
	A_SHAUpdate(&context, k_ipad, 64); /* start with inner pad */ 
	A_SHAUpdate(&context, text, text_len); /* then text of datagram */ 
	A_SHAFinal(&context, digest); /* finish up 1st pass */ 

	/* perform outer SHA1 */ 
	A_SHAInit(&context); /* init context for 2nd pass */ 
	A_SHAUpdate(&context, k_opad, 64); /* start with outer pad */ 
	A_SHAUpdate(&context, digest, 20); /* then results of 1st hash */ 
	A_SHAFinal(&context, digest); /* finish up 2nd pass */ 
} 

/*
 * F(P, S, c, i) = U1 xor U2 xor ... Uc 
 * U1 = PRF(P, S || Int(i)) 
 * U2 = PRF(P, U1) 
 * Uc = PRF(P, Uc-1) 
 */ 
static void F(char *password, unsigned char *ssid, int ssidlength, int iterations, int count, unsigned char *output) 
{ 
	unsigned char digest[36], digest1[A_SHA_DIGEST_LEN]; 
	int i, j; 

	/* U1 = PRF(P, S || int(i)) */ 
	memcpy(digest, ssid, ssidlength); 
	digest[ssidlength] = (unsigned char)((count>>24) & 0xff); 
	digest[ssidlength+1] = (unsigned char)((count>>16) & 0xff); 
	digest[ssidlength+2] = (unsigned char)((count>>8) & 0xff); 
	digest[ssidlength+3] = (unsigned char)(count & 0xff); 
	hmac_sha1(digest, ssidlength+4, (unsigned char*) password, (int) strlen(password), digest1); // for WPA update

	/* output = U1 */ 
	memcpy(output, digest1, A_SHA_DIGEST_LEN); 

	for (i = 1; i < iterations; i++) 
	{ 
		/* Un = PRF(P, Un-1) */ 
		hmac_sha1(digest1, A_SHA_DIGEST_LEN, (unsigned char*) password, (int) strlen(password), digest); // for WPA update
		memcpy(digest1, digest, A_SHA_DIGEST_LEN); 

		/* output = output xor Un */ 
		for (j = 0; j < A_SHA_DIGEST_LEN; j++) 
		{ 
			output[j] ^= digest[j]; 
		} 
	} 
}
 
int PasswordHash(char *password, unsigned char *ssid, int ssidlength, unsigned char *output) 
{ 
	if ((strlen(password) > 63) || (ssidlength > 32)) 
		return 0; 

	F(password, ssid, ssidlength, 4096, 1, output); 
	F(password, ssid, ssidlength, 4096, 2, &output[A_SHA_DIGEST_LEN]); 
	return 1; 
}

/*
 * description: 
 *	station profile initialization
 * 	Initial the Profile List from values in NVRAM named staXXX.
 * 	Each Profile is seperated by '\t' in NVRAM values.
 */
int initStaProfile(void)
{
	PRT_PROFILE_SETTING nextProfileSetting;
	char tmp_buffer[512];
	const char *wordlist = NULL;
	char *tok = NULL;
	int i;
	char *value = (char *) nvram_bufget(RT2860_NVRAM, "RadioOff");
	int s = socket(AF_INET, SOCK_DGRAM, 0);

	if (strcmp((char *)value, "1") == 0) {
		G_bRadio = 0;
		OidSetInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, 1);
	} else {
		G_bRadio = 1;
		OidSetInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, 1);
	}
	close(s);

	// fprintf(stderr, "kathy -- init_StaProfile()\n");
	G_ConnectStatus = NdisMediaStateDisconnected;

	//staProfile
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staProfile");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("no previous profiles defined");
		return 0;
	}

	//It is always NULL in Lighty, because cgi is not a daemon.
	if (headerProfileSetting == NULL ) { //It is always NULL in Lighty
		headerProfileSetting = malloc(sizeof(RT_PROFILE_SETTING));
		memset(headerProfileSetting, 0x00, sizeof(RT_PROFILE_SETTING));
		headerProfileSetting->Next = NULL;
	}
	currentProfileSetting = headerProfileSetting;

	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN) ; tok ;  i++) {
		//profile
		sprintf((char *)currentProfileSetting->Profile, "%s", tok);
		// fprintf(stderr, "i=%d, Profile=%s, tok=%s\n", i,currentProfileSetting->Profile, tok);
		tok = strtok(NULL, TOKEN);

		if (tok != NULL && currentProfileSetting->Next == NULL) {
			nextProfileSetting = malloc(sizeof(RT_PROFILE_SETTING));
			memset(nextProfileSetting, 0x00, sizeof(RT_PROFILE_SETTING));
			nextProfileSetting->Next = NULL;
			currentProfileSetting->Next = nextProfileSetting;
			currentProfileSetting = nextProfileSetting;
		}
		else
			currentProfileSetting = currentProfileSetting->Next;
	}
	G_staProfileNum = i;

	// SSID
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staSSID");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta SSID has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->SSID, "%s", tok);
		currentProfileSetting->SsidLen = strlen(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// NetworkType
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staNetworkType");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta NetworkType has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->NetworkType = atoi(tok);
		// fprintf(stderr, "i=%d, NetworkType=%d\n", i,currentProfileSetting->NetworkType);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// PSMode
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staPSMode");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta PSMode has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->PSmode= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// AdhocMode
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staAdhocMode");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta AdhocMode has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->AdhocMode= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Channel
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staChannel");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta Channel has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Channel= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// PreamType
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staPreamType");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta PreamType has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->PreamType= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// RTSCheck
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staRTSCheck");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta RTSCheck has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->RTSCheck= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// FragmentCheck
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staFragmentCheck");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta FragmentCheck has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->FragmentCheck= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// RTS
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staRTS");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta RTS has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->RTS= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Fragment
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staFragment");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Fragment has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Fragment= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Auth
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staAuth");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Auth has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Authentication= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Encryption
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staEncrypt");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Encryption has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Encryption= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key1	
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey1");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key1 has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Key1, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key2
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey2");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key2 has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Key2, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key3
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey3");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key3 has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Key3, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key4
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey4");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key4 has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Key4, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key1Type
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey1Type");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key1Type has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key1Type= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key2Type
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey2Type");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key2Type has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key2Type= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key3Type
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey3Type");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key3Type has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key3Type= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key4Type
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey4Type");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key4Type has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key4Type= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key1Length
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey1Length");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key1Lenght has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key1Length= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key2Length
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey2Length");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key2Lenght has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key2Length= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key3Length
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey3Length");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key3Lenght has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key3Length= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// Key4Length
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey4Length");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta Key4Length has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Key4Length= atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// DefaultKeyID
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staKeyDefaultId");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta DefaultKeyID has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->KeyDefaultId = atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// WPAPSK
	//bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staWpaPsk");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta WPAPSK has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	//snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", wordlist);
	char *strp = strdup(wordlist);
	for (i = 0, tok = strsep(&strp, TOKEN); tok; tok = strsep(&strp, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->WpaPsk, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}
	free(strp);
#ifdef WPA_SUPPLICANT_SUPPORT
	//keymgmt
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xKeyMgmt");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Key Mgmt has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->KeyMgmt = atoi(tok);

		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	// EAP
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xEAP");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x EAP has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->EAP = atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert ID
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xIdentity");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Identity has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Identity, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert Password
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xPassword");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Password has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->Password, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert Client Cert Path
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xClientCert");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Client Cert has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->ClientCert, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert Private Key Path
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xPrivateKey");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Private Key has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->PrivateKey, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert Private Key Password
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xPrivateKeyPassword");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Private Key Password has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->PrivateKeyPassword, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Cert CA Cert
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xCACert");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x CA Cert has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		sprintf((char *)currentProfileSetting->CACert, "%s", tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}

	//Tunnel
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xTunnel");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0) {
		DBG_MSG("Sta 802.1x Tunnel has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Tunnel = atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}
#endif

	//Active
	bzero(tmp_buffer, sizeof(tmp_buffer));
	wordlist = nvram_bufget(RT2860_NVRAM, "staActive");
	if (wordlist == NULL || strcmp(wordlist, "" ) == 0)	{
		DBG_MSG("Sta Active has no data.");
		return -1;
	}

	currentProfileSetting = headerProfileSetting;
	sprintf(tmp_buffer, "%s", wordlist);
	for (i = 0, tok = strtok(tmp_buffer, TOKEN); tok; tok = strtok(NULL, TOKEN), i++) {
		currentProfileSetting->Active = atoi(tok);
		if (currentProfileSetting->Next != NULL)
			currentProfileSetting = currentProfileSetting->Next;
	}
	return 0;
}

/*
 * description:
 * 	1. Before call this function, the initStaProfile() should be called.
 * 	2. It will update following Global Variable.
 *   	  headerProfileSetting   -> Profile List header
 *   	  selectedProfileSetting -> The 'selectedIndex'th Profile.
 * 	3. It return the 'selectedIndex - 1' Profile pointer.
 *    	  previousProfileSetting -> The 'selectedIndex - 1' the Profile.
 */
PRT_PROFILE_SETTING selectStaProfile(int pIndex)
{
	PRT_PROFILE_SETTING previousProfileSetting = NULL;
	int i;

	previousProfileSetting = selectedProfileSetting = headerProfileSetting;
	for (i = 2; i <= pIndex; i++) {
		selectedProfileSetting = selectedProfileSetting->Next;
		if (i == pIndex-1)
			previousProfileSetting = selectedProfileSetting;
	}
	if (selectedProfileSetting == headerProfileSetting) {
		previousProfileSetting = NULL;
	}

	return previousProfileSetting;
}

static void writeProfileToNvram()
{
	char tmp_buffer[512], tmp_data[8];

	DBG_MSG("\n[DBG]%s <-----\n", __func__); //Landen

	if (headerProfileSetting == NULL)
		return;

	//profile name
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->Profile);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staProfile", tmp_buffer);

	//ssid
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->SSID);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staSSID", tmp_buffer);

	//NetworkType
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->NetworkType);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staNetworkType", tmp_buffer);

	//PSMode
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->PSmode);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staPSMode", tmp_buffer);

	//AdhocMode
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->AdhocMode);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staAdhocMode", tmp_buffer);

	//Channel
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Channel);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staChannel", tmp_buffer);

	//PreamType
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->PreamType);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staPreamType", tmp_buffer);

	//RTSCheck
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->RTSCheck);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staRTSCheck", tmp_buffer);

	//FragmentCheck
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->FragmentCheck);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staFragmentCheck", tmp_buffer);

	//AdhocMode
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->RTS);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staRTS", tmp_buffer);

	//Fragment
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Fragment);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staFragment", tmp_buffer);
	/* Security Policy */

	//Authentication
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Authentication);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staAuth", tmp_buffer);

	//Encryption
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Encryption);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staEncrypt", tmp_buffer);

	//KeyDefaultId
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->KeyDefaultId);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKeyDefaultId", tmp_buffer);

	//Key1Type
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Key1Type);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey1Type", tmp_buffer);

	//Key2Type
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Key2Type);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey2Type", tmp_buffer);

	//Key3Type
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Key3Type);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey3Type", tmp_buffer);

	//Key4Type
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Key4Type);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey4Type", tmp_buffer);

	//Key1Lenght
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Key1Length);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey1Length", tmp_buffer);

	//Key2Length
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Key2Length);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey2Type", tmp_buffer);

	//Key3Length
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Key3Length);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey3Length", tmp_buffer);

	//Key4Length
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Key4Length);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey4Length", tmp_buffer);

	//Key1
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->Key1);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey1", tmp_buffer);

	//Key2
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->Key2);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey2", tmp_buffer);

	//Key3
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->Key3);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey3", tmp_buffer);

	//Key4
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->Key4);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staKey4", tmp_buffer);

	//WpaPsk
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->WpaPsk);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staWpaPsk", tmp_buffer);

#ifdef WPA_SUPPLICANT_SUPPORT
	//Key Mgmt
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->KeyMgmt);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xKeyMgmt", tmp_buffer);

	//EAP
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->EAP);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xEAP", tmp_buffer);

	//Tunnel
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Tunnel);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xTunnel", tmp_buffer);

	//Identity
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->Identity);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xIdentity", tmp_buffer);

	//Password
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->Password);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xPassword", tmp_buffer);

	//Client Cert Path
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->ClientCert);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xClientCert", tmp_buffer);

	//Private Key
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->PrivateKey);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xPrivateKey", tmp_buffer);

	//Private Key Password
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->PrivateKeyPassword);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xPrivateKeyPassword", tmp_buffer);

	//CA CertPath
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, (char *)currentProfileSetting->CACert);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xCACert", tmp_buffer);
#else
	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, "7");
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xEAP", tmp_buffer);

	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, "3");
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xTunnel", tmp_buffer);
	nvram_bufset(RT2860_NVRAM, "sta8021xKeyMgmt", tmp_buffer);

	bzero(tmp_buffer, 512);
	currentProfileSetting = headerProfileSetting;
	do {
		strcat(tmp_buffer, "0");
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "sta8021xIdentity", tmp_buffer);
	nvram_bufset(RT2860_NVRAM, "sta8021xPassword", tmp_buffer);
	nvram_bufset(RT2860_NVRAM, "sta8021xClientCert", tmp_buffer);
	nvram_bufset(RT2860_NVRAM, "sta8021xPrivateKey", tmp_buffer);
	nvram_bufset(RT2860_NVRAM, "sta8021xPrivateKeyPassword", tmp_buffer);
	nvram_bufset(RT2860_NVRAM, "sta8021xCACert", tmp_buffer);
#endif

	//Active
	bzero(tmp_buffer, 512);
	bzero(tmp_data, 8);
	currentProfileSetting = headerProfileSetting;
	do {
		sprintf(tmp_data, "%d", currentProfileSetting->Active);
		strcat(tmp_buffer, tmp_data);
		currentProfileSetting = currentProfileSetting->Next;
		if (currentProfileSetting != NULL)
			strcat(tmp_buffer, "\t");	
	} while (currentProfileSetting != NULL);
	nvram_bufset(RT2860_NVRAM, "staActive", tmp_buffer);

	nvram_commit(RT2860_NVRAM);

	DBG_MSG("[DBG]%s ----->\n", __func__); //Landen
}

/*
 * description: station connection
 */
static void sta_connection(int tmp_networktype, int tmp_auth, int tmp_encry, int tmp_defaultkeyid, PNDIS_802_11_SSID pSsid, unsigned char Bssid[6], char *tmp_wpapsk, char *tmp_key1, char *tmp_key2, char *tmp_key3, char *tmp_key4, RT_802_11_PREAMBLE tmp_preamtype, int tmp_rtscheck, NDIS_802_11_RTS_THRESHOLD tmp_rts, int tmp_fragmentcheck, NDIS_802_11_FRAGMENTATION_THRESHOLD tmp_fragment, NDIS_802_11_POWER_MODE tmp_psmode, int tmp_channel)
{
	int s, ret, nKeyLen=0, j, i;
	NDIS_802_11_REMOVE_KEY		removeKey;
	PNDIS_802_11_KEY			pKey = NULL;
	PNDIS_802_11_WEP			pWepKey = NULL;
	PNDIS_802_11_PASSPHRASE		pPassPhrase = NULL;
	unsigned long               PassphraseBufLen;
	unsigned long				lBufLen;
	unsigned char				keyMaterial[40];
	NDIS_802_11_CONFIGURATION	Configuration;
	unsigned long				CurrentWirelessMode;
	char				cmd[256];

	DBG_MSG("\n[DBG]%s <-----", __FUNCTION__); //Landen

	//fprintf(stderr, "sta_connection()\n");
	s = socket(AF_INET, SOCK_DGRAM, 0);

	WpaSupplicant_flag = atoi(nvram_bufget(RT2860_NVRAM , "staWpaSupplicant_flag"));
	if (WpaSupplicant_flag == TRUE)
	{
		DBG_MSG("WpaSupplicant_flag == TRUE"); //Landen
		int wpa_supplicant_support = 0 ,ieee8021x_support = 0;

		do_system("killall wpa_supplicant");
		sleep(2);
		ret = OidSetInformation(OID_802_11_SET_IEEE8021X, s, "ra0", &ieee8021x_support, sizeof(ieee8021x_support));
		if (ret < 0)
			DBG_MSG("Set OID_802_11_SET_IEEE8021X has error =%d, ieee8021x_support=%d", ret, ieee8021x_support);
			//fprintf(stderr, "Set OID_802_11_SET_IEEE8021X has error =%d, ieee8021x_support=%d\n", ret, ieee8021x_support);
		ret = OidSetInformation(RT_OID_WPA_SUPPLICANT_SUPPORT, s, "ra0", &wpa_supplicant_support, sizeof(wpa_supplicant_support));
		if (ret < 0)
			DBG_MSG("Set RT_OID_WPA_SUPPLICANT_SUPPORT has error =%d, wpa_supplicant_support=%d", ret, wpa_supplicant_support);
		nvram_bufset(RT2860_NVRAM, "staWpaSupplicant_flag", "0");
		WpaSupplicant_flag = FALSE;
	}

	//step 0: OID_802_11_INFRASTRUCTURE_MODE
	DBG_MSG("[DBG]step0"); //Landen
	ret = OidSetInformation(OID_802_11_INFRASTRUCTURE_MODE, s, "ra0", &tmp_networktype, sizeof(int));
	if (ret < 0)
		DBG_MSG("Set OID_802_11_INFRASTRUCTURE_MODE has error =%d, networktype=%d", ret, tmp_networktype);

	//step 1:
	DBG_MSG("[DBG]step1"); //Landen
	if (!tmp_rtscheck)
		tmp_rts = 2347;
	ret = OidSetInformation(OID_802_11_RTS_THRESHOLD, s, "ra0", &tmp_rts, sizeof(NDIS_802_11_RTS_THRESHOLD));
	if (ret < 0)
		DBG_MSG("Set OID_802_11_RTS_THRESHOLD has error =%d, rts=%d", ret, tmp_rts);

	if (!tmp_fragmentcheck)
		tmp_fragment = 2346;
	ret = OidSetInformation(OID_802_11_FRAGMENTATION_THRESHOLD, s, "ra0", &selectedProfileSetting->Fragment, sizeof(NDIS_802_11_FRAGMENTATION_THRESHOLD));
	if (ret < 0)
		DBG_MSG("Set OID_802_11_FRAGMENTATION_THRESHOLD has error =%d, Fragment=%d", ret, selectedProfileSetting->Fragment);

	if (tmp_networktype == Ndis802_11Infrastructure) {
		ret = OidSetInformation(OID_802_11_POWER_MODE, s, "ra0", &tmp_psmode, sizeof(NDIS_802_11_POWER_MODE));
		if (ret < 0)
			DBG_MSG("Set OID_802_11_POWER_MODE has error =%d, psmode=%d", ret, tmp_psmode);

		ret = OidSetInformation(RT_OID_802_11_PREAMBLE, s, "ra0", &tmp_preamtype, sizeof(RT_802_11_PREAMBLE));
		if (ret < 0)
			DBG_MSG("Set RT_OID_802_11_PREAMBLE has error =%d, preamtype=%d", ret, tmp_preamtype);
	}
	else if (tmp_networktype == Ndis802_11IBSS) {
		unsigned long	lFreq = 0;
		ret = OidQueryInformation(OID_802_11_CONFIGURATION, s, "ra0", &Configuration, sizeof(Configuration));
		if (ret < 0)
			DBG_MSG("Set OID_802_11_CONFIGURATION has error =%d", ret);

		for (i = 0; i < G_nChanFreqCount; i++) {
			if (tmp_channel == ChannelFreqTable[i].lChannel) {
				lFreq = ChannelFreqTable[i].lFreq;
				break;
			}
		}
		if (lFreq != Configuration.DSConfig) {
			Configuration.DSConfig = lFreq/1000;
			ret = OidSetInformation(OID_802_11_CONFIGURATION, s, "ra0", &Configuration, sizeof(Configuration));
			if (ret < 0)
				DBG_MSG("Set OID_802_11_CONFIGURATION has error=%d",ret);
		}
	}

	//step 2: Security mode
	DBG_MSG("[DBG]step 2"); //Landen
	ret = OidSetInformation(OID_802_11_AUTHENTICATION_MODE, s, "ra0", &tmp_auth, sizeof(tmp_auth));
	if (ret < 0)
		DBG_MSG("Set OID_802_11_AUTHENTICATION_MODE has error =%d, auth=%d", ret, tmp_auth);

	ret = OidSetInformation(OID_802_11_ENCRYPTION_STATUS, s, "ra0", &tmp_encry, sizeof(tmp_encry));
	if (ret < 0)
		DBG_MSG("Set OID_802_11_ENCRYPTION_STATUS has error =%d, encry=%d", ret, tmp_encry);

	if (tmp_encry == Ndis802_11WEPEnabled) {
		//----------------------------------------------------------//
		//Key 1
		//----------------------------------------------------------//

		nKeyLen = strlen(tmp_key1);
		if (nKeyLen == 0) {
			removeKey.Length = sizeof(NDIS_802_11_REMOVE_KEY);
			removeKey.KeyIndex = 0;
			for (j = 0; j < 6; j++)
				removeKey.BSSID[j] = 0xff;
			ret = OidSetInformation(OID_802_11_REMOVE_KEY, s, "ra0", &removeKey, removeKey.Length);
			if (ret < 0)
				DBG_MSG("Set Key1 OID_802_11_REMOVE_KEY has error =%d", ret);
		} else if (strcmp(tmp_key1, "0")) {
			if (nKeyLen == 10)
				nKeyLen = 5;
			else if (nKeyLen == 26)
				nKeyLen = 13;

			lBufLen = sizeof(NDIS_802_11_WEP) + nKeyLen - 1;
			// Allocate Resource
			pWepKey = (PNDIS_802_11_WEP)malloc(lBufLen);
			pWepKey->Length = lBufLen;
			pWepKey->KeyLength = nKeyLen;
			pWepKey->KeyIndex = 0;

			if (tmp_defaultkeyid == 1)
				pWepKey->KeyIndex |= 0x80000000;

			if (strlen(tmp_key1) == 5)
				memcpy(pWepKey->KeyMaterial, tmp_key1, 5);
			else if (strlen(tmp_key1) == 10)
				AtoH(tmp_key1, pWepKey->KeyMaterial, 5);
			else if (strlen(tmp_key1) == 13)
				memcpy(pWepKey->KeyMaterial, tmp_key1, 13);
			else if (strlen(tmp_key1) == 26)
				AtoH(tmp_key1, pWepKey->KeyMaterial, 13);

			ret = OidSetInformation(OID_802_11_ADD_WEP, s, "ra0", pWepKey, pWepKey->Length);
			if (ret < 0)
				DBG_MSG("Set Key1 OID_802_11_ADD_WEP has error =%d", ret);

			free(pWepKey);
		}

		//----------------------------------------------------------//
		//Key 2
		//----------------------------------------------------------//
		nKeyLen = strlen(tmp_key2);
		if (nKeyLen == 0) {
			removeKey.Length = sizeof(NDIS_802_11_REMOVE_KEY);
			removeKey.KeyIndex = 1;
			for (j = 0; j < 6; j++)
				removeKey.BSSID[j] = 0xff;
			ret = OidSetInformation(OID_802_11_REMOVE_KEY, s, "ra0", &removeKey, removeKey.Length);
			if (ret < 0)
				DBG_MSG("Set Key2 OID_802_11_REMOVE_KEY has error =%d", ret);
		} else if (strcmp(tmp_key2, "0")) {
			if (nKeyLen == 10)
				nKeyLen = 5;
			else if (nKeyLen == 26)
				nKeyLen = 13;

			lBufLen = sizeof(NDIS_802_11_WEP) + nKeyLen - 1;
			// Allocate Resource
			pWepKey = (PNDIS_802_11_WEP)malloc(lBufLen);
			pWepKey->Length = lBufLen;
			pWepKey->KeyLength = nKeyLen;
			pWepKey->KeyIndex = 1;

			if (tmp_defaultkeyid == 2)
				pWepKey->KeyIndex |= 0x80000000;

			if (strlen(tmp_key2) == 5)
				memcpy(pWepKey->KeyMaterial, tmp_key2, 5);
			else if (strlen(tmp_key2) == 10)
				AtoH(tmp_key2, pWepKey->KeyMaterial, 5);
			else if (strlen(tmp_key2) == 13)
				memcpy(pWepKey->KeyMaterial, tmp_key2, 13);
			else if (strlen(tmp_key2) == 26)
				AtoH(tmp_key2, pWepKey->KeyMaterial, 13);

			ret = OidSetInformation(OID_802_11_ADD_WEP, s, "ra0", pWepKey, pWepKey->Length);
			if (ret < 0)
				DBG_MSG("Set Key2 OID_802_11_ADD_WEP has error =%d\n", ret);
			free(pWepKey);
		}
		//----------------------------------------------------------//
		//Key 3
		//----------------------------------------------------------//
		nKeyLen = strlen(tmp_key3);
		if (nKeyLen == 0) {
			removeKey.Length = sizeof(NDIS_802_11_REMOVE_KEY);
			removeKey.KeyIndex = 2;
			for(j = 0; j < 6; j++)
				removeKey.BSSID[j] = 0xff;
			ret = OidSetInformation(OID_802_11_REMOVE_KEY, s, "ra0", &removeKey, removeKey.Length);
			if (ret < 0)
				DBG_MSG("Set Key3 OID_802_11_REMOVE_KEY has error =%d", ret);
		} else if (strcmp(tmp_key3, "0")) {
			if (nKeyLen == 10)
				nKeyLen = 5;
			else if (nKeyLen == 26)
				nKeyLen = 13;

			lBufLen = sizeof(NDIS_802_11_WEP) + nKeyLen - 1;
			// Allocate Resource
			pWepKey = (PNDIS_802_11_WEP)malloc(lBufLen);
			pWepKey->Length = lBufLen;
			pWepKey->KeyLength = nKeyLen;
			pWepKey->KeyIndex = 2;
			if (tmp_defaultkeyid == 3)
				pWepKey->KeyIndex |= 0x80000000;

			if (strlen(tmp_key3) == 5)
				memcpy(pWepKey->KeyMaterial, tmp_key3, 5);
			else if (strlen(tmp_key3) == 10)
				AtoH(tmp_key3, pWepKey->KeyMaterial, 5);
			else if (strlen(tmp_key3) == 13)
				memcpy(pWepKey->KeyMaterial, tmp_key3, 13);
			else if (strlen(tmp_key3) == 26)
				AtoH(tmp_key3, pWepKey->KeyMaterial, 13);

			ret = OidSetInformation(OID_802_11_ADD_WEP, s, "ra0", pWepKey, pWepKey->Length);
			if (ret < 0)
				DBG_MSG("Set Key3 OID_802_11_ADD_WEP has error =%d", ret);
			free(pWepKey);
		}
		//----------------------------------------------------------//
		//Key 4
		//----------------------------------------------------------//
		nKeyLen = strlen(tmp_key4);
		if (nKeyLen == 0) {
			removeKey.Length = sizeof(NDIS_802_11_REMOVE_KEY);
			removeKey.KeyIndex = 3;
			for(j = 0; j < 6; j++)
				removeKey.BSSID[j] = 0xff;
			ret = OidSetInformation(OID_802_11_REMOVE_KEY, s, "ra0", &removeKey, removeKey.Length);
			if (ret < 0)
				DBG_MSG("Set Key4 OID_802_11_REMOVE_KEY has error =%d", ret);
		} else if (strcmp(tmp_key4, "0")) {
			if (nKeyLen == 10)
				nKeyLen = 5;
			else if (nKeyLen == 26)
				nKeyLen = 13;

			lBufLen = sizeof(NDIS_802_11_WEP) + nKeyLen - 1;
			// Allocate Resource
			pWepKey = (PNDIS_802_11_WEP)malloc(lBufLen);
			pWepKey->Length = lBufLen;
			pWepKey->KeyLength = nKeyLen;
			pWepKey->KeyIndex = 3;
			if (tmp_defaultkeyid == 4)
				pWepKey->KeyIndex |= 0x80000000;

			if (strlen(tmp_key4) == 5)
				memcpy(pWepKey->KeyMaterial, tmp_key4, 5);
			else if (strlen(tmp_key4) == 10)
				AtoH(tmp_key4, pWepKey->KeyMaterial, 5);
			else if (strlen(tmp_key4) == 13)
				memcpy(pWepKey->KeyMaterial, tmp_key4, 13);
			else if (strlen(tmp_key4) == 26)
				AtoH(tmp_key4, pWepKey->KeyMaterial, 13);

			ret = 	OidSetInformation(OID_802_11_ADD_WEP, s, "ra0", pWepKey, pWepKey->Length);
			if (ret < 0)
				DBG_MSG("Set Key4 OID_802_11_ADD_WEP has error =%d", ret);
			free(pWepKey);
		}
	} else if (tmp_auth == Ndis802_11AuthModeWPAPSK || tmp_auth == Ndis802_11AuthModeWPA2PSK || tmp_auth == Ndis802_11AuthModeWPANone) {
#if 1
		snprintf(cmd, sizeof(cmd), "iwpriv ra0 set WPAPSK=%s", tmp_wpapsk);
		DBG_MSG("[DBG]%s", cmd);//Landen
		do_system(cmd);
#else //Old IOCTL
		nKeyLen = 32;
		lBufLen = (sizeof(NDIS_802_11_KEY) + nKeyLen - 1);
		// Allocate Resouce
		pKey = (PNDIS_802_11_KEY)malloc(lBufLen); // Don't use GMEM_ZEROINIT to get random key
		pKey->Length = lBufLen;
		pKey->KeyLength = nKeyLen;
		pKey->KeyIndex = 0x80000000;

		if (strlen(tmp_wpapsk) == 64) {
			AtoH(tmp_wpapsk, keyMaterial, 32);
			memcpy(pKey->KeyMaterial, keyMaterial, 32);		
		}
		else {
			PasswordHash(tmp_wpapsk, pSsid->Ssid, pSsid->SsidLength, keyMaterial);
DBG_MSG("[DBG] PasswordHash(%s, %s, %d)", tmp_wpapsk, pSsid->Ssid, pSsid->SsidLength); //Landen
			memcpy(pKey->KeyMaterial, keyMaterial, 32);
		}
		PassphraseBufLen = sizeof(NDIS_802_11_PASSPHRASE) + strlen(tmp_wpapsk) - 1;
		pPassPhrase=(PNDIS_802_11_PASSPHRASE)malloc(PassphraseBufLen);
		pPassPhrase->KeyLength = strlen(tmp_wpapsk);
		memcpy(pPassPhrase->KeyMaterial, tmp_wpapsk, pPassPhrase->KeyLength);
		ret = OidSetInformation(OID_802_11_SET_PASSPHRASE, s, "ra0", pPassPhrase, PassphraseBufLen);
		if (ret < 0)
			DBG_MSG("Set OID_802_11_SET_PASSPHRASE has error =%d pPassPhrase=%s", ret, pPassPhrase->KeyMaterial);
		ret = OidSetInformation(RT_OID_802_11_ADD_WPA, s, "ra0", pKey, pKey->Length);
		if (ret < 0)
			DBG_MSG("Set RT_OID_802_11_ADD_WPA has error =%d pKey=%d\n", ret, pKey->KeyMaterial);
		free(pKey);
#endif
	}

	//step 3: SSID
	DBG_MSG("step 3"); //Landen
	if (tmp_networktype == Ndis802_11IBSS ) // Ad hoc use SSID
	{
		ret = OidSetInformation(OID_802_11_SSID, s, "ra0", pSsid, sizeof(NDIS_802_11_SSID));
		if (ret < 0)
			DBG_MSG("Set OID_802_11_SSID has error =%d, pSsid->Ssid=%s", ret, pSsid->Ssid);
		else
			memcpy(&G_SSID, pSsid, sizeof(NDIS_802_11_SSID));
	}
	else
	{
		ret = OidSetInformation(OID_802_11_SSID, s, "ra0", pSsid, sizeof(NDIS_802_11_SSID));
		if (ret < 0)
			DBG_MSG("Set OID_802_11_SSID has error =%d, pSsid->Ssid=%s", ret, pSsid->Ssid);
		else
			memcpy(&G_SSID, pSsid, sizeof(NDIS_802_11_SSID));

		/*
		ret = OidSetInformation(OID_802_11_BSSID, s, "ra0", &Bssid, 6);
		if (ret < 0) {
			error(E_L, E_LOG, "Set OID_802_11_BSSID has error =%d, \n", ret);
		} else 
		*/
		{
			memcpy(G_Bssid, Bssid, 6);
		}
		sleep(1);
	}

	close(s);

	/* restart wan.sh if needed (renew dhcp, pppoe etc) */
	const char *opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (strcmp(opmode, "0"))
	{
		//do_system("wan.sh"); //Landen mark
	}
	DBG_MSG("%s ----->\n", __FUNCTION__); //Landen
}

/*
 * description: connect to AP according to the active profile
 */
void initStaConnection(void)
{
	NDIS_802_11_SSID Ssid;
	PRT_PROFILE_SETTING p = headerProfileSetting;

	if (p == NULL)
		return;
	while (p->Active == 0) {
		if (p->Next == NULL) {
			p = headerProfileSetting;
			break;
		}
		p = p->Next;
	}

	strcpy((char *)Ssid.Ssid ,(char *)p->SSID);
	Ssid.SsidLength = p->SsidLen;
	unsigned char Bssid[6];
	sta_connection(p->NetworkType, p->Authentication, p->Encryption, p->KeyDefaultId, &Ssid, Bssid, (char *)p->WpaPsk, (char *)p->Key1, (char *)p->Key2, (char *)p->Key3, (char *)p->Key4, p->PreamType, p->RTSCheck, p->RTS, p->FragmentCheck, p->Fragment, p->PSmode, p->Channel);
}

/*
 * description: return station radio status
 */
static int getStaProfile(char *input)
{
	char tmpImg[40];
	int i = 0, s;

	NDIS_802_11_SSID                SsidQuery;
	unsigned int                    ConnectStatus = 0;
	NDIS_802_11_WEP_STATUS          Encryp = Ndis802_11WEPDisabled;
	NDIS_802_11_AUTHENTICATION_MODE AuthenType = Ndis802_11AuthModeOpen;
	NDIS_802_11_NETWORK_INFRASTRUCTURE      NetworkType = Ndis802_11Infrastructure;

	initStaProfile();
	if (G_staProfileNum == 0)
		return 0;
	if (headerProfileSetting == NULL)
		return 0;

	currentProfileSetting = headerProfileSetting;
	do {
		memset(tmpImg, 0x00, sizeof(tmpImg));
		// check activate function for the profile
		if (currentProfileSetting->Active)
		{
			// get connected SSID
			s = socket(AF_INET, SOCK_DGRAM, 0);

			//step 1
			if (OidQueryInformation(OID_GEN_MEDIA_CONNECT_STATUS, s, "ra0", &ConnectStatus, sizeof(ConnectStatus)) < 0) {
				DBG_MSG("Query OID_GEN_MEDIA_CONNECT_STATUS error!");
				close(s);
				return 0;
			}

			//step 2
			if (OidQueryInformation(RT_OID_802_11_RADIO, s, "ra0", &G_bRadio, sizeof(G_bRadio)) < 0) {
				DBG_MSG("Query RT_OID_802_11_RADIO error!");
				close(s);
				return 0;
			}
			if (ConnectStatus == 1 && G_bRadio)
			{
				OidQueryInformation(OID_802_11_WEP_STATUS, s, "ra0", &Encryp, sizeof(Encryp) );
				OidQueryInformation(OID_802_11_AUTHENTICATION_MODE, s, "ra0", &AuthenType, sizeof(AuthenType));
				OidQueryInformation(OID_802_11_INFRASTRUCTURE_MODE, s, "ra0", &NetworkType, sizeof(NetworkType));

				memset(&SsidQuery, 0x00, sizeof(SsidQuery));
				OidQueryInformation(OID_802_11_SSID, s, "ra0", &SsidQuery, sizeof(SsidQuery));

				int tmp_auth = currentProfileSetting->Authentication;
				if (tmp_auth == Ndis802_11AuthModeMax)
					tmp_auth = Ndis802_11AuthModeOpen;
				if (strcmp((char *)SsidQuery.Ssid, (char *)currentProfileSetting->SSID) == 0 &&
						currentProfileSetting->Encryption == Encryp &&
						tmp_auth == AuthenType &&
						currentProfileSetting->NetworkType == NetworkType)
				{
					memcpy(&G_SSID, &SsidQuery, sizeof(NDIS_802_11_SSID));
					snprintf(tmpImg, sizeof(tmpImg), "<img src=\"/graphics/checkmrk.gif\">");
				}
				else
					snprintf(tmpImg, sizeof(tmpImg), "<img src=\"/graphics/uncheckmrk.gif\">");
				close(s);
			}
			else if (G_bRadio)
			{
				int tmp_auth, tmp_encry, tmp_defaultkeyid, tmp_networktype, tmp_preamtype, tmp_channel; //tmp_adhocmode,
				char tmp_wpapsk[65], tmp_key1[27], tmp_key2[27], tmp_key3[27], tmp_key4[27], tmp_bssid[13];
				char tmp_rtscheck=0, tmp_fragmentcheck=0;
				NDIS_802_11_RTS_THRESHOLD	tmp_rts;
				NDIS_802_11_FRAGMENTATION_THRESHOLD	tmp_fragment;
				NDIS_802_11_SSID			SSID;
				NDIS_802_11_POWER_MODE		tmp_psmode;

				memset(&SSID, 0x00, sizeof(SSID));
				bzero(tmp_bssid, sizeof(tmp_bssid));
				bzero(tmp_wpapsk, sizeof(tmp_wpapsk));
				bzero(tmp_key1, sizeof(tmp_key1));
				bzero(tmp_key2, sizeof(tmp_key2));
				bzero(tmp_key3, sizeof(tmp_key3));
				bzero(tmp_key4, sizeof(tmp_key4));
				memset(tmp_wpapsk, 0x00, sizeof(tmp_wpapsk));

				SSID.SsidLength = currentProfileSetting->SsidLen;
				memcpy(SSID.Ssid, (const void *)currentProfileSetting->SSID, currentProfileSetting->SsidLen);

				tmp_networktype = currentProfileSetting->NetworkType;
				tmp_auth  = currentProfileSetting->Authentication;
				tmp_encry = currentProfileSetting->Encryption;
				tmp_preamtype = currentProfileSetting->PreamType;
				tmp_rts = currentProfileSetting->RTS;
				tmp_rtscheck = currentProfileSetting->RTSCheck;
				tmp_fragment = currentProfileSetting->Fragment;
				tmp_fragmentcheck = currentProfileSetting->FragmentCheck;
				tmp_psmode = currentProfileSetting->PSmode;
				tmp_channel = currentProfileSetting->Channel;
				tmp_defaultkeyid = currentProfileSetting->KeyDefaultId;

				//strncpy(tmp_wpapsk, selectedProfileSetting->WpaPsk, 63);
				snprintf((char *)tmp_wpapsk, sizeof(tmp_wpapsk), "%s", currentProfileSetting->WpaPsk);
				strcpy(tmp_key1, (char *)currentProfileSetting->Key1);
				strcpy(tmp_key2, (char *)currentProfileSetting->Key2);
				strcpy(tmp_key3, (char *)currentProfileSetting->Key3);
				strcpy(tmp_key4, (char *)currentProfileSetting->Key4);

				unsigned char Bssid[6];
#ifdef WPA_SUPPLICANT_SUPPORT
				if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPA ||
						currentProfileSetting->Authentication == Ndis802_11AuthModeWPA2 ||
						currentProfileSetting->Authentication == Ndis802_11AuthModeMax )//802.1x
				{
					char tmp_key[27];
					if (tmp_defaultkeyid == 1) // 1~4
						strcpy(tmp_key, tmp_key1);
					else if (tmp_defaultkeyid == 2)
						strcpy(tmp_key, tmp_key2);
					else if (tmp_defaultkeyid == 3)
						strcpy(tmp_key, tmp_key3);
					else if (tmp_defaultkeyid == 4)
						strcpy(tmp_key, tmp_key4);

					conf_WPASupplicant((char *)currentProfileSetting->SSID, currentProfileSetting->KeyMgmt, currentProfileSetting->EAP, (char *)currentProfileSetting->Identity, (char *)currentProfileSetting->Password, (char *)currentProfileSetting->CACert, (char *)currentProfileSetting->ClientCert, (char *)currentProfileSetting->PrivateKey, (char *)currentProfileSetting->PrivateKeyPassword, tmp_key, currentProfileSetting->KeyDefaultId-1, currentProfileSetting->Encryption, currentProfileSetting->Tunnel, currentProfileSetting->Authentication);
				}
				else
#endif
					sta_connection(tmp_networktype, tmp_auth, tmp_encry, tmp_defaultkeyid, &SSID, Bssid, tmp_wpapsk, tmp_key1, tmp_key2, tmp_key3, tmp_key4, tmp_preamtype, tmp_rtscheck, tmp_rts, tmp_fragmentcheck, tmp_fragment, tmp_psmode, tmp_channel);

				/*NDIS_802_11_SSID SSID;
				  memset(&SSID, 0x00, sizeof(SSID));
				  strcpy((char *)SSID.Ssid ,(char *)currentProfileSetting->SSID);
				  SSID.SsidLength = strlen((char *)currentProfileSetting->SSID);
				  OidSetInformation(OID_802_11_SSID, s, "ra0", &SSID, sizeof(NDIS_802_11_SSID));*/
				snprintf(tmpImg, sizeof(tmpImg), "<img src=\"/graphics/uncheckmrk.gif\">");
			}
			else
				snprintf(tmpImg, sizeof(tmpImg), "<img src=\"/graphics/uncheckmrk.gif\">");
			close(s);
		}

		//web_debug_header(); //this function is called in html bt lighty SSI
		printf("<tr>");

		// Radio
		printf("<td><input type=radio name=selectedProfile value=%d onClick=\"selectedProfileChange()\">%s</td>",
				i+1, tmpImg);

		// Profile 
		printf("<td>%s</td>", currentProfileSetting->Profile);
		printf("<td>%s</td>", currentProfileSetting->SSID);

		// Channel
		if (currentProfileSetting->Channel <= 0)
			printf("<td>%s</td>", "Auto");
		else
			printf("<td>%d</td>", currentProfileSetting->Channel);

		// Auth
		if (currentProfileSetting->Authentication == Ndis802_11AuthModeOpen)
			printf("<td>%s</td>","OPEN");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeShared)
			printf("<td>%s</td>", "SHARED");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPAPSK)
			printf("<td>%s</td>", "WPA-PSK");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPA2PSK)
			printf("<td>%s</td>", "WPA2-PSK");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPANone)
			printf("<td>%s</td>", "WPA-NONE");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPA)
			printf("<td>%s</td>", "WPA");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeWPA2)
			printf("<td>%s</td>", "WPA2");
		else if (currentProfileSetting->Authentication == Ndis802_11AuthModeMax) //802.1x
			printf("<td>%s</td>", "OPEN");
		else
			printf("<td>%s</td>", "unknown");

		// Encryption
		if (currentProfileSetting->Encryption == Ndis802_11WEPEnabled)
			printf("<td>%s</td>", "WEP");
		else if (currentProfileSetting->Encryption == Ndis802_11WEPDisabled)
			printf("<td>%s</td>", "NONE");
		else if (currentProfileSetting->Encryption == Ndis802_11Encryption2Enabled)
			printf("<td>%s</td>", "TKIP");
		else if (currentProfileSetting->Encryption == Ndis802_11Encryption3Enabled)
			printf("<td>%s</td>", "AES");
		else
			printf("<td>%s</td>", "unknown");

		// NetworkType
		if (currentProfileSetting->NetworkType == Ndis802_11Infrastructure)
			printf("<td>%s</td>", "Infrastructure");
		else
			printf("<td>%s</td>", "Ad Hoc");

		printf("</tr>\n");
		currentProfileSetting = currentProfileSetting->Next;
		i++;
	} while (currentProfileSetting != NULL );
	return 0;
}

/*
 * arguments:   type - 1 ~ hmm
 * description: write selected profile data
 */
static int getStaProfileData(char *input)
{
	int type;
	int pIndex = 0;
	const char *value = NULL;

	initStaProfile(); //Landen: Lighty's CGI needs init every time.

	if (headerProfileSetting == NULL) {
		DBG_MSG("headerProfileSetting is NULL");
		return;
	}

	value = nvram_bufget(RT2860_NVRAM , "selectedProfile");
	DBG_MSG("Get from nvram: selectedProfile=%s", value); //Landen
	pIndex = atoi(value);
	if (pIndex <= 0) {
		DBG_MSG("selectedProfile(%d) is invalid", pIndex);
		return;
	}

	selectStaProfile(pIndex);
	
	value = web_get("Ajax", input, 0); //Send HTTP header when do Ajax
	if(strcmp(value, "") != 0)
		web_debug_header();

	if (selectedProfileSetting == NULL)
		return printf("0");

	value = web_get("type", input, 0);
	type = atoi(value);

	switch (type)
	{
		case 1: //profile name
			if (selectedProfileSetting->Profile == NULL)
				return printf("none");
			return printf("%s", selectedProfileSetting->Profile);
		case 2: //ssid
			if (selectedProfileSetting->SSID == NULL)
				return printf("none");
			return printf("%s", selectedProfileSetting->SSID);
		case 3: //network type
			return printf("%d", selectedProfileSetting->NetworkType);
		case 4: //power saving mode
			if (selectedProfileSetting->PSmode == Ndis802_11PowerModeCAM)
				return printf("0");
			return printf("1");
		case 5: //preamble type
			if (selectedProfileSetting->PreamType == Rt802_11PreambleAuto)
				return printf("0");
			return printf("1");
		case 6: //RTS check
			return printf("%d", selectedProfileSetting->RTSCheck);
		case 7: //RTS
			return printf("%d", selectedProfileSetting->RTS);
		case 8: //fragment check
			return printf("%d", selectedProfileSetting->FragmentCheck);
		case 9: //fragment
			return printf("%d", selectedProfileSetting->Fragment);
		case 10: //encryp
			return printf("%d", selectedProfileSetting->Encryption);
		case 11: //authentication
			return printf("%d", selectedProfileSetting->Authentication);
		case 12: //key1
			if (selectedProfileSetting->Key1 == NULL || strcmp(selectedProfileSetting->Key1, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->Key1);
		case 13: //key2
			if (selectedProfileSetting->Key2 == NULL || strcmp(selectedProfileSetting->Key2, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->Key2);
		case 14: //key3
			if (selectedProfileSetting->Key3 == NULL || strcmp(selectedProfileSetting->Key3, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->Key3);
		case 15: //key4
			if (selectedProfileSetting->Key4 == NULL || strcmp(selectedProfileSetting->Key4, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->Key4);
		case 16: //key1 type
			return printf("%d", selectedProfileSetting->Key1Type);
		case 17: //key2 type
			return printf("%d", selectedProfileSetting->Key2Type);
		case 18: //key3 type
			return printf("%d", selectedProfileSetting->Key3Type);
		case 19: //key4 type
			return printf("%d", selectedProfileSetting->Key4Type);
		case 20: //key1 length
			return printf("%d", selectedProfileSetting->Key1Length);
		case 21: //key2 length
			return printf("%d", selectedProfileSetting->Key2Length);
		case 22: //key3 length
			return printf("%d", selectedProfileSetting->Key3Length);
		case 23: //key4 length
			return printf("%d", selectedProfileSetting->Key4Length);
		case 24: //key default id
			return printf("%d", selectedProfileSetting->KeyDefaultId);
		case 25: //passphrase
			if (selectedProfileSetting->WpaPsk == NULL || strcmp(selectedProfileSetting->WpaPsk, "0") == 0)
				return printf("XDXD");
			return printf("%s", selectedProfileSetting->WpaPsk);
#ifdef WPA_SUPPLICANT_SUPPORT
		case 26: //key mgmt
			return printf("%d", selectedProfileSetting->KeyMgmt);
		case 27: //eap
			return printf("%d", selectedProfileSetting->EAP);
		case 28: //cert id
			if (selectedProfileSetting->Identity == NULL || strcmp(selectedProfileSetting->Identity, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->Identity);
		case 29: //ca cert
			if (selectedProfileSetting->CACert == NULL || strcmp(selectedProfileSetting->CACert, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->CACert);
		case 30: //client cert
			if (selectedProfileSetting->ClientCert == NULL || strcmp(selectedProfileSetting->ClientCert, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->ClientCert);
		case 31: //private key path
			if (selectedProfileSetting->PrivateKey == NULL || strcmp(selectedProfileSetting->PrivateKey, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->PrivateKey);
		case 32: //private key passwd
			if (selectedProfileSetting->PrivateKeyPassword == NULL || strcmp(selectedProfileSetting->PrivateKeyPassword, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->PrivateKeyPassword);
		case 33: //passwd
			if (selectedProfileSetting->Password == NULL || strcmp(selectedProfileSetting->Password, "0") == 0)
				return printf("");
			return printf("%s", selectedProfileSetting->Password);
		case 34: //tunnel
			return printf("%d", selectedProfileSetting->Tunnel);
#else
		case 26:
		case 27:
		case 34:
			return printf("0");
		case 28:
		case 29:
		case 30:
		case 31:
		case 32:
		case 33:
			return printf("");
#endif
		case 35: //channel
			return printf("%d", selectedProfileSetting->Channel);
	}
	return printf("unknown Profile Field");
}

/*
 * description: goform - add ampdu originator
 */
static void setStaProfile(char *input)
{
	PRT_PROFILE_SETTING	previousProfileSetting = NULL;
	int pIndex=0 , i=0;
	char *value;
	
	initStaProfile(); //Landen: Lighty's CGI needs init every time.

	if (headerProfileSetting == NULL) {
		DBG_MSG("headerProfileSetting is NULL");
		return;
	}

	value = web_get("selectedProfile", input, 0);
	pIndex = atoi(value);
	if (pIndex <= 0) {
		DBG_MSG("selectedProfile(%d) is invalid", pIndex);
		return;
	}
	nvram_bufset(RT2860_NVRAM, "selectedProfile", value);

	//selectedProfileSetting will be set.
	previousProfileSetting = selectStaProfile(pIndex);
	
	value = web_get("hiddenButton", input, 0);
	if (!strcmp(value, "edit")) {
		//do nothing
	}
	else if (!strcmp(value, "delete"))
	{
		if (selectedProfileSetting == headerProfileSetting) {
			if (headerProfileSetting->Next == NULL)
				selectedProfileSetting = headerProfileSetting = NULL;
			else
				headerProfileSetting = headerProfileSetting->Next;
			writeProfileToNvram();
		}
		else {
			if (previousProfileSetting != NULL && selectedProfileSetting != NULL) {
				previousProfileSetting->Next = selectedProfileSetting->Next;
				writeProfileToNvram();
			}
			selectedProfileSetting = NULL;
		}

		if (headerProfileSetting == NULL)
		{ 
			nvram_bufset(RT2860_NVRAM, "staProfile", "");
			nvram_bufset(RT2860_NVRAM, "staSSID", "");
			nvram_bufset(RT2860_NVRAM, "staNetworkType", "");
			nvram_bufset(RT2860_NVRAM, "staPSMode", "");
			nvram_bufset(RT2860_NVRAM, "staAdhocMode", "");
			nvram_bufset(RT2860_NVRAM, "staChannel", "");
			nvram_bufset(RT2860_NVRAM, "staPreamType", "");
			nvram_bufset(RT2860_NVRAM, "staRTSCheck", "");
			nvram_bufset(RT2860_NVRAM, "staFragmentCheck", "");
			nvram_bufset(RT2860_NVRAM, "staRTS", "");
			nvram_bufset(RT2860_NVRAM, "staFragment", "");
			nvram_bufset(RT2860_NVRAM, "staAuth", "");
			nvram_bufset(RT2860_NVRAM, "staEncrypt", "");
			nvram_bufset(RT2860_NVRAM, "staKeyDefaultId", "");
			nvram_bufset(RT2860_NVRAM, "staKey1Type", "");
			nvram_bufset(RT2860_NVRAM, "staKey2Type", "");
			nvram_bufset(RT2860_NVRAM, "staKey3Type", "");
			nvram_bufset(RT2860_NVRAM, "staKey4Type", "");
			nvram_bufset(RT2860_NVRAM, "staKey1Length", "");
			nvram_bufset(RT2860_NVRAM, "staKey2Length", "");
			nvram_bufset(RT2860_NVRAM, "staKey3Length", "");
			nvram_bufset(RT2860_NVRAM, "staKey4Length", "");
			nvram_bufset(RT2860_NVRAM, "staKey1", "");
			nvram_bufset(RT2860_NVRAM, "staKey2", "");
			nvram_bufset(RT2860_NVRAM, "staKey3", "");
			nvram_bufset(RT2860_NVRAM, "staKey4", "");
			nvram_bufset(RT2860_NVRAM, "staWpaPsk", "");
			nvram_bufset(RT2860_NVRAM, "sta8021xKeyMgmt", "");
			nvram_bufset(RT2860_NVRAM, "sta8021xEAP", "");
			nvram_bufset(RT2860_NVRAM, "sta8021xIdentity", "");
			nvram_bufset(RT2860_NVRAM, "sta8021xCACert", "");
			nvram_bufset(RT2860_NVRAM, "sta8021xClientCert", "");
			nvram_bufset(RT2860_NVRAM, "sta8021xPrivateKey", "");
			nvram_bufset(RT2860_NVRAM, "sta8021xPrivateKeyPassword", "");
			nvram_bufset(RT2860_NVRAM, "sta8021xPassword", "");
			nvram_bufset(RT2860_NVRAM, "sta8021xTunnel", "");
			nvram_bufset(RT2860_NVRAM, "staActive", "");
			nvram_commit(RT2860_NVRAM);
		}
	}
	else if (!strcmp(value, "activate"))
	{
		int tmp_auth, tmp_encry, tmp_defaultkeyid, tmp_networktype, tmp_preamtype, tmp_channel; //tmp_adhocmode,
		int s, ret;
		char tmp_wpapsk[65], tmp_key1[27], tmp_key2[27], tmp_key3[27], tmp_key4[27], tmp_bssid[13];
		char tmp_rtscheck=0, tmp_fragmentcheck=0;
		NDIS_802_11_RTS_THRESHOLD	tmp_rts;
		NDIS_802_11_FRAGMENTATION_THRESHOLD	tmp_fragment;
		NDIS_802_11_SSID			SSID;
		NDIS_802_11_POWER_MODE		tmp_psmode;

		currentProfileSetting = headerProfileSetting;
		do {
			currentProfileSetting->Active = 0;
			currentProfileSetting = currentProfileSetting->Next;
		} while (currentProfileSetting != NULL);

		selectedProfileSetting->Active = 1; // acivate
		//writeProfileToNvram();
		{
			char tmp_buffer[512] = {0}, tmp_data[8] = {0};
			currentProfileSetting = headerProfileSetting;
			do {
				sprintf(tmp_data, "%d", currentProfileSetting->Active);
				strcat(tmp_buffer, tmp_data);
				currentProfileSetting = currentProfileSetting->Next;
				if (currentProfileSetting != NULL)
					strcat(tmp_buffer, "\t");	
			} while (currentProfileSetting != NULL);
			nvram_bufset(RT2860_NVRAM, "staActive", tmp_buffer);
			nvram_commit(RT2860_NVRAM);
		}

		memset(&SSID, 0x00, sizeof(SSID));
		bzero(tmp_bssid, sizeof(tmp_bssid));
		bzero(tmp_wpapsk, sizeof(tmp_wpapsk));
		bzero(tmp_key1, sizeof(tmp_key1));
		bzero(tmp_key2, sizeof(tmp_key2));
		bzero(tmp_key3, sizeof(tmp_key3));
		bzero(tmp_key4, sizeof(tmp_key4));
		memset(tmp_wpapsk, 0x00, sizeof(tmp_wpapsk));

		SSID.SsidLength = selectedProfileSetting->SsidLen;
		memcpy(SSID.Ssid, (const void *)selectedProfileSetting->SSID, selectedProfileSetting->SsidLen);

		tmp_networktype = selectedProfileSetting->NetworkType;
		tmp_auth  = selectedProfileSetting->Authentication;
		tmp_encry = selectedProfileSetting->Encryption;
		tmp_preamtype = selectedProfileSetting->PreamType;
		tmp_rts = selectedProfileSetting->RTS;
		tmp_rtscheck = selectedProfileSetting->RTSCheck;
		tmp_fragment = selectedProfileSetting->Fragment;
		tmp_fragmentcheck = selectedProfileSetting->FragmentCheck;
		tmp_psmode = selectedProfileSetting->PSmode;
		tmp_channel = selectedProfileSetting->Channel;
		tmp_defaultkeyid = selectedProfileSetting->KeyDefaultId;

		sprintf(tmp_wpapsk, "%s", selectedProfileSetting->WpaPsk);
		strcpy(tmp_key1, (char *)selectedProfileSetting->Key1);
		strcpy(tmp_key2, (char *)selectedProfileSetting->Key2);
		strcpy(tmp_key3, (char *)selectedProfileSetting->Key3);
		strcpy(tmp_key4, (char *)selectedProfileSetting->Key4);

		s = socket(AF_INET, SOCK_DGRAM, 0);
		//step 1: OID_802_11_INFRASTRUCTURE_MODE
		ret = OidSetInformation(OID_802_11_INFRASTRUCTURE_MODE, s, "ra0", &tmp_networktype, sizeof(int));
		if (ret < 0)
			fprintf(stderr, "Set OID_802_11_INFRASTRUCTURE_MODE has error =%d, networktype=%d\n", ret, tmp_networktype);
		close(s);

		unsigned char Bssid[6];
		//activate
#ifdef WPA_SUPPLICANT_SUPPORT				
		if (selectedProfileSetting->Authentication == Ndis802_11AuthModeWPA ||
				selectedProfileSetting->Authentication == Ndis802_11AuthModeWPA2 ||
				selectedProfileSetting->Authentication == Ndis802_11AuthModeMax )//802.1x
		{
			char tmp_key[27];
			if (tmp_defaultkeyid == 1) // 1~4
				strcpy(tmp_key, tmp_key1);
			else if (tmp_defaultkeyid == 2)
				strcpy(tmp_key, tmp_key2);
			else if (tmp_defaultkeyid == 3)
				strcpy(tmp_key, tmp_key3);
			else if (tmp_defaultkeyid == 4)
				strcpy(tmp_key, tmp_key4);

			conf_WPASupplicant((char*)selectedProfileSetting->SSID, selectedProfileSetting->KeyMgmt, selectedProfileSetting->EAP, (char*)selectedProfileSetting->Identity, (char*)selectedProfileSetting->Password, (char*)selectedProfileSetting->CACert, (char*)selectedProfileSetting->ClientCert, (char*)selectedProfileSetting->PrivateKey, (char*)selectedProfileSetting->PrivateKeyPassword, tmp_key, selectedProfileSetting->KeyDefaultId-1, selectedProfileSetting->Encryption, selectedProfileSetting->Tunnel, selectedProfileSetting->Authentication);
		}
		else
#endif
			sta_connection(tmp_networktype, tmp_auth, tmp_encry, tmp_defaultkeyid, &SSID, Bssid, tmp_wpapsk, tmp_key1, tmp_key2, tmp_key3, tmp_key4, tmp_preamtype, tmp_rtscheck, tmp_rts, tmp_fragmentcheck, tmp_fragment, tmp_psmode, tmp_channel);

		// Set SSID
		/*memset(&SSID, 0x00, sizeof(NDIS_802_11_SSID));
		  SSID.SsidLength = selectedProfileSetting->SsidLen;
		  memcpy(SSID.Ssid, (const void *)selectedProfileSetting->SSID, selectedProfileSetting->SsidLen);
		  */
		//Active_flag = 1; //Landen: unused
#if defined CONFIG_USB
		//initUSB(); //Landen: why need it?
#endif
	}
	else {
		DBG_MSG("hiddenButton(%s) is invalid", value);
		return;
	}
	//websRedirect(wp, "station/profile.asp"); //Landen: goahead
	//web_redirect(getenv("HTTP_REFERER"));
	web_redirect("/station/profile.shtml");
}

/*
 * description: goform - reset statistics counters
 */
static void editStaProfile(char *input)
{
	const char *value;
	int pIndex;

	initStaProfile();

	if (headerProfileSetting == NULL) {
		DBG_MSG("headerProfileSetting is NULL");
		return;
	}
	
	value = nvram_bufget(RT2860_NVRAM , "selectedProfile");
	DBG_MSG("Get from nvram: selectedProfile=%s", value); //Landen
	pIndex = atoi(value);
	if (pIndex <= 0) {
		DBG_MSG("selectedProfile(%d) is invalid", pIndex);
		return;
	}

	selectStaProfile(pIndex); //set selectedProfileSetting

	// step 1, modify info on selectedProfileSetting

	value = web_get("profile_name", input, 0);
	if (strcmp(value, "") != 0)
		strcpy((char *)selectedProfileSetting->Profile, value);

	value = web_get("Ssid", input, 0);
	if (strcmp(value, "") != 0)
		strcpy((char *)selectedProfileSetting->SSID, value);
	DBG_MSG("selectedProfileSetting->SSID=%s", selectedProfileSetting->SSID); //Landen

	value = web_get("network_type", input, 0);
	if (strcmp(value, "") != 0)
		selectedProfileSetting->NetworkType = atoi(value);
	if (selectedProfileSetting->NetworkType == Ndis802_11Infrastructure) {
		selectedProfileSetting->AdhocMode = 0;
		selectedProfileSetting->Channel = 0;
		selectedProfileSetting->PreamType = Rt802_11PreambleLong;
	}

	value = web_get("power_saving_mode", input, 0);
	if (strcmp(value, "") != 0) {
		if (strcmp(value, "0") == 0)
			selectedProfileSetting->PSmode = Ndis802_11PowerModeCAM;
		else
			selectedProfileSetting->PSmode = Ndis802_11PowerModeMAX_PSP;
	}

	value = web_get("channel", input, 0);
	if (strcmp(value, "") != 0) {
		if (selectedProfileSetting->NetworkType == Ndis802_11IBSS)
			selectedProfileSetting->Channel = atoi(value);
		else
			selectedProfileSetting->Channel = 0;
	}

	value = web_get("b_premable_type", input, 0);
	if (strcmp(value, "") != 0) {
		if (strcmp(value, "0") == 0)
			selectedProfileSetting->PreamType = Rt802_11PreambleAuto;
		else
			selectedProfileSetting->PreamType = Rt802_11PreambleLong;
	}

	//if (websCompareVar(wp, T("rts_threshold"), T("on"))) {
	value = web_get("rts_threshold", input, 0);
	if (!strcmp(value, "on")) {
		selectedProfileSetting->RTSCheck = 1;
		value = web_get("rts_thresholdvalue", input, 0);
		if (strcmp(value, "") != 0)
			selectedProfileSetting->RTS = atoi(value);
	}
	else {
		selectedProfileSetting->RTSCheck = 0;
		selectedProfileSetting->RTS = 2347;
	}

	//if (websCompareVar(wp, T("fragment_threshold"), T("on"))) {
	value = web_get("fragment_threshold", input, 0);
	if (!strcmp(value, "on")) {
		selectedProfileSetting->FragmentCheck = 1;
		value = web_get("fragment_thresholdvalue", input, 0);
		if (strcmp(value, "") != 0)
			selectedProfileSetting->Fragment = atoi(value);
	}
	else {
		selectedProfileSetting->FragmentCheck = 0;
		selectedProfileSetting->Fragment = 2346;
	}

	value = web_get("security_infra_mode", input, 0);
	if (strcmp(value, "") != 0)
		selectedProfileSetting->Authentication = atoi(value);
	value = web_get("security_adhoc_mode", input, 0);
	if (strcmp(value, "") != 0)
		selectedProfileSetting->Authentication = atoi(value);

#ifdef WPA_SUPPLICANT_SUPPORT
	if ( selectedProfileSetting->Authentication == Ndis802_11AuthModeWPA
			|| selectedProfileSetting->Authentication == Ndis802_11AuthModeWPA2)
	{
		selectedProfileSetting->KeyMgmt = Rtwpa_supplicantKeyMgmtWPAEAP;
	}
	else if (selectedProfileSetting->Authentication == Ndis802_11AuthModeMax) //802.1x
		selectedProfileSetting->KeyMgmt = Rtwpa_supplicantKeyMgmtIEEE8021X;
	else 
		selectedProfileSetting->KeyMgmt = Rtwpa_supplicantKeyMgmtNONE;
#endif

	value = web_get("wep_key_1", input, 0);
	if(strcmp(value, "") == 0)
		strcpy((char *)selectedProfileSetting->Key1, "0");
	else
		strcpy((char *)selectedProfileSetting->Key1, value);
	value = web_get("wep_key_2", input, 0);
	if(strcmp(value, "") == 0)
		strcpy((char *)selectedProfileSetting->Key2, "0");
	else
		strcpy((char *)selectedProfileSetting->Key2, value);
	value = web_get("wep_key_3", input, 0);
	if(strcmp(value, "") == 0)
		strcpy((char *)selectedProfileSetting->Key3, "0");
	else
		strcpy((char *)selectedProfileSetting->Key3, value);
	value = web_get("wep_key_4", input, 0);
	if(strcmp(value, "") == 0)
		strcpy((char *)selectedProfileSetting->Key4, "0");
	else
		strcpy((char *)selectedProfileSetting->Key4, value);

	value = web_get("wep_key_entry_method", input, 0);
	if (strcmp(value, "") != 0) {
		selectedProfileSetting->Key1Type =
			selectedProfileSetting->Key1Type =
			selectedProfileSetting->Key1Type =
			selectedProfileSetting->Key1Type =
			atoi(value);
	}

	value = web_get("wep_key_length", input, 0);
	if (strcmp(value, "") != 0) {
		selectedProfileSetting->Key1Length =
			selectedProfileSetting->Key2Length =
			selectedProfileSetting->Key3Length =
			selectedProfileSetting->Key4Length =
			atoi(value);
	}

	value = web_get("wep_default_key", input, 0);
	if (strcmp(value, "") != 0)
		selectedProfileSetting->KeyDefaultId = atoi(value);

	value = web_get("cipher", input, 0);
	if (strcmp(value, "") != 0) {
		if (0 == atoi(value)) //TKIP
			selectedProfileSetting->Encryption = Ndis802_11Encryption2Enabled;
		else //AES
			selectedProfileSetting->Encryption = Ndis802_11Encryption3Enabled;
	}

	value = web_get("passphrase", input, 0);
	if (strcmp(value, "") != 0)
		strcpy((char *)selectedProfileSetting->WpaPsk, value);
	else
		strcpy((char *)selectedProfileSetting->WpaPsk, "0");

#ifdef WPA_SUPPLICANT_SUPPORT
	value = web_get("cert_auth_type_from_1x", input, 0);
	if (strcmp(value, "") != 0)
		selectedProfileSetting->EAP = atoi(value);
	value = web_get("cert_auth_type_from_wpa", input, 0);
	if (strcmp(value, "") != 0)
		selectedProfileSetting->EAP = atoi(value);

	value = web_get("cert_tunnel_auth_peap", input, 0);
	if (strcmp(value, "") != 0)
		selectedProfileSetting->Tunnel = atoi(value);
	value = web_get("cert_tunnel_auth_ttls", input, 0);
	if (strcmp(value, "") != 0)
		selectedProfileSetting->Tunnel = atoi(value);

	value = web_get("cert_id", input, 0);
	if (strcmp(value, "") != 0)
		strcpy((char *)selectedProfileSetting->Identity, value);
	else
		strcpy((char *)selectedProfileSetting->Identity, "0");

	value = web_get("cert_password", input, 0);
	if (strcmp(value, "") != 0)
		strcpy((char *)selectedProfileSetting->Password, value);
	else
		strcpy((char *)selectedProfileSetting->Password, "0");

	value = web_get("cert_client_cert_path", input, 0);
	char *ret = strstr(value, "\r");
	if (ret > 0)
		ret[0] = '\0';
	if (strcmp(value, "") != 0) {
		strcpy((char *)selectedProfileSetting->ClientCert, value);

		value = web_get("cert_private_key_path", input, 0);
		ret = strstr(value, "\r");
		if (ret > 0)
			ret[0] = '\0';
		if (strcmp(value, "") != 0)
			strcpy((char *)selectedProfileSetting->PrivateKey, value);
		else
			strcpy((char *)selectedProfileSetting->PrivateKey, "0");

		value = web_get("cert_private_key_password", input, 0);
		if (strcmp(value, "") != 0)
			strcpy((char *)selectedProfileSetting->PrivateKeyPassword, value);
		else
			strcpy((char *)selectedProfileSetting->PrivateKeyPassword, "0");
	}
	else {
		strcpy((char *)selectedProfileSetting->ClientCert, "0");
		strcpy((char *)selectedProfileSetting->PrivateKey, "0");
		strcpy((char *)selectedProfileSetting->PrivateKeyPassword, "0");
	}

	value = web_get("cert_ca_cert_path", input, 0);
	if (strcmp(value, "") != 0)
		strcpy((char *)selectedProfileSetting->CACert, value);
	else
		strcpy((char *)selectedProfileSetting->CACert, "0");
#endif				

	if (selectedProfileSetting->Authentication <= Ndis802_11AuthModeShared) {
		if( strlen((char *)selectedProfileSetting->Key1) > 1 || strlen((char *)selectedProfileSetting->Key2) > 1 ||
			strlen((char *)selectedProfileSetting->Key4) > 1 || strlen((char *)selectedProfileSetting->Key3) > 1)
		{
			selectedProfileSetting->Encryption = Ndis802_11WEPEnabled;
		}
		else
			selectedProfileSetting->Encryption = Ndis802_11WEPDisabled;
	}
	else if (selectedProfileSetting->Authentication == Ndis802_11AuthModeMax) //802.1x
		selectedProfileSetting->Encryption = Ndis802_11WEPEnabled;

	if (selectedProfileSetting->Active)
	{
		NDIS_802_11_SSID Ssid;
		memset(&Ssid, 0x00, sizeof(NDIS_802_11_SSID));
		strcpy((char *)Ssid.Ssid ,(char *)selectedProfileSetting->SSID);
		Ssid.SsidLength = selectedProfileSetting->SsidLen;

		unsigned char Bssid[6];
#ifdef WPA_SUPPLICANT_SUPPORT
		if (selectedProfileSetting->Authentication == Ndis802_11AuthModeWPA ||
			selectedProfileSetting->Authentication == Ndis802_11AuthModeWPA2 ||
			selectedProfileSetting->Authentication == Ndis802_11AuthModeMax )//802.1x
		{
			char tmp_key[27];
			if (selectedProfileSetting->KeyDefaultId == 1) // 1~4
				strcpy(tmp_key, (char *)selectedProfileSetting->Key1);
			else if (selectedProfileSetting->KeyDefaultId == 2)
				strcpy(tmp_key, (char *)selectedProfileSetting->Key2);
			else if (selectedProfileSetting->KeyDefaultId == 3)
				strcpy(tmp_key, (char *)selectedProfileSetting->Key3);
			else if (selectedProfileSetting->KeyDefaultId == 4)
				strcpy(tmp_key, (char *)selectedProfileSetting->Key4);

			conf_WPASupplicant((char*)selectedProfileSetting->SSID, selectedProfileSetting->KeyMgmt, selectedProfileSetting->EAP, (char*)selectedProfileSetting->Identity, (char*)selectedProfileSetting->Password, (char*)selectedProfileSetting->CACert, (char*)selectedProfileSetting->ClientCert, (char*)selectedProfileSetting->PrivateKey, (char*)selectedProfileSetting->PrivateKeyPassword, tmp_key, selectedProfileSetting->KeyDefaultId-1, selectedProfileSetting->Encryption, selectedProfileSetting->Tunnel, selectedProfileSetting->Tunnel);
		}
		else
#endif
		sta_connection(selectedProfileSetting->NetworkType, selectedProfileSetting->Authentication, selectedProfileSetting->Encryption, selectedProfileSetting->KeyDefaultId, &Ssid, Bssid, (char *)selectedProfileSetting->WpaPsk, (char *)selectedProfileSetting->Key1, (char *)selectedProfileSetting->Key2, (char *)selectedProfileSetting->Key3, (char *)selectedProfileSetting->Key4, selectedProfileSetting->PreamType, selectedProfileSetting->RTSCheck, selectedProfileSetting->RTS, selectedProfileSetting->FragmentCheck, selectedProfileSetting->Fragment, selectedProfileSetting->PSmode, selectedProfileSetting->Channel);

		//Active_flag = 1;
		sleep(1);
	}

	// setp 2, write all profile into nvram
	writeProfileToNvram();

	selectedProfileSetting = NULL;
}

/*
 * description: goform - add station profile
 */
static void addStaProfile(char *input)
{
	RT_PROFILE_SETTING  tmpProfileSetting;
	int  securitymode=-1;
	char tmp_buffer[512];
	const char *wordlist = NULL;
	char *value, *ret;

	DBG_MSG("\n%s <-----", __FUNCTION__); //Landen
	memset(&tmpProfileSetting, 0x00, sizeof(RT_PROFILE_SETTING));
	tmpProfileSetting.Next = NULL;

	//profile name
	// TODO: to tell profile is duplication with other
	value = web_get("profile_name", input, 0);
	if (strlen(value) <= 0) {
		printf("No profile name given!\n");
		return;
	}
	strcpy((char *)tmpProfileSetting.Profile, value);
	wordlist = nvram_bufget(RT2860_NVRAM, "staProfile");
	if (wordlist && strcmp(wordlist,"") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, value);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", value);
	nvram_bufset(RT2860_NVRAM, "staProfile", tmp_buffer);

	//ssid
	value = web_get("Ssid", input, 0);
	//fprintf(stderr, "%s: SSID=%s\n", __func__, value);
	strcpy((char *)tmpProfileSetting.SSID, value);
	tmpProfileSetting.SsidLen = strlen((char *)tmpProfileSetting.SSID);
	wordlist = nvram_bufget(RT2860_NVRAM, "staSSID");
	if (wordlist && strcmp(wordlist,"") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, value);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", value);
	nvram_bufset(RT2860_NVRAM, "staSSID", tmp_buffer);

	//network type
	value = web_get("network_type", input, 0);
	tmpProfileSetting.NetworkType = atoi(value);
	wordlist = nvram_bufget(RT2860_NVRAM, "staNetworkType");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.NetworkType);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.NetworkType);
	nvram_bufset(RT2860_NVRAM, "staNetworkType", tmp_buffer);

	//Adhoc mode
	if (tmpProfileSetting.NetworkType == Ndis802_11Infrastructure)
		tmpProfileSetting.AdhocMode = 0;
	else
		tmpProfileSetting.AdhocMode = 1;
	wordlist = nvram_bufget(RT2860_NVRAM, "staAdhocMode");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.AdhocMode);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.AdhocMode);
	nvram_bufset(RT2860_NVRAM, "staAdhocMode", tmp_buffer);

	//power saving mode
	value = web_get("power_saving_mode", input, 0);
	if (wordlist && strcmp(value, "0") ==0) //CAM
		tmpProfileSetting.PSmode = Ndis802_11PowerModeCAM;
	else
		tmpProfileSetting.PSmode = Ndis802_11PowerModeMAX_PSP;
	wordlist = nvram_bufget(RT2860_NVRAM, "staPSMode");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.PSmode);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.PSmode);
	nvram_bufset(RT2860_NVRAM, "staPSMode", tmp_buffer);

	//channel
	value = web_get("channel", input, 0);
	DBG_MSG("channel='%s'", value); //Landen
	if (strcmp(value, "") == 0){
		//strcpy(value, "0");
		strlcpy(value, "0", DATALEN);
	}
	tmpProfileSetting.Channel = atoi(value);
	wordlist = nvram_bufget(RT2860_NVRAM, "staChannel");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Channel);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Channel);
	nvram_bufset(RT2860_NVRAM, "staChannel", tmp_buffer);

	//b preamble type
	value = web_get("b_premable_type", input, 0);
	if (strcmp(value, "") == 0 || strcmp(value, "0") == 0)
		tmpProfileSetting.PreamType = Rt802_11PreambleAuto;
	else
		tmpProfileSetting.PreamType = Rt802_11PreambleLong;
	wordlist = nvram_bufget(RT2860_NVRAM, "staPreamType");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.PreamType);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.PreamType);
	nvram_bufset(RT2860_NVRAM, "staPreamType", tmp_buffer);

	//rts threshold value
	//if (websCompareVar(wp, T("rts_threshold"), T("on"))) { //Landen
	if(!strcmp(web_get("rts_threshold", input, 0), "on")){
		tmpProfileSetting.RTSCheck = 1;
		value = web_get("rts_thresholdvalue", input, 0);
		if(strlen(value) > 0)
			tmpProfileSetting.RTS = atoi(value);
		else
			tmpProfileSetting.RTS = 2347; 
	} else {
		tmpProfileSetting.RTSCheck = 0;
		tmpProfileSetting.RTS = 2347;
	}
	wordlist = nvram_bufget(RT2860_NVRAM, "staRTSCheck");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.RTSCheck);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.RTSCheck);
	nvram_bufset(RT2860_NVRAM, "staRTSCheck", tmp_buffer);

	wordlist = nvram_bufget(RT2860_NVRAM, "staRTS");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.RTS);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.RTS);
	nvram_bufset(RT2860_NVRAM, "staRTS", tmp_buffer);

	//fragment threshold value
	//if (websCompareVar(wp, T("fragment_threshold"), T("on"))) { //Landen
	if(!strcmp(web_get("fragment_threshold", input, 0), "on")){
		tmpProfileSetting.FragmentCheck = 1;
		value = web_get("fragment_thresholdvalue", input, 0);
		if(strlen(value) > 0 )
			tmpProfileSetting.Fragment = atoi(value);
		else
			tmpProfileSetting.Fragment = 2346;
	} else {
		tmpProfileSetting.FragmentCheck = 0;
		tmpProfileSetting.Fragment = 2346;
	}
	wordlist = nvram_bufget(RT2860_NVRAM, "staFragmentCheck");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.FragmentCheck);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.FragmentCheck);
	nvram_bufset(RT2860_NVRAM, "staFragmentCheck", tmp_buffer);

	wordlist = nvram_bufget(RT2860_NVRAM, "staFragment");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Fragment);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Fragment);
	nvram_bufset(RT2860_NVRAM, "staFragment", tmp_buffer);

	//security policy (security_infra_mode or security_adhoc_mode)
	value = web_get("security_infra_mode", input, 0);
	if (strcmp(value, "") != 0)
		securitymode = atoi(value);
	value = web_get("security_adhoc_mode", input, 0);
	if (strcmp(value, "") != 0)
		securitymode = atoi(value);

	tmpProfileSetting.Authentication = securitymode;
	wordlist = nvram_bufget(RT2860_NVRAM, "staAuth");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Authentication);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Authentication);
	nvram_bufset(RT2860_NVRAM, "staAuth", tmp_buffer);

#ifdef WPA_SUPPLICANT_SUPPORT
	if (tmpProfileSetting.Authentication == Ndis802_11AuthModeWPA
			|| tmpProfileSetting.Authentication == Ndis802_11AuthModeWPA2)
	{
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtWPAEAP;
	}
	else if (tmpProfileSetting.Authentication == Ndis802_11AuthModeMax) //802.1x
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtIEEE8021X;
	else 
		tmpProfileSetting.KeyMgmt = Rtwpa_supplicantKeyMgmtNONE;

	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xKeyMgmt");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.KeyMgmt);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.KeyMgmt);
	nvram_bufset(RT2860_NVRAM, "sta8021xKeyMgmt", tmp_buffer);
#endif

	//wep key 1
	value = web_get("wep_key_1", input, 0);
	if (strcmp(value, "") == 0)
		strcpy((char *)tmpProfileSetting.Key1, "0");
	else
		strcpy((char *)tmpProfileSetting.Key1, value);
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey1");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.Key1);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.Key1);
	nvram_bufset(RT2860_NVRAM, "staKey1", tmp_buffer);

	//wep key 2
	value = web_get("wep_key_2", input, 0);
	if (strcmp(value, "") == 0)
		strcpy((char *)tmpProfileSetting.Key2, "0");
	else
		strcpy((char *)tmpProfileSetting.Key2, value);
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey2");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.Key2);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.Key2);
	nvram_bufset(RT2860_NVRAM, "staKey2", tmp_buffer);

	//wep key 3
	value = web_get("wep_key_3", input, 0);
	if (strcmp(value, "") == 0)
		strcpy((char *)tmpProfileSetting.Key3, "0");
	else
		strcpy((char *)tmpProfileSetting.Key3, value);
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey3");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.Key3);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.Key3);
	nvram_bufset(RT2860_NVRAM, "staKey3", tmp_buffer);

	//wep key 4
	value = web_get("wep_key_4", input, 0);
	if (strcmp(value, "") == 0)
		strcpy((char *)tmpProfileSetting.Key4, "0");
	else
		strcpy((char *)tmpProfileSetting.Key4, value);
	wordlist = nvram_bufget(RT2860_NVRAM, "staKey4");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.Key4);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.Key4);
	nvram_bufset(RT2860_NVRAM, "staKey4", tmp_buffer);

	//wep key entry method
	value = web_get("wep_key_entry_method", input, 0);
	tmpProfileSetting.Key1Type = tmpProfileSetting.Key2Type =
		tmpProfileSetting.Key3Type = tmpProfileSetting.Key4Type =
		atoi(value);

	wordlist = nvram_bufget(RT2860_NVRAM, "staKey1Type");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Key1Type);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Key1Type);
	nvram_bufset(RT2860_NVRAM, "staKey1Type", tmp_buffer);

	wordlist = nvram_bufget(RT2860_NVRAM, "staKey2Type");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Key2Type);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Key2Type);
	nvram_bufset(RT2860_NVRAM, "staKey2Type", tmp_buffer);

	wordlist = nvram_bufget(RT2860_NVRAM, "staKey3Type");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Key3Type);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Key3Type);
	nvram_bufset(RT2860_NVRAM, "staKey3Type", tmp_buffer);

	wordlist = nvram_bufget(RT2860_NVRAM, "staKey4Type");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Key4Type);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Key4Type);
	nvram_bufset(RT2860_NVRAM, "staKey4Type", tmp_buffer);

	//wep key length
	value = web_get("wep_key_length", input, 0);
	tmpProfileSetting.Key1Length = tmpProfileSetting.Key2Length = 
		tmpProfileSetting.Key3Length = tmpProfileSetting.Key4Length =
		atoi(value);

	wordlist = nvram_bufget(RT2860_NVRAM, "staKey1Length");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Key1Length);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Key1Length);
	nvram_bufset(RT2860_NVRAM, "staKey1Length", tmp_buffer);

	wordlist = nvram_bufget(RT2860_NVRAM, "staKey2Length");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Key2Length);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Key2Length);
	nvram_bufset(RT2860_NVRAM, "staKey2Length", tmp_buffer);

	wordlist = nvram_bufget(RT2860_NVRAM, "staKey3Length");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Key3Length);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Key3Length);
	nvram_bufset(RT2860_NVRAM, "staKey3Length", tmp_buffer);

	wordlist = nvram_bufget(RT2860_NVRAM, "staKey4Length");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Key4Length);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Key4Length);
	nvram_bufset(RT2860_NVRAM, "staKey4Length", tmp_buffer);

	//wep default key
	value = web_get("wep_default_key", input, 0);
	tmpProfileSetting.KeyDefaultId= atoi(value);
	wordlist = nvram_bufget(RT2860_NVRAM, "staKeyDefaultId");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.KeyDefaultId);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.KeyDefaultId);
	nvram_bufset(RT2860_NVRAM, "staKeyDefaultId", tmp_buffer);

	//cipher, "staEncrypt"
	value = web_get("cipher", input, 0);
	if (strcmp(value, "0") == 0) //TKIP
		tmpProfileSetting.Encryption = Ndis802_11Encryption2Enabled;
	else if (strcmp(value, "1") == 0) //AES
		tmpProfileSetting.Encryption = Ndis802_11Encryption3Enabled;
	else { //empty
		if (tmpProfileSetting.Authentication <= Ndis802_11AuthModeShared) {
			if (strlen((char *)tmpProfileSetting.Key1) > 1 || strlen((char *)tmpProfileSetting.Key2) > 1 ||
					strlen((char *)tmpProfileSetting.Key4) > 1 || strlen((char *)tmpProfileSetting.Key3) > 1)
			{
				tmpProfileSetting.Encryption = Ndis802_11WEPEnabled;
			}
			else
				tmpProfileSetting.Encryption = Ndis802_11WEPDisabled;
		}
		else if (tmpProfileSetting.Authentication == Ndis802_11AuthModeMax) //802.1x
			tmpProfileSetting.Encryption = Ndis802_11WEPEnabled;
		else
			tmpProfileSetting.Encryption = Ndis802_11WEPDisabled;
	}
	wordlist = nvram_bufget(RT2860_NVRAM, "staEncrypt");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Encryption);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Encryption);
	nvram_bufset(RT2860_NVRAM, "staEncrypt", tmp_buffer);

	//passphrase
	value = web_get("passphrase", input, 0);
	if(strcmp(value, "") == 0)
		strcpy((char *)tmpProfileSetting.WpaPsk, "0");
	else
		strcpy((char *)tmpProfileSetting.WpaPsk, value);
	wordlist = nvram_bufget(RT2860_NVRAM, "staWpaPsk");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.WpaPsk);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.WpaPsk);
	nvram_bufset(RT2860_NVRAM, "staWpaPsk", tmp_buffer);

#ifdef WPA_SUPPLICANT_SUPPORT
	//cert auth from 1x, wpa
	tmpProfileSetting.EAP = Rtwpa_supplicantEAPNONE;
	value = web_get("cert_auth_type_from_1x", input, 0);
	if (strcmp(value, "") != 0)
		tmpProfileSetting.EAP = (RT_WPA_SUPPLICANT_EAP)atoi(value);
	value = web_get("cert_auth_type_from_wpa", input, 0);
	if (strcmp(value, "") != 0)
		tmpProfileSetting.EAP = (RT_WPA_SUPPLICANT_EAP)atoi(value);

	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xEAP");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.EAP);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.EAP);
	nvram_bufset(RT2860_NVRAM, "sta8021xEAP", tmp_buffer);

	//cert tunnel auth peap, ttls
	tmpProfileSetting.Tunnel = Rtwpa_supplicantTUNNENONE;
	value = web_get("cert_tunnel_auth_peap", input, 0);
	if (strcmp(value, "") != 0)
		tmpProfileSetting.Tunnel = (RT_WPA_SUPPLICANT_TUNNEL)atoi(value);
	value = web_get("cert_tunnel_auth_ttls", input, 0);
	if (strcmp(value, "") != 0)
		tmpProfileSetting.Tunnel = (RT_WPA_SUPPLICANT_TUNNEL)atoi(value);

	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xTunnel");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Tunnel);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Tunnel);
		nvram_bufset(RT2860_NVRAM, "sta8021xTunnel", tmp_buffer);

	//certificate identity
	value = web_get("cert_id", input, 0);
	if(strcmp(value, "") == 0)
		sprintf((char *)tmpProfileSetting.Identity, "%s", "0");
	else
		sprintf((char *)tmpProfileSetting.Identity, "%s", value);
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xIdentity");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.Identity);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.Identity);
	nvram_bufset(RT2860_NVRAM, "sta8021xIdentity", tmp_buffer);

	//certificate password
	value = web_get("cert_password", input, 0);
	if(strcmp(value, "") == 0)
		sprintf((char *)tmpProfileSetting.Password, "%s", "0");
	else
		sprintf((char *)tmpProfileSetting.Password, "%s", value);
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xPassword");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.Password);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.Password);
	nvram_bufset(RT2860_NVRAM, "sta8021xPassword", tmp_buffer);

	//client certificate path
	value = strdup(web_get("cert_client_cert_path", input, 0));
	if(strcmp(value, "") == 0){
		sprintf((char *)tmpProfileSetting.ClientCert, "%s", "0");
	} else {
		ret = strstr(value, "\r");
		if (ret > 0)
			ret[0] = '\0';
		sprintf((char *)tmpProfileSetting.ClientCert, "%s", value);
	}
	free(value);
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xClientCert");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.ClientCert);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.ClientCert);
	nvram_bufset(RT2860_NVRAM, "sta8021xClientCert", tmp_buffer);

	//private key path
	value = strdup(web_get("cert_private_key_path", input, 0));
	if(strcmp(value, "") == 0){
		sprintf((char *)tmpProfileSetting.PrivateKey, "%s", "0");
	} else {
		ret = strstr(value, "\r");
		if (ret > 0)
			ret[0] = '\0';
		sprintf((char *)tmpProfileSetting.PrivateKey, "%s", value);
	}
	free(value);
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xPrivateKey");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.PrivateKey);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.PrivateKey);
	nvram_bufset(RT2860_NVRAM, "sta8021xPrivateKey", tmp_buffer);

	//private key password
	value = web_get("cert_private_key_password", input, 0);
	if(strcmp(value, "") == 0)
		sprintf((char *)tmpProfileSetting.PrivateKeyPassword, "%s", "0");
	else
		sprintf((char *)tmpProfileSetting.PrivateKeyPassword, "%s", value);
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xPrivateKeyPassword");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.PrivateKeyPassword);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.PrivateKeyPassword);
	nvram_bufset(RT2860_NVRAM, "sta8021xPrivateKeyPassword", tmp_buffer);

	//CA cert path
	value = web_get("cert_ca_cert_path", input, 0);
	if(strcmp(value, "") == 0)
		sprintf((char *)tmpProfileSetting.CACert, "%s", "0");
	else
		sprintf((char *)tmpProfileSetting.CACert, "%s", value);
	wordlist = nvram_bufget(RT2860_NVRAM, "sta8021xCACert");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, tmpProfileSetting.CACert);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", tmpProfileSetting.CACert);
	nvram_bufset(RT2860_NVRAM, "sta8021xCACert", tmp_buffer);
#else
#define BUFSET(column, value) \
	wordlist = nvram_bufget(RT2860_NVRAM, column); \
	if (wordlist && strcmp(wordlist, "") != 0) \
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%s", wordlist, value); \
	else \
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s", value); \
	nvram_bufset(RT2860_NVRAM, column, tmp_buffer);

	BUFSET("sta8021xEAP", "7");
	BUFSET("sta8021xTunnel", "3");
	BUFSET("sta8021xKeyMgmt", "3");
	BUFSET("sta8021xIdentity", "0");
	BUFSET("sta8021xPassword", "0");
	BUFSET("sta8021xClientCert", "0");
	BUFSET("sta8021xPrivateKey", "0");
	BUFSET("sta8021xPrivateKeyPassword", "0");
	BUFSET("sta8021xCACert", "0");
#endif		

	//write into /etc/rt61sta.ui
	//writeProfileToFile(&tmpProfileSetting);

	tmpProfileSetting.Active = 0;
	wordlist = nvram_bufget(RT2860_NVRAM, "staActive");
	if (wordlist && strcmp(wordlist, "") != 0)
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%s\t%d", wordlist, tmpProfileSetting.Active);
	else
		snprintf(tmp_buffer, sizeof(tmp_buffer), "%d", tmpProfileSetting.Active);
	nvram_bufset(RT2860_NVRAM, "staActive", tmp_buffer);

	nvram_commit(RT2860_NVRAM);

	freeHeaderProfileSettings();
	headerProfileSetting = NULL;
	initStaProfile();
}

/*
 * description: goform - set advanced configuration
 */
static void setStaAdvance(char *input)
{
	char *w_mode, *cr_bg, *cr_a, *bg_prot, *rate, *burst;
//#ifdef RT2860_STA_TXBF_SUPPORT
#if defined (RT2860_STA_SUPPORT) && defined (RT2860_TXBF_SUPPORT)
	char	*txbf;
#endif
	char *ht, *bw, *gi, *mcs, *rf;

	int ret;
	unsigned char radio_status=0;

  web_debug_header();

//	w_mode = websGetVar(wp, T("wireless_mode"), T("0"));
	w_mode = strdup(web_get("wireless_mode", input, 1));
	//cr_bg = websGetVar(wp, T("country_region_bg"), T("0"));
	cr_bg = strdup(web_get("country_region_bg", input, 1));
	//cr_a = websGetVar(wp, T("country_region_a"), T("0"));
	cr_a = strdup(web_get("country_region_a", input, 1));	
	//bg_prot = websGetVar(wp, T("bg_protection"), T("0"));
	bg_prot = strdup(web_get("bg_protection", input, 1));	
	//rate = websGetVar(wp, T("tx_rate"), T("0"));
	rate = strdup(web_get("tx_rate", input, 1));	
	//burst = websGetVar(wp, T("tx_burst"), T("off"));
	burst = strdup(web_get("tx_burst", input, 1));	
//#ifdef RT2860_STA_TXBF_SUPPORT
#if defined (RT2860_STA_SUPPORT) && defined (RT2860_TXBF_SUPPORT)
	//txbf = websGetVar(wp, T("txbf"), T("0"));
	txbf = strdup(web_get("txbf", input, 1));	
#endif
	//ht = websGetVar(wp, T("n_mode"), T("0"));
	ht = strdup(web_get("n_mode", input, 1));
	//bw = websGetVar(wp, T("n_bandwidth"), T("0"));
	bw = strdup(web_get("n_bandwidth", input, 1));	
	//gi = websGetVar(wp, T("n_gi"), T("0"));
	gi = strdup(web_get("n_gi", input, 1));
	//mcs = websGetVar(wp, T("n_mcs"), T("0"));
	mcs = strdup(web_get("n_mcs", input, 1));
	//rf = websGetVar(wp, T("radiohiddenButton"), T("2"));
  rf = strdup(web_get("radiohiddenButton", input, 1));
  
	radio_status = atoi(rf);
	if (radio_status < 2) {
		int s = socket(AF_INET, SOCK_DGRAM, 0);
		OidSetInformation(RT_OID_802_11_RADIO, s, "ra0", &radio_status, sizeof(radio_status));
		if (radio_status == 1) {
			nvram_bufset(RT2860_NVRAM, "RadioOff", "0");
			G_bRadio = 1;
			ret = OidSetInformation(OID_802_11_BSSID_LIST_SCAN, s, "ra0", 0, 0);
			if (ret < 0)
				fprintf(stderr, "%s: Set OID_802_11_BSSID_LIST_SCAN error = %d \n", __func__, ret);
	
			Sleep(3);
			if (G_SSID.SsidLength > 0) {
				ret = OidSetInformation(OID_802_11_SSID, s, "ra0", &G_SSID, sizeof(NDIS_802_11_SSID));
				if (ret < 0)
					fprintf(stderr, "%s: Set OID_802_11_SSID error = %d \n", __func__, ret);
					//error(E_L, E_LOG, T("Set OID_802_11_SSID error = %d"), ret);
			} 
			/*
			else {
				ret = OidSetInformation(OID_802_11_BSSID, s, "ra", &G_Bssid, 6);
				if (ret < 0)
					error(E_L, E_LOG, T("Set OID_802_11_BSSID error = %d"), ret);
			}
			*/
		} else {
			nvram_bufset(RT2860_NVRAM, "RadioOff", "1");
			G_bRadio = 0;
		}
		close(s);
		nvram_commit(RT2860_NVRAM);
	//	websRedirect(wp, "station/advance.asp");
	  web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	nvram_bufset(RT2860_NVRAM, "WirelessMode", w_mode);
	nvram_bufset(RT2860_NVRAM, "CountryRegion", cr_bg);
	nvram_bufset(RT2860_NVRAM, "CountryRegionABand", cr_a);
	nvram_bufset(RT2860_NVRAM, "BGProtection", bg_prot);
	nvram_bufset(RT2860_NVRAM, "TxRate", rate);
	if (!strncmp(burst, "on", 3))
		nvram_bufset(RT2860_NVRAM, "TxBurst", "1");
	else
		nvram_bufset(RT2860_NVRAM, "TxBurst", "0");
//#ifdef RT2860_STA_TXBF_SUPPORT
#if defined (RT2860_STA_SUPPORT) && defined (RT2860_TXBF_SUPPORT)
	if (!strncmp(txbf, "2", 2)) {
		nvram_bufset(RT2860_NVRAM, "ETxBfEnCond", "1");
		nvram_bufset(RT2860_NVRAM, "ETxBfIncapable", "0");
		nvram_bufset(RT2860_NVRAM, "ITxBfEn", "0");
	}
	else {
		nvram_bufset(RT2860_NVRAM, "ETxBfEnCond", "0");
		nvram_bufset(RT2860_NVRAM, "ETxBfIncapable", "1");
		nvram_bufset(RT2860_NVRAM, "ITxBfEn", "0");
	}
#endif // TXBF //
	nvram_bufset(RT2860_NVRAM, "HT_OpMode", ht);
	nvram_bufset(RT2860_NVRAM, "HT_BW", bw);
	nvram_bufset(RT2860_NVRAM, "HT_GI", gi);
	nvram_bufset(RT2860_NVRAM, "HT_MCS", mcs);
	nvram_commit(RT2860_NVRAM);

	do_system("ifconfig ra0 down; ralink_init gen 2860; ifconfig ra0 up");
	do_system("iwpriv ra0 set EthConvertMode=dongle");

	if (-1 != initStaProfile())
		initStaConnection();

	/* restart wan.sh if needed (renew dhcp, pppoe etc) */
	const char *opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (strcmp(opmode, "0"))
	{
		do_system("wan.sh");
	}

	//websRedirect(wp,"station/advance.asp");
	web_redirect(getenv("HTTP_REFERER"));
	
	
//#ifdef RT2860_STA_TXBF_SUPPORT	
#if defined (RT2860_STA_SUPPORT) && defined (RT2860_TXBF_SUPPORT)
	free_all(12, w_mode, cr_bg, cr_a, bg_prot, rate, burst, txbf, ht, bw, gi, mcs, rf);
#else
	free_all(11, w_mode, cr_bg, cr_a, bg_prot, rate, burst, ht, bw, gi, mcs, rf);
#endif
	return;
}

/*
 * description: goform - set 11n configuration
 */
void setSta11nCfg(char *input)
{
	char *a_mpdu_enable, *autoBA, *mpdu_density, *a_msdu_enable;
	int policy;
	int s;
	OID_BACAP_STRUC BACap;

	web_debug_header();

	//a_mpdu_enable = websGetVar(wp, T("a_mpdu_enable"), T("off"));
	a_mpdu_enable = strdup(web_get("a_mpdu_enable", input, 1));
	
	//autoBA = websGetVar(wp, T("autoBA"), T("0"));
	a_mpdu_enable = strdup(web_get("autoBA", input, 1));
	
	//mpdu_density = websGetVar(wp, T("mpdu_density"), T("0"));
	mpdu_density = strdup(web_get("mpdu_density", input, 1));
		
	//a_msdu_enable = websGetVar(wp, T("a_msdu_enable"), T("off"));
	a_msdu_enable = strdup(web_get("a_msdu_enable", input, 1));
	
	if (!strcmp(a_mpdu_enable, "on")) {
		policy = 1;
		nvram_bufset(RT2860_NVRAM, "staPolicy", "1"); //FIXME: typo?
	}
	else {
		policy = 0;
		nvram_bufset(RT2860_NVRAM, "staPolicy", "0");
	}
	nvram_bufset(RT2860_NVRAM, "HT_AutoBA", autoBA);
	nvram_bufset(RT2860_NVRAM, "HT_MpduDensity", mpdu_density);
	nvram_bufset(RT2860_NVRAM, "HT_AMSDU", strcmp(a_msdu_enable, "off")? "1":"0");
	nvram_commit(RT2860_NVRAM);

	s = socket(AF_INET, SOCK_DGRAM, 0);
	OidQueryInformation(RT_OID_802_11_QUERY_IMME_BA_CAP, s, "ra0", &BACap, sizeof(BACap));
	BACap.Policy = policy;
	BACap.AutoBA = atoi(autoBA);
	BACap.MpduDensity = atoi(mpdu_density);
	if (!strcmp(a_msdu_enable, "on"))
		BACap.AmsduEnable = 1;

	OidSetInformation(RT_OID_802_11_SET_IMME_BA_CAP, s, "ra0", &BACap, sizeof(BACap));
	close(s);

	//debug print
	//websHeader(wp);
	//websWrite(wp, T("<h3>11n configuration</h3><br>\n"));
	//websWrite(wp, T("a_mpdu_enable: %s<br>\n"), a_mpdu_enable);
	//websWrite(wp, T("autoBA: %s<br>\n"), autoBA);
	//websWrite(wp, T("mpdu_density: %s<br>\n"), mpdu_density);
	//websWrite(wp, T("a_msdu_enable: %s<br>\n"), a_msdu_enable);
	//websFooter(wp);
	//websDone(wp, 200);
	free_all(4, a_mpdu_enable, a_mpdu_enable, mpdu_density, a_msdu_enable);
}

/*
 * description: goform - make the station connect to the AP with given SSID
 */
static void setStaConnect(char *input)
{
	int  tmp_auth=0, tmp_encry=0, tmp_defaultkeyid=0, tmp_networktype=0;
	char *tmp_ssid, *tmp_wpapsk, *tmp_key1, *tmp_key2, *tmp_key3, *tmp_key4, *tmp_bssid;
#ifdef WPA_SUPPLICANT_SUPPORT
	int  tmp_keymgmt = Rtwpa_supplicantKeyMgmtNONE, tmp_eap = Rtwpa_supplicantEAPNONE, tmp_tunnel = Rtwpa_supplicantTUNNENONE;
	char *tmp_identity, *tmp_cacert, *tmp_clientcert, *tmp_privatekey, *tmp_privatekeypassword, *tmp_password;
#endif 
	char *value;

	DBG_MSG("\n%s <-----", __FUNCTION__); //Landen
	tmp_auth  = Ndis802_11AuthModeOpen;
	tmp_encry = Ndis802_11WEPDisabled;

	//ssid, networktype, bssid
	tmp_ssid = strdup(web_get("Ssid", input, 0));
	value = web_get("network_type", input, 0);
	tmp_networktype = atoi(value);
	tmp_bssid = strdup(web_get("bssid", input, 0));

	//security mode
	value = web_get("security_infra_mode", input, 0);
	if (strcmp(value, "") != 0)
		tmp_auth = atoi(value);
	value = web_get("security_adhoc_mode", input, 0);
	if (strcmp(value, "") != 0)
		tmp_auth = atoi(value);
#ifdef WPA_SUPPLICANT_SUPPORT
	//key management
	if (tmp_auth == Ndis802_11AuthModeWPA || tmp_auth == Ndis802_11AuthModeWPA2)
		tmp_keymgmt = Rtwpa_supplicantKeyMgmtWPAEAP;
	else if (tmp_auth == Ndis802_11AuthModeMax) //802.1x
		tmp_keymgmt= Rtwpa_supplicantKeyMgmtIEEE8021X;
	else 
		tmp_keymgmt = Rtwpa_supplicantKeyMgmtNONE;
#endif

	//wep key1~4
	tmp_key1 = strdup(web_get("wep_key_1", input, 0));
	tmp_key2 = strdup(web_get("wep_key_2", input, 0));
	tmp_key3 = strdup(web_get("wep_key_3", input, 0));
	tmp_key4 = strdup(web_get("wep_key_4", input, 0));
	if (strcmp(tmp_key1, "") || strcmp(tmp_key2, "") || strcmp(tmp_key3, "")
			|| strcmp(tmp_key4, ""))
	{
		// Auth mode OPEN might use encryption type: none or wep
		// if set wep key, the encry must be WEPEnable
		tmp_encry = Ndis802_11WEPEnabled;
	}

	//default key
	value = web_get("wep_default_key", input, 0);
	tmp_defaultkeyid = atoi(value);

	//cipher
	value = web_get("cipher", input, 0);
	if (strcmp(value, "") != 0) {
		int enc = atoi(value);
		if (enc == 0) //TKIP
			tmp_encry= Ndis802_11Encryption2Enabled;
		else //AES
			tmp_encry = Ndis802_11Encryption3Enabled;
	}

	//passphrase
	tmp_wpapsk = strdup(web_get("passphrase", input, 0));
DBG_MSG("[DBG]passphrase='%s'", tmp_wpapsk);

#ifdef WPA_SUPPLICANT_SUPPORT
	//eap
	value = web_get("cert_auth_type_from_1x", input, 0);
	if (strcmp(value, "") != 0)
		tmp_eap = (RT_WPA_SUPPLICANT_EAP)atoi(value);
	value = web_get("cert_auth_type_from_wpa", input, 0);
	if (strcmp(value, "") != 0)
		tmp_eap = (RT_WPA_SUPPLICANT_EAP)atoi(value);

	//tunnel
	value = web_get("cert_tunnel_auth_peap", input, 0);
	if (strcmp(value, "") != 0)
		tmp_tunnel = (RT_WPA_SUPPLICANT_TUNNEL)atoi(value);
	value = web_get("cert_tunnel_auth_ttls", input, 0);
	if (strcmp(value, "") != 0)
		tmp_tunnel = (RT_WPA_SUPPLICANT_TUNNEL)atoi(value);

	//certificate
	tmp_identity = strdup(web_get("cert_id", input, 0));
	tmp_password = strdup(web_get("cert_password", input, 0));
	tmp_clientcert = strdup(web_get("cert_client_cert_path", input, 0));
	tmp_privatekey = strdup(web_get("cert_private_key_path", input, 0));
	tmp_privatekeypassword = strdup(web_get("cert_private_key_password", input, 0));
	tmp_cacert = strdup(web_get("cert_ca_cert_path", input, 0));
#endif

	//encryp
	if (tmp_auth <= Ndis802_11AuthModeShared)
	{
		if (strlen((char *)tmp_key1) > 1 || strlen((char *)tmp_key2) > 1 ||
				strlen((char *)tmp_key3) > 1 || strlen((char *)tmp_key4) > 1)
		{
			tmp_encry= Ndis802_11WEPEnabled;
		}
		else
			tmp_encry = Ndis802_11WEPDisabled;
	}
	else if (tmp_auth == Ndis802_11AuthModeMax) //802.1x
		tmp_encry = Ndis802_11WEPEnabled;

	DBG_MSG("tmp_encry=%d", tmp_encry); //Landen

	RT_802_11_PREAMBLE                      tmp_preamtype = Rt802_11PreambleAuto;
	NDIS_802_11_RTS_THRESHOLD               tmp_rts = MAX_RTS_THRESHOLD;
	NDIS_802_11_FRAGMENTATION_THRESHOLD     tmp_fragment = 2346;
	NDIS_802_11_POWER_MODE                  tmp_psmode = Ndis802_11PowerModeCAM;
	NDIS_802_11_SSID			SSID;
	unsigned char				Bssid[6];
	int					s = socket(AF_INET, SOCK_DGRAM, 0);

	OidQueryInformation(RT_OID_802_11_PREAMBLE, s, "ra0", &tmp_preamtype, sizeof(RT_802_11_PREAMBLE));
	OidQueryInformation(OID_802_11_POWER_MODE, s, "ra0", &tmp_psmode, sizeof(NDIS_802_11_POWER_MODE));
	OidQueryInformation(OID_802_11_RTS_THRESHOLD, s, "ra0", &tmp_rts, sizeof(NDIS_802_11_RTS_THRESHOLD));
	OidQueryInformation(OID_802_11_FRAGMENTATION_THRESHOLD, s, "ra0", &tmp_fragment, sizeof(NDIS_802_11_FRAGMENTATION_THRESHOLD));
	// Set SSID
	DBG_MSG("tmp_ssid='%s'", tmp_ssid); //Landen
	memset(&SSID, 0x00, sizeof(NDIS_802_11_SSID));
	SSID.SsidLength = strlen(tmp_ssid);
	memcpy(SSID.Ssid, (const void *)tmp_ssid, SSID.SsidLength);
	DBG_MSG("SSID.Ssid='%s', SSID.SsidLength=%d", SSID.Ssid, SSID.SsidLength);//Landen

	// Set BSSID
	memset(Bssid, 0x00, sizeof(Bssid));
	AtoH(tmp_bssid, Bssid, 6);

	//site_survey_connect
#ifdef WPA_SUPPLICANT_SUPPORT
	if (tmp_auth == Ndis802_11AuthModeWPA ||
			tmp_auth == Ndis802_11AuthModeWPA2 ||
			tmp_auth == Ndis802_11AuthModeMax )//802.1x
	{
		char tmp_key[27];
		if (tmp_defaultkeyid == 1) // 1~4
			strcpy(tmp_key, tmp_key1);
		else if (tmp_defaultkeyid == 2)
			strcpy(tmp_key, tmp_key2);
		else if (tmp_defaultkeyid == 3)
			strcpy(tmp_key, tmp_key3);
		else if (tmp_defaultkeyid == 4)
			strcpy(tmp_key, tmp_key4);

		tmp_defaultkeyid -=1;
		conf_WPASupplicant(tmp_ssid, tmp_keymgmt, tmp_eap, tmp_identity, tmp_password, tmp_cacert, tmp_clientcert, tmp_privatekey, tmp_privatekeypassword, tmp_key, tmp_defaultkeyid, tmp_encry, tmp_tunnel, tmp_auth);
	}
	else
#endif
		sta_connection(tmp_networktype, tmp_auth, tmp_encry, tmp_defaultkeyid, &SSID, Bssid, tmp_wpapsk, tmp_key1, tmp_key2, tmp_key3, tmp_key4, tmp_preamtype, 0, tmp_rts, 0, tmp_fragment, tmp_psmode, 0);  //tmp_channel 0 is auto.

#if defined CONFIG_USB
	//initUSB(); //TODO why need it?
	//init_usb(); //in lighttpd usb.cgi
#endif
#ifdef WPA_SUPPLICANT_SUPPORT
	free_all(13, tmp_ssid, tmp_bssid, tmp_wpapsk, 
		 tmp_identity, tmp_password, tmp_clientcert,
		 tmp_privatekey, tmp_privatekeypassword, tmp_cacert, 
		 tmp_key1, tmp_key2, tmp_key3, tmp_key4);
#else
	free_all(7, tmp_ssid, tmp_bssid, tmp_wpapsk, 
		 tmp_key1, tmp_key2, tmp_key3, tmp_key4);
#endif
	DBG_MSG("%s ----->\n", __FUNCTION__); //Landen
}

/*
 * description: goform - set G_bdBm_ischeck (displaying dbm or % type)
 */
static void setStaDbm(char *input)
{
	char *dbm;
char    G_bdBm_ischeck = 0; //false
	//dbm = websGetVar(wp, T("dbmChecked"), T("off"));
	  web_debug_header();
	dbm = strdup(web_get("dbmChecked", input, 1));
	if (!strncmp(dbm, "on", 3))
		G_bdBm_ischeck = 1;
	else
		G_bdBm_ischeck = 0;
		
	if (!strcmp(dbm, "1")) {
		
		nvram_bufset(RT2860_NVRAM, "G_bdBm_ischeck", "1"); //FIXME: typo?
	}
	else {

		nvram_bufset(RT2860_NVRAM, "G_bdBm_ischeck", "0");
	}
	nvram_commit(RT2860_NVRAM);		
	DBG_MSG("[DBG]%s", getenv("HTTP_REFERER"));

	//websRedirect(wp, "station/link_status.asp");
	web_redirect(getenv("HTTP_REFERER"));
	return;
}

static void STFs(int nvram, int index, char *flash_key, char *value)
{
	char *tmp = (char *) nvram_bufget(nvram, flash_key);
	if(!tmp)
		tmp = "";
	nvram_bufset(nvram, flash_key, set_nth_value(index, tmp, value));
	return ;
}

/*
 * description: goform - set station QoS parameters - wmm, dls setup, tear down
 */
static void setStaQoS(char *input)
{
	int s;
	char *button;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	//button = websGetVar(wp, T("button_type"), T("0"));
	  web_debug_header();
	button = strdup(web_get("button_type", input, 1));
	if (!strncmp(button, "0", 2)) {
		close(s);
		printf("unrecognized button type");
		//websError(wp, 500, "unrecognized button type");
		return;
	}
	else if (!strncmp(button, "1", 2)) {
		unsigned long apsd;
		NDIS_802_11_SSID Ssid;
		int wmm_en, ps_en, acbe, acbk, acvi, acvo;

		//wmm_en = websCompareVar(wp, T("wmm_enable"), T("on"));
		if(!strcmp(web_get("wmm_enable", input, 1), "on"))
    	wmm_en = 1;
		else
    	wmm_en = 0; 

		//ps_en = websCompareVar(wp, T("wmm_ps_enable"), T("on"));
		if(!strcmp(web_get("wmm_ps_enable", input, 1), "on"))
    	ps_en = 1;
		else
    	ps_en = 0; 
    	
		//acbe = websCompareVar(wp, T("wmm_ps_mode_acbe"), T("on"));
		if(!strcmp(web_get("wmm_ps_mode_acbe", input, 1), "on"))
    	acbe = 1;
		else
    	acbe = 0; 
    	
		//acbk = websCompareVar(wp, T("wmm_ps_mode_acbk"), T("on"));
		if(!strcmp(web_get("wmm_ps_mode_acbk", input, 1), "on"))
    	acbk = 1;
		else
    	acbk = 0; 
    	
		//acvi = websCompareVar(wp, T("wmm_ps_mode_acvi"), T("on"));
		if(!strcmp(web_get("wmm_ps_mode_acvi", input, 1), "on"))
    	acvi = 1;
		else
    	acvi = 0; 
    	
		//acvo = websCompareVar(wp, T("wmm_ps_mode_acvo"), T("on"));
		if(!strcmp(web_get("wmm_ps_mode_acvo", input, 1), "on"))
    	acvo = 1;
		else
    	acvo = 0; 
    	
		nvram_bufset(RT2860_NVRAM, "WmmCapable", wmm_en? "1":"0");
		nvram_bufset(RT2860_NVRAM, "APSDCapable", ps_en? "1":"0");
		STFs(RT2860_NVRAM, 0, "AckPolicy", acbe? "1":"0");
		STFs(RT2860_NVRAM, 1, "AckPolicy", acbk? "1":"0");
		STFs(RT2860_NVRAM, 2, "AckPolicy", acvi? "1":"0");
		STFs(RT2860_NVRAM, 3, "AckPolicy", acvo? "1":"0");
		nvram_commit(RT2860_NVRAM);

		if (wmm_en) {
			OidQueryInformation(RT_OID_802_11_QUERY_APSD_SETTING, s, "ra0", &apsd, sizeof(apsd));
			if (ps_en) {
				apsd |= 0x00000001;
				if (acbk)
					apsd |= 0x00000002;
				if (acbe)
					apsd |= 0x00000004;
				if (acvi)
					apsd |= 0x00000008;
				if (acvo)
					apsd |= 0x00000010;
				apsd &= 0x0000007F;  //set apsd bit be zero (xxxxxxx1)
			}
			else
				apsd &= 0x0000007E;  //set apsd bit be zero (xxxxxxx0)
		}
		else
			apsd &= 0x0000007E;  //set apsd bit be zero (xxxxxxx0)

		OidSetInformation(RT_OID_802_11_SET_APSD_SETTING, s, "ra0", &apsd, sizeof(apsd));
		OidSetInformation(RT_OID_802_11_SET_WMM, s, "ra0", &wmm_en, sizeof(wmm_en));

		OidQueryInformation(OID_802_11_SSID, s, "ra0", &Ssid, sizeof(Ssid));
		OidSetInformation(OID_802_11_DISASSOCIATE, s, "ra0", NULL, 0);
		Sleep(1);
		OidSetInformation(OID_802_11_SSID, s, "ra0", &Ssid, sizeof(Ssid));
	}
	else if (!strncmp(button, "2", 2)) {
		int dls_en;
		char *mac0, *mac1, *mac2, *mac3, *mac4, *mac5;
		RT_802_11_DLS_UI dls;
		unsigned char mac[6];

		//dls_en = websCompareVar(wp, T("wmm_dls_enable"), T("on"));
		if(!strcmp(web_get("wmm_dls_enable", input, 1), "on"))
    	dls_en = 1;
		else
    	dls_en = 0; 
    	
		//mac0 = websGetVar(wp, T("mac0"), T("0"));
		//mac1 = websGetVar(wp, T("mac1"), T("1"));
		//mac2 = websGetVar(wp, T("mac2"), T("2"));
		//mac3 = websGetVar(wp, T("mac3"), T("3"));
		//mac4 = websGetVar(wp, T("mac4"), T("4"));
		//mac5 = websGetVar(wp, T("mac5"), T("5"));
		
		mac0 = strdup(web_get("mac0", input, 1));
		mac1 = strdup(web_get("mac1", input, 1));
		mac2 = strdup(web_get("mac2", input, 1));
		mac3 = strdup(web_get("mac3", input, 1));
		mac4 = strdup(web_get("mac4", input, 1));
		mac5 = strdup(web_get("mac5", input, 1));
		dls.MacAddr[0] = (unsigned char)strtoul(mac0, (char **)NULL, 16);
		dls.MacAddr[1] = (unsigned char)strtoul(mac1, (char **)NULL, 16);
		dls.MacAddr[2] = (unsigned char)strtoul(mac2, (char **)NULL, 16);
		dls.MacAddr[3] = (unsigned char)strtoul(mac3, (char **)NULL, 16);
		dls.MacAddr[4] = (unsigned char)strtoul(mac4, (char **)NULL, 16);
		dls.MacAddr[5] = (unsigned char)strtoul(mac5, (char **)NULL, 16);
		//dls.TimeOut = atoi(websGetVar(wp, T("timeout"), T("0")));
    dls.TimeOut = atoi(strdup(web_get("timeout", input, 1)));
		if (dls.MacAddr[0] == 0 && dls.MacAddr[1] == 0 && dls.MacAddr[2] == 0 &&
				dls.MacAddr[3] == 0 && dls.MacAddr[4] == 0 && dls.MacAddr[5] == 0) {
			close(s);
			//websError(wp, 500, "invalid DLS MAC address (00s)");
			printf("invalid DLS MAC address (00s)");
			return;
		}
		if (dls.MacAddr[0] == 0xff && dls.MacAddr[1] == 0xff && dls.MacAddr[2] == 0xff &&
				dls.MacAddr[3] == 0xff && dls.MacAddr[4] == 0xff && dls.MacAddr[5] == 0xff) {
			close(s);
			//websError(wp, 500, "invalid DLS MAC address (FFs)");
			printf("invalid DLS MAC address (FFs)");
			return;
		}
		if (OidQueryInformation(OID_802_3_CURRENT_ADDRESS, s, "ra0", &mac, sizeof(mac)) >= 0) {
			if (dls.MacAddr[0] == mac[0] && dls.MacAddr[1] == mac[1] &&
					dls.MacAddr[2] == mac[2] && dls.MacAddr[3] == mac[3] &&
					dls.MacAddr[4] == mac[4] && dls.MacAddr[5] == mac[5]) {
				close(s);
				//websError(wp, 500, "invalid DLS MAC address");
				printf("invalid DLS MAC address");
				return;
			}
		}
		nvram_bufset(RT2860_NVRAM, "DLSCapable", dls_en? "1":"0");

		dls.Valid = 1;
		OidSetInformation(RT_OID_802_11_SET_DLS, s, "ra0", &dls_en, sizeof(unsigned long));
		OidSetInformation(RT_OID_802_11_SET_DLS_PARAM, s, "ra0", &dls, sizeof(dls));
	}
	else if (!strncmp(button, "3", 2)) {
		RT_802_11_DLS_INFO dls_info;
		int s_dls;


		//s_dls = atoi(websGetVar(wp, T("selected_dls"), T("0")));
		s_dls  = atoi(strdup(web_get("selected_dls", input, 1)));
		if (s_dls != 0) {
			OidQueryInformation(RT_OID_802_11_QUERY_DLS_PARAM, s, "ra0", &dls_info, sizeof(dls_info));
			if (dls_info.Entry[s_dls-1].Valid == 1) {
				dls_info.Entry[s_dls-1].Valid = 0;
				dls_info.Entry[s_dls-1].Status = DLS_NONE;
				OidSetInformation(RT_OID_802_11_SET_DLS_PARAM, s, "ra0", &dls_info.Entry[s_dls-1], sizeof(RT_802_11_DLS_UI));
			}
		}
	}
	close(s);
	sleep(1);

	web_redirect(getenv("HTTP_REFERER"));
	//websRedirect(wp, "station/qos.asp");
}

/*
 * description: goform - reset statistics counters
 */
static void resetStaCounters(char *input)
{
	int s;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	OidSetInformation(RT_OID_802_11_RESET_COUNTERS, s, "ra0", 0, 0);
	close(s);
	web_redirect(getenv("HTTP_REFERER"));
	//websRedirect(wp, "station/statistics.asp");
	return;
}

static void updateCert(char *input)
{
	char *query = web_get("query", input, 0);
	char file[128];
	FILE *pp;
	
	sleep(2);
	if (!strcmp(query, "key"))
		pp = popen("nvram_get cert KeyCertFile", "r");
	else if (!strcmp(query, "client"))
		pp = popen("nvram_get cert CACLCertFile", "r");
	else if (!strcmp(query, "as"))
		pp = popen("nvram_get wapi ASCertFile", "r");
	else if (!strcmp(query, "user"))
		pp = popen("nvram_get wapi UserCertFile", "r");
	else
		return;

	if (!pp) {
		DBG_MSG("Certificate update error");
		return;
	}
	memset(file, 0, 128);
	fscanf(pp, "%s", file);
	pclose(pp);

	//websWrite(wp, T("HTTP/1.1 200 OK\nContent-type: text/plain\nPragma: no-cache\nCache-Control: no-cache\n\n"));
	web_debug_header_no_cache();	

	if (strlen(file) > 0)
		printf("%s", file);
	else
		printf("");

	return;
}

#ifdef WPA_SUPPLICANT_SUPPORT
static void exec_WPASupplicant(char* ssid, NDIS_802_11_WEP_STATUS encryp, NDIS_802_11_AUTHENTICATION_MODE auth, RT_WPA_SUPPLICANT_KEY_MGMT keymgmt, int keyidx, char* wepkey)
{
	// auth mode
	int s, ret;
	unsigned char wpa_supplicant_support = 2, ieee8021x_support = 1;
	NDIS_802_11_SSID Ssid;

	system("killall wpa_supplicant");
	sleep(1);
	//fprintf(stderr, "exec_WPASupplicant()\n");
	memset(&Ssid, 0x00, sizeof(NDIS_802_11_SSID));
	strcpy((char *)Ssid.Ssid ,ssid);
	Ssid.SsidLength = strlen(ssid);

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (auth == Ndis802_11AuthModeMax)
		auth = Ndis802_11AuthModeOpen;

	if (keymgmt == Rtwpa_supplicantKeyMgmtNONE)
	{
		wpa_supplicant_support = 0;
		ieee8021x_support = 0;

		ret = OidSetInformation(OID_802_11_SET_IEEE8021X, s, "ra0", &ieee8021x_support, sizeof(ieee8021x_support));
		if (ret < 0) {
			fprintf(stderr, "Set OID_802_11_SET_IEEE8021X has error =%d, ieee8021x_support=%d\n", ret, ieee8021x_support);
			close(s);
			return;
		}

		ret = OidSetInformation(RT_OID_WPA_SUPPLICANT_SUPPORT, s, "ra0", &wpa_supplicant_support, sizeof(wpa_supplicant_support));
		if (ret < 0) {
			fprintf(stderr, "Set RT_OID_WPA_SUPPLICANT_SUPPORT has error =%d, wpa_supplicant_support=%d\n", ret, wpa_supplicant_support);
			fprintf(stderr, "Please check the driver configuration whether support WAP_SUPPORT!!");
			close(s);
			return;
		}
	}
	else
	{
		ret = OidSetInformation(OID_802_11_SET_IEEE8021X, s, "ra0", &ieee8021x_support, sizeof(ieee8021x_support));
		if (ret < 0) {
			fprintf(stderr, "Set OID_802_11_SET_IEEE8021X has error =%d, ieee8021x_support=%d\n", ret, ieee8021x_support);
			close(s);
			return;
		}

		ret = OidSetInformation(RT_OID_WPA_SUPPLICANT_SUPPORT, s, "ra0", &wpa_supplicant_support, sizeof(wpa_supplicant_support));
		if (ret < 0) {
			fprintf(stderr, "Set RT_OID_WPA_SUPPLICANT_SUPPORT has error =%d, wpa_supplicant_support=%d\n", ret, wpa_supplicant_support);
			fprintf(stderr, "Please check the driver configuration whether support WAP_SUPPORT!!");
			close(s);
			return;
		}
	}

	ret = OidSetInformation(OID_802_11_AUTHENTICATION_MODE, s, "ra0", &auth, sizeof(auth));
	if (ret < 0) {
		fprintf(stderr, "Set OID_802_11_AUTHENTICATION_MODE has error =%d, auth=%d\n", ret, auth);
		close(s);
		return;
	}

	// encryp mode
	ret = OidSetInformation(OID_802_11_ENCRYPTION_STATUS, s, "ra0", &encryp, sizeof(encryp));
	if (ret < 0) {
		fprintf(stderr, "Set OID_802_11_ENCRYPTION_STATUS has error =%d, encry=%d\n", ret, encryp);
		close(s);
		return;
	}

	if (encryp == Ndis802_11WEPEnabled)
	{
		PNDIS_802_11_WEP	pWepKey = NULL;
		unsigned long		lBufLen;
		int 				nKeyLen;

		nKeyLen = strlen(wepkey);
		if (nKeyLen == 0)
		{
			NDIS_802_11_REMOVE_KEY	removeKey;
			int j=0;
			removeKey.Length = sizeof(NDIS_802_11_REMOVE_KEY);
			removeKey.KeyIndex = keyidx;
			for (j = 0; j < 6; j++)
				removeKey.BSSID[j] = 0xff;

			ret = OidSetInformation(OID_802_11_REMOVE_KEY, s, "ra0", &removeKey, removeKey.Length);
			if (ret < 0)
				fprintf(stderr, "Set OID_802_11_REMOVE_KEY has error =%d, \n", ret);
		}
		else
		{
			if (nKeyLen == 10)
				nKeyLen = 5;
			else if (nKeyLen == 26)
				nKeyLen = 13;

			lBufLen = sizeof(NDIS_802_11_WEP) + nKeyLen - 1;
			// Allocate Resource
			pWepKey = (PNDIS_802_11_WEP)malloc(lBufLen);
			pWepKey->Length = lBufLen;
			pWepKey->KeyLength = nKeyLen;
			pWepKey->KeyIndex = keyidx;

				pWepKey->KeyIndex |= 0x80000000;

			if (strlen(wepkey) == 5)
				memcpy(pWepKey->KeyMaterial, wepkey, 5);
			else if (strlen(wepkey) == 10)
				AtoH(wepkey, pWepKey->KeyMaterial, 5);
			else if (strlen(wepkey) == 13)
				memcpy(pWepKey->KeyMaterial, wepkey, 13);
			else if (strlen(wepkey) == 26)
				AtoH(wepkey, pWepKey->KeyMaterial, 13);

			OidSetInformation(OID_802_11_ADD_WEP, s, "ra0", pWepKey, pWepKey->Length);
			free(pWepKey);
		}
	}

	// set ssid for associate
	if (OidSetInformation(OID_802_11_SSID, s, "ra0", &Ssid, sizeof(NDIS_802_11_SSID)) < 0) {
		fprintf(stderr, "Set OID_802_11_SSID has error =%d, pSsid->Ssid=%s\n", ret, Ssid.Ssid);
		close(s);
		return;
	}

	/*
	if (OidSetInformation(OID_802_11_BSSID, s, "ra0", &bssid, 6) < 0) {
		error(E_L, E_LOG, T("Set OID_802_11_BSSID has error."));
		close(s);
		return;
	}
	*/

	close(s);

	const char *opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (strcmp(opmode, "0")) {
		do_system("wpa_supplicant -B -ira0 -c/etc/wpa_supplicant.conf -Dralink -d");
		/* restart wan.sh if needed (renew dhcp, pppoe etc) */
		do_system("wan.sh");
	} else {
		do_system("wpa_supplicant -B -ira0 -bbr0 -c/etc/wpa_supplicant.conf -Dralink -d");
	}
	nvram_bufset(RTDEV_NVRAM, "staWpaSupplicant_flag", "1");
	WpaSupplicant_flag = TRUE;
}

static void conf_WPASupplicant(char* ssid, RT_WPA_SUPPLICANT_KEY_MGMT keymgmt, RT_WPA_SUPPLICANT_EAP eap, char* identity, char* password, char* cacert, char* clientcert, char* privatekey, char* privatekeypassword, char* wepkey, int keyidx, NDIS_802_11_WEP_STATUS encryp, RT_WPA_SUPPLICANT_TUNNEL tunnel, NDIS_802_11_AUTHENTICATION_MODE auth)
{
	FILE *wsconf;
	char wpaconf[] = "/etc/wpa_supplicant.conf";

	//fprintf(stderr, "conf_WPASupplicant()\n");

	fprintf(stderr, "wpaconf=%s\n", wpaconf);
	fprintf(stderr, "conf_WPASupplicant(), keymgmt=%d, Rtwpa_supplicantKeyMgmtNONE=%d\n", keymgmt, Rtwpa_supplicantKeyMgmtNONE);

	wsconf = fopen(wpaconf, "w+");

	fprintf(wsconf, "ctrl_interface=/var/run/wpa_supplicant\n");
	fprintf(wsconf, "eapol_version=1\n");
	fprintf(wsconf, "ap_scan=0\n");
	fprintf(wsconf, "network={\n");
	fprintf(wsconf, "ssid=\"%s\"\n", ssid);

	if (keymgmt == Rtwpa_supplicantKeyMgmtWPAEAP)
	{
		fprintf(wsconf, "key_mgmt=%s\n", "WPA-EAP");

		if (auth == Ndis802_11AuthModeWPA)
			fprintf(wsconf, "proto=WPA\n");
		else if (auth == Ndis802_11AuthModeWPA2)
			fprintf(wsconf, "proto=RSN\n");

		if (encryp == Ndis802_11Encryption2Enabled) //tkip
		{			
			fprintf(wsconf, "pairwise=TKIP\n");
			fprintf(wsconf, "group=TKIP\n");
		}
		else if (encryp == Ndis802_11Encryption3Enabled) //aes
		{
			fprintf(wsconf, "pairwise=CCMP TKIP\n");
			fprintf(wsconf, "group=CCMP TKIP\n");
		}

	}
	else if (keymgmt == Rtwpa_supplicantKeyMgmtIEEE8021X)
	{
		fprintf(wsconf, "key_mgmt=%s\n", "IEEE8021X");
		if (eap == Rtwpa_supplicantEAPTLS || eap == Rtwpa_supplicantEAPTTLS)
			fprintf(wsconf, "eapol_flags=3\n");
		else if (eap == Rtwpa_supplicantEAPMD5)
			fprintf(wsconf, "eapol_flags=0\n");
	}
	else if (keymgmt == Rtwpa_supplicantKeyMgmtNONE)
	{
		fprintf(wsconf, "key_mgmt=%s\n", "NONE");
		fprintf(wsconf, "}\n");
		fclose(wsconf);
		exec_WPASupplicant(ssid, encryp, auth, keymgmt, keyidx, wepkey);
		return;
	}

	//id
	fprintf(wsconf, "identity=\"%s\"\n",identity);

	//CA cert
	if (strcmp(cacert, "0" ) != 0 && strcmp(cacert, "") !=0) //option
		fprintf(wsconf, "ca_cert=\"%s\"\n", cacert);

	//eap
	switch(eap)
	{
		case Rtwpa_supplicantEAPTLS:
			fprintf(wsconf, "eap=TLS\n");
			fprintf(wsconf, "client_cert=\"%s\"\n", clientcert);
			fprintf(wsconf, "private_key=\"%s\"\n", privatekey);
			fprintf(wsconf, "private_key_passwd=\"%s\"\n", privatekeypassword);
			break;
		case Rtwpa_supplicantEAPTTLS:
			fprintf(wsconf, "eap=TTLS\n");
			if( strcmp(clientcert, "0" ) != 0 && strcmp(clientcert, "") !=0 ) //option
			{
				fprintf(wsconf, "client_cert=\"%s\"\n", clientcert);
				fprintf(wsconf, "private_key=\"%s\"\n", privatekey);
				fprintf(wsconf, "private_key_passwd=\"%s\"\n", privatekeypassword);
			}
			if (tunnel == Rtwpa_supplicantTUNNELMSCHAPV2)
				fprintf(wsconf, "phase2=\"auth=MSCHAPV2\"\n");
			else if (tunnel == Rtwpa_supplicantTUNNELMSCHAP)
				fprintf(wsconf, "phase2=\"auth=MSCHAP\"\n");
			else if (tunnel == Rtwpa_supplicantTUNNELPAP)
				fprintf(wsconf, "phase2=\"auth=PAP\"\n");
			break;

		case Rtwpa_supplicantEAPPEAP:
			fprintf(wsconf, "eap=PEAP\n");
			//fprintf(stderr, "clientcert=%s, strlen=%d\n", clientcert, strlen((char *)clientcert));
			if( strcmp(clientcert, "0" ) != 0 && strcmp(clientcert, "") !=0) //option
			{
				fprintf(wsconf, "client_cert=\"%s\"\n", clientcert);
				fprintf(wsconf, "private_key=\"%s\"\n", privatekey);
				fprintf(wsconf, "private_key_passwd=\"%s\"\n", privatekeypassword);
			}
			fprintf(wsconf, "password=\"%s\"\n", password);
			fprintf(wsconf, "phase1=\"peaplable=0\"\n");

			if (tunnel == Rtwpa_supplicantTUNNELMSCHAPV2)
				fprintf(wsconf, "phase2=\"auth=MSCHAPV2\"\n");

			break;
		case Rtwpa_supplicantEAPMD5:
			fprintf(wsconf, "eap=MD5\n");
			fprintf(wsconf, "password=\"%s\"\n", password);
			fprintf(wsconf, "wep_tx_keyidx=%d\n", keyidx);
			if (strlen(wepkey) == 5 || strlen(wepkey) == 13)
				fprintf(wsconf, "wep_key%d=\"%s\"\n", keyidx, wepkey);
			else if (strlen(wepkey) == 10 || strlen(wepkey) == 26)
			fprintf(wsconf, "wep_key%d=%s\n", keyidx, wepkey);

			break;
		default:
			break;
	}

	fprintf(wsconf, "}\n");
	fclose(wsconf);
	exec_WPASupplicant(ssid, encryp, auth, keymgmt, keyidx, wepkey);
}
#endif

void set_sta_basic(int nvram, char *input)
{
	char *value;
#if 0 /* AP Profile Only */
	char *wirelessmode, *hssid, *isolated_ssid;
	/* It should >= max(RT2860, RTEDV) */
	char    hidden_ssid[RT2860_MBSSID_NUM | RTDEV_MBSSID_NUM];
	char	noforwarding[RT2860_MBSSID_NUM | RTDEV_MBSSID_NUM];
	char *sz11aChannel, *sz11bChannel, *sz11gChannel;
	int i, new_bssid_num = 0, old_bssid_num, is_n = 0, is_ac = 0;
	char bssid_num[3];
	int max_mbssid = 1;
#else
	char *wirelessmode;
	int i, is_n = 0, is_ac = 0;
	int new_bssid_num = 0; //for workaround
	char *sz11aChannel, *sz11bChannel, *sz11gChannel;
#endif
	char *abg_rate, *n_mcs;
	char ifprefix[4];

	strcpy(ifprefix, "ra");
#if 0 /* AP Profile Only */
#if ! defined CONFIG_FIRST_IF_NONE 
	if (nvram == RT2860_NVRAM) {
		strcpy(ifprefix, "ra");	
		max_mbssid = RT2860_MBSSID_NUM;
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if (nvram == RTDEV_NVRAM) {
		strcpy(ifprefix, "rai");	
		max_mbssid = RTDEV_MBSSID_NUM;
	}
#endif
#endif

	value = web_get("radiohiddenButton", input, 0);
	if (!strncmp(value, "0", 2)) {
		do_system("iwpriv %s0 set RadioOn=0", ifprefix);
		nvram_set(nvram, "RadioOff", "1");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	else if (!strncmp(value, "1", 2)) {
		do_system("iwpriv %s0 set RadioOn=1", ifprefix);
		nvram_set(nvram, "RadioOff", "0");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	value = web_get("wifihiddenButton", input, 0);
	if (!strncmp(value, "0", 2)) {
#if 0 /* AP Profile only */
#if defined (RT2860_MBSS_SUPPORT) || defined (RTDEV_MBSS_SUPPORT)
		int bssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
		do {
			bssid_num--;
			do_system("ifconfig %s%d down", ifprefix, bssid_num);
		} while (bssid_num > 0);
#else
		do_system("ifconfig %s0 down", ifprefix);
#endif
#else
		do_system("ifconfig %s0 down", ifprefix);
#endif
		//do_system("rmmod rt2860v2_ap");
		nvram_set(nvram, "WiFiOff", "1");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}
	else if (!strncmp(value, "1", 2)) {
#if 0 /* AP Profile only */
		do_system("insmod -q rt2860v2_ap");
#if defined (RT2860_MBSS_SUPPORT) || defined (RTDEV_MBSS_SUPPORT)
		int idx = 0;
		int bssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
		do {
			do_system("ifconfig %s%d up", ifprefix, idx);
			idx++;
		} while (idx < bssid_num);
#else
		do_system("ifconfig %s0 up", ifprefix);
#endif
#else
		do_system("ifconfig %s0 up", ifprefix);
#endif
		nvram_set(nvram, "WiFiOff", "0");
		nvram_commit(nvram);
		web_redirect(getenv("HTTP_REFERER"));
		return;
	}

	wirelessmode = NULL;
	abg_rate = n_mcs = NULL;
	sz11aChannel = sz11bChannel = sz11gChannel = NULL;
	//web_debug_header();
	//fetch from web input
#if 0 /* AP Profile Only */
	wirelessmode = hssid = isolated_ssid = NULL;
	sz11aChannel = sz11bChannel = sz11gChannel = NULL;
	hssid = strdup(web_get("hssid", input, 1));
	isolated_ssid = strdup(web_get("isolated_ssid", input, 1));
	for (i = 0, value = web_get(racat("mssid_", i), input, 1); 
	     i < max_mbssid && strlen(value) > 0; 
	     i++, value = web_get(racat("mssid_", i), input, 1)) {
		new_bssid_num++;
		nvram_bufset(nvram, racat("SSID", new_bssid_num), value);
		if (new_bssid_num > 1) {
			sprintf(hidden_ssid, "%s;", hidden_ssid);
			sprintf(noforwarding, "%s;", hidden_ssid);
		} else {
			sprintf(hidden_ssid, "");
			sprintf(noforwarding, "");
		}
		if (strchr(hssid, i+0x30) != NULL)
			sprintf(hidden_ssid, "%s%s", hidden_ssid, "1");
		else
			sprintf(hidden_ssid, "%s%s", hidden_ssid, "0");
		if (strchr(isolated_ssid, i+0x30) != NULL)
			sprintf(noforwarding, "%s%s", hidden_ssid, "1");
		else
			sprintf(noforwarding, "%s%s", hidden_ssid, "0");
	}
	if (new_bssid_num < 1) {
		DBG_MSG("'SSID' should not be empty!");
		goto leave;
	}
	WebTF(input, "mbssidapisolated", nvram, "NoForwardingBTNBSSID", 1);
	old_bssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
#endif
	sz11aChannel = strdup(web_get("sz11aChannel", input, 1));
	sz11bChannel = strdup(web_get("sz11bChannel", input, 1));
	sz11gChannel = strdup(web_get("sz11gChannel", input, 1));
	wirelessmode = strdup(web_get("wirelessmode", input, 1)); //9: bgn mode
	abg_rate = strdup(web_get("abg_rate", input, 0));
	WebTF(input, "tx_stream", nvram, "HT_TxStream", 1);
	WebTF(input, "rx_stream", nvram, "HT_RxStream", 1);


	nvram_bufset(nvram, "WirelessMode", wirelessmode);
	//BasicRate: bg,bgn,n:15, b:3; g,gn:351
	if (!strncmp(wirelessmode, "4", 2) || !strncmp(wirelessmode, "7", 2)) //g, gn
		nvram_bufset(nvram, "BasicRate", "351");
	else if (!strncmp(wirelessmode, "1", 2)) //b
		nvram_bufset(nvram, "BasicRate", "3");
	else //bg,bgn,n
		nvram_bufset(nvram, "BasicRate", "15");

	if (!strncmp(wirelessmode, "8", 2) || !strncmp(wirelessmode, "9", 2) ||
	    !strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "11", 3))
		is_n = 1;
#if defined (RT2860_AC_SUPPORT) || defined (RTDEV_AC_SUPPORT)
	else if (!strncmp(wirelessmode, "14", 2) || !strncmp(wirelessmode, "15", 2))
		is_ac = 1;
#endif

#if 0 /* AP Profile Only */
	//#WPS
	if (!strcmp(hidden_ssid, "1")) {
		nvram_bufset(nvram, "WscModeOption", "0");
		do_system("miniupnpd.sh init");
		do_system("route delete 239.255.255.250");
	} else {
		const char *wordlist= nvram_bufget(nvram, "WscModeOption");
		if(wordlist){
			if (strcmp(wordlist, "0"))
				do_system("iwpriv %s0 set WscConfStatus=1", ifprefix);
			nvram_bufset(nvram, "WscConfigured", "1");
		}
	}
#else
	const char *wordlist= nvram_bufget(nvram, "WscModeOption");
	if(wordlist){
		if (strcmp(wordlist, "0"))
			do_system("iwpriv %s0 set WscConfStatus=1", ifprefix);
		nvram_bufset(nvram, "WscConfigured", "1");
	}
#endif

#if 0 /* AP Profile Only */
	sprintf(bssid_num, "%d", new_bssid_num);
	nvram_bufset(nvram, "BssidNum", bssid_num);
#if ! defined CONFIG_FIRST_IF_NONE 
	if ((nvram == RT2860_NVRAM) && 
	 	(new_bssid_num < 1 || new_bssid_num > RT2860_MBSSID_NUM)) {
		DBG_MSG("'bssid_num' %d is out of range!", new_bssid_num);
		goto leave;
	}
#endif
#if ! defined CONFIG_SECOND_IF_NONE 
	if ((nvram == RTDEV_NVRAM) && 
		(new_bssid_num < 1 || new_bssid_num > RTDEV_MBSSID_NUM)) {
		DBG_MSG("'bssid_num' %d is out of range!", new_bssid_num);
		goto leave;
	}
#endif
	revise_mbss_value(nvram, old_bssid_num, new_bssid_num);

	//Broadcast SSID
	nvram_bufset(nvram, "HideSSID", hidden_ssid);

	// NoForwarding and NoForwardingBTNBSSID
	nvram_bufset(nvram, "NoForwarding", noforwarding);
#endif

	//11abg Channel or AutoSelect
	if ((0 == strlen(sz11aChannel)) && (0 == strlen(sz11bChannel)) &&
	    (0 == strlen(sz11gChannel))) {
		DBG_MSG("'Channel' should not be empty!");
		goto leave;
	}
	if (!strncmp(sz11aChannel, "0", 2) || !strncmp(sz11bChannel, "0", 2) ||
	    !strncmp(sz11gChannel, "0", 2))
		nvram_bufset(nvram, "AutoChannelSelect", "1");
	else
		nvram_bufset(nvram, "AutoChannelSelect", "0");
	if (0 != strlen(sz11aChannel))
	{
		nvram_bufset(nvram, "Channel", sz11aChannel);
		do_system("iwpriv %s0 set Channel=%s", ifprefix, sz11aChannel);
	}
	if (0 != strlen(sz11bChannel))
	{
		nvram_bufset(nvram, "Channel", sz11bChannel);
		do_system("iwpriv %s0 set Channel=%s", ifprefix, sz11bChannel);
	}
	if (0 != strlen(sz11gChannel))
	{
		nvram_bufset(nvram, "Channel", sz11gChannel);
		do_system("iwpriv %s0 set Channel=%s", ifprefix, sz11gChannel);
	}
	sleep(1);

	//Rate for a, b, g
	if (strncmp(abg_rate, "", 1)) {
		int rate = strtol(abg_rate, NULL, 10);
		if (!strncmp(wirelessmode, "0", 2) || !strncmp(wirelessmode, "2", 2) || !strncmp(wirelessmode, "4", 2)) {
			if (rate == 1 || rate == 2 || rate == 5 || rate == 11)
				nvram_bufset(nvram, "FixedTxMode", "CCK");
			else
				nvram_bufset(nvram, "FixedTxMode", "OFDM");
#if 1 /* Workaround for the same field in Profile without mbssid setting. */
			new_bssid_num = 1;
			if (rate == 1)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "0");
			else if (rate == 2)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "1");
			else if (rate == 5)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "2");
			else if (rate == 6)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "0");
			else if (rate == 9)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "1");
			else if (rate == 11)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "3");
			else if (rate == 12)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "2");
			else if (rate == 18)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "3");
			else if (rate == 24)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "4");
			else if (rate == 36)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "5");
			else if (rate == 48)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "6");
			else if (rate == 54)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "7");
			else
				SetAllValues(new_bssid_num, nvram, HT_MCS, "33");
#endif
		}
		else if (!strncmp(wirelessmode, "1", 2)) {
			nvram_bufset(nvram, "FixedTxMode", "CCK");
#if 1 /* Workaround for the same field in Profile without mbssid setting. */
			new_bssid_num = 1;
			if (rate == 1)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "0");
			else if (rate == 2)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "1");
			else if (rate == 5)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "2");
			else if (rate == 11)
				SetAllValues(new_bssid_num, nvram, HT_MCS, "3");
#endif
		}
		else //shall not happen
			DBG_MSG("wrong configurations from web UI");
	}
	else
		nvram_bufset(nvram, "FixedTxMode", "HT");

	//HT_OpMode, HT_BW, HT_GI, HT_MCS, HT_RDG, HT_EXTCHA, HT_AMSDU, HT_TxStream, HT_RxStream
	if (is_n | is_ac) {
		WebTF(input, "n_mode", nvram, "HT_OpMode", 1);
		WebTF(input, "n_bandwidth", nvram, "HT_BW", 1);
		WebTF(input, "n_gi", nvram, "HT_GI", 1);
		n_mcs = strdup(web_get("n_mcs", input, 1));
#if 1 /* Workaround for the same field in Profile without mbssid setting. */
		new_bssid_num = 1;
		SetAllValues(new_bssid_num, nvram, HT_MCS, n_mcs);
#endif
		WebTF(input, "n_rdg", nvram, "HT_RDG", 1);
		WebTF(input, "n_extcha", nvram, "HT_EXTCHA", 1);
		WebTF(input, "n_stbc", nvram, "HT_STBC", 1);
		WebTF(input, "n_amsdu", nvram, "HT_AMSDU", 1);
		WebTF(input, "n_autoba", nvram, "HT_AutoBA", 1);
		WebTF(input, "n_badecline", nvram, "HT_BADecline", 1);
		WebTF(input, "n_disallow_tkip", nvram, "HT_DisallowTKIP", 1);
		WebTF(input, "n_2040_coexit", nvram, "HT_BSSCoexistence", 1);
		WebTF(input, "n_ldpc", nvram, "HT_LDPC", 1);
	}
#if defined (RT2860_AC_SUPPORT) || defined (RTDEV_AC_SUPPORT)
	if (is_ac) {
		WebTF(input, "vht_bandwidth", nvram, "VHT_BW", 1);
		WebTF(input, "vht2nd11aChannel", nvram, "VHT_Sec80_Channel", 1);
		WebTF(input, "vht_stbc", nvram, "VHT_STBC", 1);
		WebTF(input, "vht_sgi", nvram, "VHT_SGI", 1);
		WebTF(input, "vht_bw_signal", nvram, "VHT_BW_SIGNAL", 1);
		WebTF(input, "vht_ldpc", nvram, "VHT_LDPC", 1);
	}
#endif

	if (!strncmp(wirelessmode, "6", 2) || !strncmp(wirelessmode, "11", 2))
	{
#if 0 /* AP only Profile */
		int mbssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
#endif
		int i = 0;
#if 1 /* Workaround for same Profile field without mbssid setting. */
		int mbssid_num = 1;
		while (i < mbssid_num)
		{
			char tmp[128];
			char *EncrypType = (char *) nvram_bufget(nvram, "EncrypType");
			get_nth_value(i, EncrypType, ';', tmp, 128);
			if (!strcmp(tmp, "NONE") || !strcmp(tmp, "AES"))
			{
				memset(tmp, 0, 128);
				char *IEEE8021X = (char *) nvram_bufget(nvram, "IEEE8021X");
				get_nth_value(i, IEEE8021X, ';', tmp, 128);
				if (!strcmp(tmp, "0"))
				{
					i++;
					continue;
				}
			}
			set_nth_value_flash(nvram, i, "AuthMode", "OPEN");
			set_nth_value_flash(nvram, i, "EncrypType", "NONE");
			set_nth_value_flash(nvram, i, "IEEE8021X", "0");
			i++;
		}
#endif
	}
	nvram_commit(nvram);
	/*  this is a workaround:
	 *  when AP is built as kernel
	 *  if more ssids are created, driver won't exe RT28xx_MBSS_Init again
	 *  therefore, we reboot to make it available
	 *  (PS. CONFIG_MIPS would be "y")
	 */
#if 0 /* AP only Profile */
	if (new_bssid_num != old_bssid_num)
		do_system("reboot");
#endif

	//do_system("init_system restart");
	do_system("ifconfig ra0 down; ralink_init gen 2860; ifconfig ra0 up");
	do_system("iwpriv ra0 set EthConvertMode=dongle");

	if (-1 != initStaProfile())
		initStaConnection();

	/* restart wan.sh if needed (renew dhcp, pppoe etc) */
	const char *opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (strcmp(opmode, "0"))
	{
		do_system("wan.sh");
	}
leave:
/* AP only 
	free_all(8, hssid, isolated_ssid, wirelessmode, 
		    sz11aChannel, sz11bChannel, sz11gChannel,
		    abg_rate, n_mcs);
*/
	free_all(2, abg_rate, n_mcs);
	web_redirect(getenv("HTTP_REFERER"));
}

void set_sta_advance(int nvram, char *input)
{
	char 	*wmm_capable, *countrycode, *countryregion;
	char 	wmm_enable[16];
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	char  	*rvt = NULL;
	int     oldrvt;
	const char 	*oldrx, *oldtx;
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	char  	*txbf = NULL;
#endif // TXBF //
	int     i, ssid_num, wlan_mode;

	wmm_capable = countrycode = NULL;
	//web_debug_header();
	//fetch from web input
	WebTF(input, "bg_protection", nvram, "BGProtection", 1);
	// WebTF(input, "basic_rate", nvram, "BasicRate", 1);
#if 0 /* AP Profile only */
	WebTF(input, "beacon", nvram, "BeaconPeriod", 1);
#endif
	WebTF(input, "dtim", nvram, "DtimPeriod", 1);
	WebTF(input, "fragment", nvram, "FragThreshold", 1);
	WebTF(input, "rts", nvram, "RTSThreshold", 1);
	WebTF(input, "tx_power", nvram, "TxPower", 1);
	WebTF(input, "short_preamble", nvram, "TxPreamble", 1);
	WebTF(input, "short_slot", nvram, "ShortSlot", 1);
	WebTF(input, "tx_burst", nvram, "TxBurst", 1);
	WebTF(input, "pkt_aggregate", nvram, "PktAggregate", 1);
	WebTF(input, "ieee_80211h", nvram, "IEEE80211H", 1);
	wmm_capable = strdup(web_get("wmm_capable", input, 1));
	WebTF(input, "apsd_capable", nvram, "APSDCapable", 1);
	WebTF(input, "dls_capable", nvram, "DLSCapable", 1);
	countrycode = strdup(web_get("country_code", input, 1));
	countryregion = strdup(web_get("country_region", input, 1));
	WebTF(input, "m2u_enable", nvram, "M2UEnabled", 1);
	WebTF(input, "rd_region", nvram, "RDRegion", 1);
	WebTF(input, "carrier_detect", nvram, "CarrierDetect", 1);
#ifdef CONFIG_RT2860V2_AUTO_PROVISION
	WebTF(input, "auto_provision", nvram, "AutoProvisionEn", 1);
#endif
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	rvt = strdup(web_get("rvt", input, 1));
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	txbf = strdup(web_get("txbf", input, 1));
#endif // TXBF //

#if 0 /* AP Profile only */
	if (NULL != nvram_bufget(nvram, "BssidNum"))
		ssid_num = strtol(nvram_bufget(nvram, "BssidNum"), NULL, 10);
	else
		ssid_num = 1;
#else
	ssid_num = 1;
#endif
	wlan_mode = strtol(nvram_bufget(nvram, "WirelessMode"), NULL, 10);
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	if (NULL != nvram_bufget(nvram, "RVT"))
		oldrvt = strtol(nvram_bufget(nvram, "RVT"), NULL, 10);
	else
		oldrvt = 0;
#endif // getRVTBuilt //

	//set to nvram
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	if (!strncmp(rvt, "1", 2) && oldrvt != 1) {
		//backup the previous configurations for Rx/TxStream
		oldrx = nvram_bufget(nvram, "HT_RxStream");
		oldtx = nvram_bufget(nvram, "HT_TxStream");
		nvram_bufset(nvram, "HT_RxStream_old", oldrx);
		nvram_bufset(nvram, "HT_TxStream_old", oldtx);

		nvram_bufset(nvram, "RVT", "1");
		nvram_bufset(nvram, "HT_RxStream", "1");
		nvram_bufset(nvram, "HT_TxStream", "1");
		nvram_bufset(nvram, "HT_GI", "0");
		nvram_bufset(nvram, "VideoTurbine", "1");
	}
	else if (!strncmp(rvt, "0", 2) && oldrvt != 0) {
		//restore the previous configurations for Rx/TxStream
		oldrx = nvram_bufget(nvram, "HT_RxStream_old");
		oldtx = nvram_bufget(nvram, "HT_TxStream_old");

		nvram_bufset(nvram, "RVT", "0");
		nvram_bufset(nvram, "HT_RxStream", oldrx);
		nvram_bufset(nvram, "HT_TxStream", oldtx);
		nvram_bufset(nvram, "HT_GI", "1");
		nvram_bufset(nvram, "VideoTurbine", "0");
	}
#endif // getRVTBuilt //
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	if (!strncmp(txbf, "3", 2)) {
		nvram_bufset(nvram, "ETxBfEnCond", "1");
		nvram_bufset(nvram, "ETxBfIncapable", "0");
		nvram_bufset(nvram, "ITxBfEn", "1");
	}
	else if (!strncmp(txbf, "2", 2)) {
		nvram_bufset(nvram, "ETxBfEnCond", "1");
		nvram_bufset(nvram, "ETxBfIncapable", "0");
		nvram_bufset(nvram, "ITxBfEn", "0");
	}
	else if (!strncmp(txbf, "1", 2)) {
		nvram_bufset(nvram, "ETxBfEnCond", "0");
		nvram_bufset(nvram, "ETxBfIncapable", "1");
		nvram_bufset(nvram, "ITxBfEn", "1");
	}
	else {
		nvram_bufset(nvram, "ETxBfEnCond", "0");
		nvram_bufset(nvram, "ETxBfIncapable", "1");
		nvram_bufset(nvram, "ITxBfEn", "0");
	}
#endif // TXBF //

	bzero(wmm_enable, sizeof(char)*16);

#if 0 /* AP Profile only */
	for (i = 0; i < ssid_num; i++)
	{
		sprintf(wmm_enable+strlen(wmm_enable), "%d", strtol(wmm_capable, NULL, 10));
		sprintf(wmm_enable+strlen(wmm_enable), "%c", ';');
	}
	wmm_enable[strlen(wmm_enable) - 1] = '\0';
#else
	snprintf(wmm_enable, sizeof(wmm_enable), 
			"%d", strtol(wmm_capable, NULL, 10));
#endif
	nvram_bufset(nvram, "WmmCapable", wmm_enable);

	if (!strncmp(wmm_capable, "1", 2)) {
		if (wlan_mode < 5)
			nvram_bufset(nvram, "TxBurst", "0");
	}

#if ! defined CONFIG_EXT_CHANNEL_LIST
	if (wlan_mode == 0 || wlan_mode == 1 || wlan_mode == 4 ||
	    wlan_mode == 9 || wlan_mode == 6) {
		if (!strcmp(countryregion, "8"))
			nvram_bufset(RTDEV_NVRAM, "CountryRegion", "31");
		else if (!strcmp(countryregion, "9"))
			nvram_bufset(RTDEV_NVRAM, "CountryRegion", "32");
		else if (!strcmp(countryregion, "10"))
			nvram_bufset(RTDEV_NVRAM, "CountryRegion", "33");
		else
			nvram_bufset(RTDEV_NVRAM, "CountryRegion", countryregion);
	} else if (wlan_mode == 2 || wlan_mode == 8 || wlan_mode == 11 ||
		   wlan_mode == 14 || wlan_mode == 15) {
		nvram_bufset(RTDEV_NVRAM, "CountryRegionABand", countryregion);
	}
#endif
	nvram_bufset(nvram, "CountryCode", countrycode);

	nvram_commit(nvram);

	//do_system("init_system restart");
	do_system("ifconfig ra0 down; ralink_init gen 2860; ifconfig ra0 up");
	do_system("iwpriv ra0 set EthConvertMode=dongle");

	if (-1 != initStaProfile())
		initStaConnection();

	/* restart wan.sh if needed (renew dhcp, pppoe etc) */
	const char *opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (strcmp(opmode, "0"))
	{
		do_system("wan.sh");
	}

	//websRedirect(wp,"station/advance.asp");
	web_redirect(getenv("HTTP_REFERER"));
	//do_system("init_system restart");

	free_all(3, wmm_capable, countrycode, countryregion);
#if defined CONFIG_RA_CLASSIFIER_MODULE && defined CONFIG_RT2860V2_AP_VIDEO_TURBINE && defined (RT2860_TXBF_SUPPORT)
	free(rvt);
#endif
#if defined (RT2860_TXBF_SUPPORT) || defined (RTDEV_TXBF_SUPPORT)
	free(txbf);
#endif
}

void set_sta_wmm(int nvram, char *input)
{
	WebTF(input, "ap_aifsn_all", nvram, "APAifsn", 0);
	WebTF(input, "ap_cwmin_all", nvram, "APCwmin", 0);
	WebTF(input, "ap_cwmax_all", nvram, "APCwmax", 0);
	WebTF(input, "ap_txop_all", nvram, "APTxop", 0);
	WebTF(input, "ap_acm_all", nvram, "APACM", 0);
	WebTF(input, "ap_ackpolicy_all", nvram, "AckPolicy", 0);
	WebTF(input, "sta_aifsn_all", nvram, "BSSAifsn", 0);
	WebTF(input, "sta_cwmin_all", nvram, "BSSCwmin", 0);
	WebTF(input, "sta_cwmax_all", nvram, "BSSCwmax", 0);
	WebTF(input, "sta_txop_all", nvram, "BSSTxop", 0);
	WebTF(input, "sta_acm_all", nvram, "BSSACM", 0);

	nvram_commit(nvram);
	//do_system("init_system restart");
	do_system("ifconfig ra0 down; ralink_init gen 2860; ifconfig ra0 up");
	do_system("iwpriv ra0 set EthConvertMode=dongle");

	if (-1 != initStaProfile())
		initStaConnection();

	/* restart wan.sh if needed (renew dhcp, pppoe etc) */
	const char *opmode = nvram_get(RT2860_NVRAM, "OperationMode");
	if (strcmp(opmode, "0"))
	{
		do_system("wan.sh");
	}

	 web_back_parentpage();
}

int main(int argc, char **argv) 
{
	char *page, *inStr;
	long inLen;

	DBG_MSG("\n%s <------", __FUNCTION__); //Landen
	nvram_init(RT2860_NVRAM); //Landen
	if (argc > 1){
		if(!strcmp(argv[1], "init")) {
			//qos_init();		
			goto leave;	
		} else { 
			inStr = argv[1];
			DBG_MSG("From Command Line, inStr=%s\n", inStr);
		}
	} else {
		inLen = strtol(getenv("CONTENT_LENGTH"), NULL, 10) + 1;
		if (inLen <= 1) {
			fprintf(stderr, "%s: get no data!\n", __func__);
			return -1;
		}
		inStr = malloc(inLen);
		memset(inStr, 0, inLen);
		fgets(inStr, inLen, stdin);
	}

	page = web_get("page", inStr, 0);
	DBG_MSG("page='%s'", page); //Landen open
	if (!strcmp(page, "setSta11nCfg")) {
		setSta11nCfg(inStr);
	} else if (!strcmp(page, "setStaAdvance")) {
		setStaAdvance(inStr);
	} else if (!strcmp(page, "setStaDbm")) {
		setStaDbm(inStr);
	} else if (!strcmp(page, "setStaQoS")) {
		setStaQoS(inStr);
	} else if (!strcmp(page, "resetStaCounters")) {
		resetStaCounters(inStr);
	} else if (!strcmp(page, "setStaConnect")) {
		setStaConnect(inStr);
	} else if (!strcmp(page, "addStaProfile")) {
		addStaProfile(inStr);
	} else if (!strcmp(page, "getStaProfile")) {
		getStaProfile(inStr);
	} else if (!strcmp(page, "setStaProfile")) {
		setStaProfile(inStr);
	} else if (!strcmp(page, "getStaProfileData")) {
		getStaProfileData(inStr);
	} else if (!strcmp(page, "editStaProfile")) {
		editStaProfile(inStr);
	} else if (!strcmp(page, "updateCert")) {
		updateCert(inStr);
	} else if (!strcmp(page, "set_sta_basic")) {
		set_sta_basic(RT2860_NVRAM, inStr);
	} else if (!strcmp(page, "set_sta_advance")) {
		set_sta_advance(RT2860_NVRAM, inStr);
	} else if (!strcmp(page, "set_sta_wmm")) {
		set_sta_wmm(RT2860_NVRAM, inStr);
	} 
	free(inStr);
	
leave:
	nvram_close(RT2860_NVRAM); //Landen
	DBG_MSG("%s ------>\n", __FUNCTION__); //Landen
	
	return 0;
}
