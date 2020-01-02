
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef __WIN32__
#include <netinet/in.h>		/* for host / network byte order conversions	*/
#endif
#include <sys/mman.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>

#if defined(__BEOS__) || defined(__NetBSD__) || defined(__APPLE__)
#include <inttypes.h>
#endif

#ifdef __WIN32__
typedef unsigned int __u32;

#define SWAP_LONG(x) \
	((__u32)( \
		(((__u32)(x) & (__u32)0x000000ffUL) << 24) | \
		(((__u32)(x) & (__u32)0x0000ff00UL) <<  8) | \
		(((__u32)(x) & (__u32)0x00ff0000UL) >>  8) | \
		(((__u32)(x) & (__u32)0xff000000UL) >> 24) ))
typedef		unsigned char	uint8_t;
typedef		unsigned short	uint16_t;
typedef		unsigned int	uint32_t;

#define     ntohl(a)	SWAP_LONG(a)
#define     htonl(a)	SWAP_LONG(a)
#endif	/* __WIN32__ */

#ifndef	O_BINARY		/* should be define'd on __WIN32__ */
#define O_BINARY	0
#endif

#include <image.h>

char *cmdname;

extern unsigned long crc32 (unsigned long crc, const char *buf, unsigned int len);

void
usage ()
{
	fprintf (stderr, "          -2 ==> config default path\n"
			 "          -5 ==> config 5G path\n"
		);
	exit (EXIT_FAILURE);
}

