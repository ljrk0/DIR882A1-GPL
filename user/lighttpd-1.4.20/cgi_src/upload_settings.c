
/*
 * Codes at here are heavily taken from upload.cgi.c which is for large file uploading , but 
 * in fact "upload_settings" only need few memory(~16k) so it is not necessary to follow 
 * upload.cgi.c at all.
 * 
 * YYHuang@Ralink TODO: code size.
 *  
 */

#include <unistd.h>	//for unlink
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <linux/reboot.h>

#define RFC_ERROR "RFC1867 ...."
#define UPLOAD_FILE "/var/tmpSettings"
#define RT2860_CONF_STR "#RT2860CONF"
#define RTDEV_CONF_STR "#RTDEV_CONF"

#define REFRESH_TIMEOUT		"60000"		/* 40000 = 40 secs*/

void *memmem(const void *buf, size_t buf_len, const void *byte_line, size_t byte_line_len)
{
	unsigned char *bl = (unsigned char *)byte_line;
	unsigned char *bf = (unsigned char *)buf;
	unsigned char *p  = bf;

	while (byte_line_len <= (buf_len - (p - bf))){
		unsigned int b = *bl & 0xff;
		if ((p = (unsigned char *) memchr(p, b, buf_len - (p - bf))) != NULL){
			if ( (memcmp(p, byte_line, byte_line_len)) == 0)
				return p;
			else
				p++;
		}else{
			break;
		}
	}
	return NULL;
}

#define MEM_SIZE	1024
#define MEM_HALT	512
int findStrInFile(char *filename, int offset, unsigned char *str, int str_len)
{
	int pos = 0, rc;
	FILE *fp;
	unsigned char mem[MEM_SIZE];

	if(str_len > MEM_HALT)
		return -1;
	if(offset <0)
		return -1;

	fp = fopen(filename, "rb");
	if(!fp)
		return -1;

	rewind(fp);
	fseek(fp, offset + pos, SEEK_SET);
	rc = fread(mem, 1, MEM_SIZE, fp);
	while(rc){
		unsigned char *mem_offset;
		mem_offset = (unsigned char*)memmem(mem, rc, str, str_len);
		if(mem_offset){
			fclose(fp);	//found it
			return (mem_offset - mem) + pos + offset;
		}

		if(rc == MEM_SIZE){
			pos += MEM_HALT;	// 8
		}else
			break;

		rewind(fp);
		fseek(fp, offset+pos, SEEK_SET);
		rc = fread(mem, 1, MEM_SIZE, fp);
	}

	fclose(fp);
	return -1;
}

/*
 *  ps. callee must free memory...
 */
void *getMemInFile(char *filename, int offset, int len)
{
	void *result;
	FILE *fp;
	if( (fp = fopen(filename, "r")) == NULL ){
		return NULL;
	}
	fseek(fp, offset, SEEK_SET);
	result = malloc(sizeof(unsigned char) * (len + 1) );
	memset(result, 0, sizeof(unsigned char) * (len + 1));
	if(!result){
		fclose(fp);
		return NULL;
	}
	if( fread(result, 1, len, fp) != len){
		free(result);
		fclose(fp);
		return NULL;
	}
	fclose(fp);
	return result;
}

