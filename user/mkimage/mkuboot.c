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
	fprintf (stderr, "          mkuboot -f filename\n"
		);
	exit (EXIT_FAILURE);
}

int
main (int argc, char **argv)
{
	char *uboot_file = NULL;
	char *out_file = NULL;
	char *product = NULL;
	char *sw_version = NULL;
	char *hw_version = NULL;
	int ifd_out = 0;
	int ifd_in = 0;
	char buf[512] = {};
	struct stat sbuf;
	unsigned char *ptr = NULL;
	char *data = NULL;
	uboot_header_t header;
	uboot_header_t *uhdr = &header;
	unsigned long ul_crc = 0xFFFFFFFF;
	
	cmdname = *argv;

	while (--argc > 0 && **++argv == '-') {
		while (*++*argv) {
			switch (**argv) {
			case 'f':
				if (--argc <= 0)
					usage ();
				uboot_file = *++argv;
				goto NXTARG;
			case 'p':
				if (--argc <= 0)
					usage ();
				product = *++argv;
				goto NXTARG;
			case 's':
				if (--argc <= 0)
					usage ();
				sw_version = *++argv;
				goto NXTARG;
			case 'h':
				if (--argc <= 0)
					usage ();
				hw_version = *++argv;
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
	
	ifd_in = open(uboot_file,  O_RDONLY);
	if(-1 == ifd_in)
	{
		printf("open %s file fail\n", uboot_file);
		exit (EXIT_FAILURE);
	}

	if (fstat(ifd_in, &sbuf) < 0) 
	{
		printf ("Can't stat uboot file: %s\n", strerror(errno));
		exit (EXIT_FAILURE);
	}

	ptr = (unsigned char *)mmap(0, sbuf.st_size,
					    PROT_READ, MAP_SHARED, ifd_in, 0);
	if ((caddr_t)ptr == (caddr_t)-1) 
	{
		printf ("Can't read Config out file: %s\n",strerror(errno));
		exit (EXIT_FAILURE);
	}
	
	data = (char *)ptr;
	ul_crc = crc32 (0, data, sbuf.st_size);

	uhdr->ih_magic = htonl(IH_MAGIC);
	uhdr->ih_time  = htonl(sbuf.st_mtime);
	uhdr->ih_size  = htonl(sbuf.st_size);
	uhdr->ih_dcrc  = htonl(ul_crc);
	strncpy((char *)uhdr->product, product, PRODUCT_LEN);	
	strncpy((char *)uhdr->sw_version, sw_version, VERSION_LEN);
	strncpy((char *)uhdr->hw_version, hw_version, VERSION_LEN);

	ifd_out = open(out_file,  O_RDWR|O_CREAT|O_TRUNC|O_BINARY, 0666);
	if(-1 == ifd_out)
	{
		printf("open %s file fail\n", out_file);
		exit (EXIT_FAILURE);
	}

	if(sizeof(uboot_header_t) != write(ifd_out, uhdr, sizeof(uboot_header_t)))
	{
		printf("write uboot out file fail\n");
		
		close(ifd_out);
		
		close(ifd_in);
		
		exit (EXIT_FAILURE);
	}

	if(sbuf.st_size != write(ifd_out, ptr, sbuf.st_size))
	{
		printf("write uboot out file fail\n");
		
		close(ifd_out);
		
		close(ifd_in);
		
		exit (EXIT_FAILURE);
	}
	
	(void) munmap((void *)ptr, sbuf.st_size);

	close(ifd_out);
		
	close(ifd_in);

	exit (EXIT_SUCCESS);
}