int
main (int argc, char **argv)
{
	char *rt2860_file = NULL;
	char *rtdev_file = NULL;
	char *out_file = NULL;
	FILE *fp = NULL;
	int ifd_out = 0;
	int ifd_tmp_out = 0;
	char buf[512] = {};
	int found = 0;
	unsigned long ul_crc = 0xFFFFFFFF;
	char *p = NULL;
	int i = 2, j = 0;
	unsigned char zero = 0;
	unsigned char *ptr = NULL;
	struct stat sbuf;
	char *data = NULL;
	unsigned char def_ff = 0xFF;
	unsigned int ui_len = 0;
	
	cmdname = *argv;

	while (--argc > 0 && **++argv == '-') {
		while (*++*argv) {
			switch (**argv) {
			case '2':
				if (--argc <= 0)
					usage ();
				rt2860_file = *++argv;
				goto NXTARG;
			case '5':
				if (--argc <= 0)
					usage ();
				rtdev_file = *++argv;
				goto NXTARG;
			case 'o':
				if (--argc <= 0)
					usage ();
				out_file = *++argv;
				goto NXTARG;
			default:
				usage ();
			}
		}
NXTARG:		;
	}
	
	ifd_out = open(out_file,  O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0666);
	if(-1 == ifd_out)
	{
		printf("open all out Config config file fail\n");
		if (fp != 0)
		{
			fclose(fp);
		}
		
		exit (EXIT_FAILURE);
	}
	
	for(j = 0; j < 0x2000; j++)
	{
		if(1 != write(ifd_out, (char *)&def_ff, 1))
		{
			printf("write 1 all out Config config file fail:%s\n", strerror(errno));
			if (fp != 0)
			{
				fclose(fp);
			}
					
			close(ifd_out);
			
			exit (EXIT_FAILURE);
		}
	}
	
	do
	{
		if(2 == i)
			fp = fopen(rt2860_file, "ro");
		else
			fp = fopen(rtdev_file, "ro");
		if(NULL == fp)
		{
			printf("open %s config file fail\n", (i == 2) ? rt2860_file:rtdev_file);
			exit (EXIT_FAILURE);
		}

		while (NULL != fgets(buf, sizeof(buf), fp)) 
		{
			if (buf[0] == '\n' || buf[0] == '#')
				continue;
			if (!strncmp(buf, "Default\n", 8)) 
			{
				found = 1;
				break;
			}
		}
		if (!found) 
		{
			printf("file format error!\n");
			if (fp != 0)
			{
				fclose(fp);
			}
			exit (EXIT_FAILURE);
		}
		
		if(2 == i)
			ifd_tmp_out = open("Config",  O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0666);
		else
			ifd_tmp_out = open("rtdev",  O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0666);
		if(-1 == ifd_tmp_out)
		{
			printf("open out Config config file fail\n");
			if (fp != 0)
			{
				fclose(fp);
			}

			close(ifd_out);

			exit (EXIT_FAILURE);
		}
		
		if(sizeof(ul_crc) != write(ifd_tmp_out, (char *)&ul_crc, sizeof(ul_crc)))
		{
			printf("write out Config config file fail\n");
			if (fp != 0)
			{
				fclose(fp);
			}
			
			close(ifd_tmp_out);

			close(ifd_out);
			
			exit (EXIT_FAILURE);
		}
		
		ui_len = 4;
		
		while (NULL != fgets(buf, sizeof(buf), fp))
		{
			if (buf[0] == '\n' || buf[0] == '#')
				continue;
			
			if (NULL == (p = strchr(buf, '=')))
	        {
				printf("%s file format error!\n\n", rt2860_file);
				if (fp != 0)
				{
					fclose(fp);
				}

				close(ifd_tmp_out);
				
				close(ifd_out);
				
				exit (EXIT_FAILURE);
			}
			buf[strlen(buf) - 1] = '\0'; //remove carriage return
			//*p++ = '\0'; //seperate the string
			if(strlen(buf) != write(ifd_tmp_out, buf, strlen(buf)))
			{
				printf("write buf to out Config config file fail\n");
				if (fp != 0)
				{
					fclose(fp);
				}
				
				close(ifd_tmp_out);
				
				close(ifd_out);

				exit (EXIT_FAILURE);
			}

			ui_len += strlen(buf);

			if(1 != write(ifd_tmp_out, (char *)&zero, 1))
			{
				printf("write zero to out Config config file fail\n");
				if (fp != 0)
				{
					fclose(fp);
				}
				
				close(ifd_tmp_out);
				
				close(ifd_out);

				exit (EXIT_FAILURE);
			}

			ui_len += 1;
		}

		if(2 == i)
		{
			for(j = 0; j < 0x4000-ui_len; j++)
			{
				if(1 != write(ifd_tmp_out, (char *)&zero, 1))
				{
					printf("write config zero to out Config config file fail\n");
					if (fp != 0)
					{
						fclose(fp);
					}
					
					close(ifd_tmp_out);
					
					close(ifd_out);

					exit (EXIT_FAILURE);
				}
			}
		}
		else
		{
			for(j = 0; j < 0x2000-ui_len; j++)
			{
				if(1 != write(ifd_tmp_out, (char *)&zero, 1))
				{
					printf("write rtdev zero to out Config config file fail\n");
					if (fp != 0)
					{
						fclose(fp);
					}
					
					close(ifd_tmp_out);
					
					close(ifd_out);

					exit (EXIT_FAILURE);
				}
			}

		}

		lseek(ifd_tmp_out, 0, SEEK_SET);
		
		if (fstat(ifd_tmp_out, &sbuf) < 0) 
		{
			printf ("Can't stat Config out file: %s\n", strerror(errno));
			exit (EXIT_FAILURE);
		}
		
		ptr = (unsigned char *)mmap(0, sbuf.st_size,
					    PROT_READ, MAP_SHARED, ifd_tmp_out, 0);
		if ((caddr_t)ptr == (caddr_t)-1) 
		{
			printf ("Can't read Config out file: %s\n",strerror(errno));
			exit (EXIT_FAILURE);
		}
		
		data = (char *)(ptr+4);
		ul_crc = crc32 (0, data, sbuf.st_size -4);

		if(sizeof(ul_crc) != write(ifd_tmp_out, (char *)&ul_crc, sizeof(ul_crc)))
		{
			printf("renew write out Config config file fail\n");
			if (fp != 0)
			{
				fclose(fp);
			}
			
			close(ifd_tmp_out);
			
			close(ifd_out);
			
			exit (EXIT_FAILURE);
		}
		
		if (fp != 0)
		{
			fclose(fp);
		}
		
		if(sbuf.st_size != write(ifd_out, ptr, sbuf.st_size))
		{
			printf("write 2 all out Config config file fail\n");
			if (fp != 0)
			{
				fclose(fp);
			}
			
			close(ifd_tmp_out);
			
			close(ifd_out);
			
			exit (EXIT_FAILURE);
		}
		
		(void) munmap((void *)ptr, sbuf.st_size);

		close(ifd_tmp_out);

	}while(--i);

	for(j = 0; j < 0x8000; j++)
	{
		if(1 != write(ifd_out, (char *)&zero, 1))
		{
			printf("write zero to out Config config file fail\n");
			if (fp != 0)
			{
				fclose(fp);
			}
					
			close(ifd_out);

			exit (EXIT_FAILURE);
		}
	}

	close(ifd_out);

	unlink("Config");
	
	unlink("rtdev");

	exit (EXIT_SUCCESS);
}