void import(char *filename, int offset, int len)
{
	int  read_len;
	int  rt2860_str_len;
	int  rt2860_conf_detect = 0;
	int  rtdev_conf_detect = 0;
	int  first_sharp = 0;
	char tmp_str[16] = {0};
	char cmd[4096] = {0};
	char data;
	FILE *fp_2860  = NULL;
	FILE *fp_rtdev = NULL;
	char *pname_2860 = NULL;
	char *pname_rtdev = NULL;
	FILE *fp, *src;

//	snprintf(cmd, 4096, "cp %s /var/tmpcgi", filename);
//	system(cmd);

	if(!( src = fopen(filename, "r"))){
		printf("fopen(%s) NG<br>\n", filename);
		//fclose(pname);
		return;
	}

	if( fseek(src, offset, SEEK_SET) == -1){
		printf("fseek error\n");
	}

	// ralink_init utility need this to identity(?) .dat file.
	//fprintf(fp, "Default\n");

	fp = NULL;
	first_sharp = 0;
	while( len > 0 ){
		if(! fread(&data, 1, 1, src)){
			rt2860_conf_detect = 0;
			rtdev_conf_detect  = 0;
			break;
		}

		if((data == '#') && (first_sharp == 0)){
			//read profile specific string. 
			//strlen is the same of all profile specific string.
			first_sharp = 1;
			printf("first_sharp detected<br>\n");

			if(fp != NULL)
				fwrite(&data, 1, 1, fp);

			len--;
			continue;
		} 

		if((data == '#') && (first_sharp == 1)){
			printf("Second_sharp detected<br>\n");

			tmp_str[0] = data;

			rt2860_str_len = strlen(RT2860_CONF_STR);

			read_len = fread((tmp_str+1), 1, rt2860_str_len - 1, src);
			printf("read_len=%d<br>\n", read_len);
			if (read_len != (rt2860_str_len - 1)) {
				printf("read rtdev config NG, read %d != request %d<br>\n",
					read_len, rt2860_str_len - 1);
				goto err;
			}
			len -= rt2860_str_len;
			printf("tmp_str=\"%s\"<br>\n", tmp_str);

			if (!strcmp(tmp_str, RT2860_CONF_STR)) {
				rt2860_conf_detect = 1;
				if(! fread(&data, 1, 1, src)) //read \n
					goto err;
				printf("Detect rt2860 config<br>\n");
				pname_2860 = tempnam("/var", "set");
				if(pname_2860 == NULL)
					pname_2860 = "/var/pname_rt2860";

				if(!(fp_2860 = fopen(pname_2860, "w+"))){
					printf("fopen(%s) NG<br>\n", pname_2860);
					goto err;
				}

				printf("Write 2860 config to %s<br>\n", pname_2860);
				fp = fp_2860;
			} else if(!strcmp(tmp_str, RTDEV_CONF_STR)){
				rtdev_conf_detect = 1;
				if(! fread(&data, 1, 1, src)) //read \n
					goto err;
				printf("Detect rtdev config<br>\n");

				pname_rtdev = tempnam("/var", "set");
				if(pname_rtdev == NULL)
					pname_rtdev = "/var/pname_rtdev";

				if(!(fp_rtdev = fopen(pname_rtdev, "w+"))){
					printf("fopen(%s) NG<br>\n", pname_rtdev);
					goto err;
				}

				printf("Write rtdev config to %s<br>\n", pname_rtdev);
				fp = fp_rtdev;
			} else {
				if(fp != NULL)
					fwrite(tmp_str, 1, rt2860_str_len, fp);
			}
		} else {
			if(fp != NULL)
				fwrite(&data, 1, 1, fp);

			len--;
		}

		first_sharp = 0;
	}

	if(fp_2860 != NULL){
		fclose(fp_2860);
		fp_2860 = NULL;
	}

	if(fp_rtdev != NULL){
		fclose(fp_rtdev);
		fp_rtdev = NULL;
	}
#if 1
	if (rt2860_conf_detect == 1){
		system("ralink_init clear 2860");
		snprintf(cmd, 4096, "ralink_init renew 2860 %s", pname_2860);
		system(cmd);
		//unlink(pname_2860);
		free(pname_2860);
	}

	if (rtdev_conf_detect == 1){
		system("ralink_init clear rtdev");
		snprintf(cmd, 4096, "ralink_init renew rtdev %s", pname_rtdev);
		system(cmd);
		//unlink(pname_rtdev);
		free(pname_rtdev);
	}
#else // Easy debug mode
	if (rt2860_conf_detect == 1){
		printf("ralink_init clear 2860<br>\n");
		printf("ralink_init renew 2860 %s<br>\n", pname_2860);
		free(pname_2860);
	}

	if (rtdev_conf_detect == 1){
		printf("ralink_init clear rtdev<br>\n");
		printf("ralink_init renew rtdev %s<br>\n", pname_rtdev);
		free(pname_rtdev);
	}
#endif

err:
	if(fp_2860 != NULL)
		fclose(fp_2860);

	if(fp_rtdev != NULL)
		fclose(fp_rtdev);

	if(src != NULL)
		fclose(src);

}

#define DEFAULT_LAN_IP "10.10.10.254"
char *getLanIP(void)
{
	static char buf[64];
	char *nl;
	FILE *fp;

	memset(buf, 0, sizeof(buf));
	if( (fp = popen("nvram_get 2860 lan_ipaddr", "r")) == NULL )
		goto error;

	if(!fgets(buf, sizeof(buf), fp)){
		pclose(fp);
		goto error;
	}

	if(!strlen(buf)){
		pclose(fp);
		goto error;
	}
	pclose(fp);

	if(nl = strchr(buf, '\n'))
		*nl = '\0';

	return buf;

error:
	fprintf(stderr, "warning, cant find lan ip\n");
	return DEFAULT_LAN_IP;
}

void javascriptUpdate(void)
{
	printf("<script language=\"JavaScript\" type=\"text/javascript\">");
	printf(" \
function refresh_all(){	\
  top.location.href = \"http://%s\"; \
} \
function update(){ \
  self.setTimeout(\"refresh_all()\", %s);\
}", getLanIP(), REFRESH_TIMEOUT);
	printf("update();");
	printf("</script>");
}

int main (int argc, char *argv[])
{
	int file_begin, file_end;
	int line_begin, line_end;
	char *boundary= NULL; 
	int boundary_len;
	FILE *fp = NULL;
	long inLen;
	char *inStr = NULL;
	char *tmp;

	fp = fopen(UPLOAD_FILE, "w");
	if (fp == NULL)
		return;

	tmp = getenv("CONTENT_LENGTH");
	if (tmp == NULL) tmp = "";
	inLen = strtol(tmp, NULL, 10) + 1;
	if (inLen >= 2147483647) inLen = 2147483647;
	if (inLen <= 0) inLen = 0;
	if (inLen <= 1) {
		goto err;
	}

	inStr = malloc(inLen * sizeof(char));
	if(inStr == NULL)
		goto err;//inStr= "";

	memset(inStr, 0, inLen);
	fread(inStr, 1, inLen, stdin);
	fwrite(inStr, 1, inLen, fp);
	fclose(fp);


	tmp = getenv("SERVER_SOFTWARE");
	if (tmp == NULL) tmp = "";
	printf(
"\
Server: %s\n\
Pragma: no-cache\n\
Content-type: text/html\n", tmp);

	printf("\n\
<html>\n\
<head>\n\
<TITLE>Import Settings</TITLE>\n\
<link rel=stylesheet href=/style/normal_ws.css type=text/css>\n\
<meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\">\n\
</head>\n\
<body> <h1> Import Settings</h1>");
    
	if(inLen < 200){
		printf("failed, can't get env var.\n");
		goto err;
	}

	line_begin = 0;
	if((line_end = findStrInFile(UPLOAD_FILE, line_begin, "\r\n", 2)) == -1){
		printf("%s", RFC_ERROR);
		goto err;
	}
	boundary_len = line_end - line_begin;
	boundary = getMemInFile(UPLOAD_FILE, line_begin, boundary_len);

	// sth like this..
	// Content-Disposition: form-data; name="filename"; filename="\\192.168.3.171\tftpboot\a.out"
	//
	char *line, *semicolon, *user_filename, *reboot_later;
	line_begin = line_end + 2;
	if((line_end = findStrInFile(UPLOAD_FILE, line_begin, "\r\n", 2)) == -1){
		printf("%s", RFC_ERROR);
		goto err;
	}
	line = getMemInFile(UPLOAD_FILE, line_begin, line_end - line_begin);
	if(strncasecmp(line, "content-disposition: form-data;", strlen("content-disposition: form-data;"))){
		printf("%s", RFC_ERROR);
		goto err;
	}
	semicolon = line + strlen("content-disposition: form-data;");
/*
	if(! (semicolon = strchr(semicolon, ';'))  ){
		printf("dont support multi-field upload.\n");
		goto err;
	}
*/
	reboot_later = semicolon + strlen(" name=\"");
	if( strncasecmp(reboot_later, "RebootLater", strlen("RebootLater"))  ){
		printf("%s", RFC_ERROR);
		goto err;
	}
	line_begin = line_end + 2;
	if((line_end = findStrInFile(UPLOAD_FILE, line_begin, "\r\n", 2)) == -1){
		printf("%s", RFC_ERROR);
		goto err;
	}
	line_begin = line_end + 2;
	if((line_end = findStrInFile(UPLOAD_FILE, line_begin, "\r\n", 2)) == -1){
		printf("%s", RFC_ERROR);
		goto err;
	}
	reboot_later = getMemInFile(UPLOAD_FILE, line_begin, line_end - line_begin);
	printf("reboot_later=%s<br>", reboot_later);

	if (line != NULL)
		free(line);

	// goto 2nd bundary line.
	line_begin = line_end + 2;
	if((line_end = findStrInFile(UPLOAD_FILE, line_begin, "\r\n", 2)) == -1){
		printf("%s", RFC_ERROR);
		goto err;
	}
	line = getMemInFile(UPLOAD_FILE, line_begin, line_end - line_begin);
	if(strncmp(line, boundary, strlen(boundary))){
		printf("boundary error:\n[%s]!=[%s]\n", line_begin, boundary);
		goto err;
	}
	// goto 2nd Content-Disposition line
	line_begin = line_end + 2;
	if((line_end = findStrInFile(UPLOAD_FILE, line_begin, "\r\n", 2)) == -1){
		printf("%s", RFC_ERROR);
		goto err;
	}
	if(line != NULL)
		free(line);

	line = getMemInFile(UPLOAD_FILE, line_begin, line_end - line_begin);
	if(strncasecmp(line, "content-disposition: form-data;", strlen("content-disposition: form-data;"))){
		printf("%s", RFC_ERROR);
		goto err;
	}
	semicolon = line + strlen("content-disposition: form-data;") + 1;
	if(! (semicolon = strchr(semicolon, ';'))  ){
		printf("dont support multi-field upload.\n");
		goto err;
	}
	user_filename = semicolon + 2;
	if( strncasecmp(user_filename, "filename=", strlen("filename="))  ){
		printf("%s", RFC_ERROR);
		goto err;
	}
	user_filename += strlen("filename=");
	//until now we dont care about what  true filename is.
	if(line != NULL)
		free(line);

	// We may check a string  "Content-Type: application/octet-stream" here
	// but if our firmware extension name is the same with other known name, 
	// the browser will define other content-type itself instead,
	line_begin = line_end + 2;
	if((line_end = findStrInFile(UPLOAD_FILE, line_begin, "\r\n", 2)) == -1){
		printf("%s", RFC_ERROR);
		goto err;
	}

	line_begin = line_end + 2;
	if((line_end = findStrInFile(UPLOAD_FILE, line_begin, "\r\n", 2)) == -1){
		printf("%s", RFC_ERROR);
		goto err;
	}

	file_begin = line_end + 2;

	if( (file_end = findStrInFile(UPLOAD_FILE, file_begin, boundary, boundary_len)) == -1){
		printf("%s", RFC_ERROR);
		goto err;
	}
	file_end -= 2;		// back 2 chars.(\r\n);

	//printf("b=%d, e=%d<br>\n", file_begin, file_end);
	import(UPLOAD_FILE, file_begin, file_end - file_begin);

	printf("done");
	javascriptUpdate();
	printf("</body></html>\n");
	sleep(3);
	if (*reboot_later != '1')
		reboot(LINUX_REBOOT_CMD_RESTART);
err:
	if (!inStr)
		free(inStr);

	if (boundary)
		free(boundary);

	if (reboot_later)
		free(reboot_later);

	exit(0);
}

