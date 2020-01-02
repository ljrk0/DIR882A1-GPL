#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>

int dhcp6c_main(int argc, char **argv);
int dhcp6s_main(int argc, char **argv);

int main(int argc, char **argv)
{
	char *progname;

	if (argc < 1) {
		fprintf(stderr, "ERROR: This should not happen.\n");
		exit(EXIT_FAILURE);
	}

	progname = basename(argv[0]);
	if (strcmp(progname, "dhcp6c") == 0)
		return dhcp6c_main(argc, argv);
	if (strcmp(progname, "dhcp6s") == 0)
		return dhcp6s_main(argc, argv);

	++argv;
	--argc;
	if (argc < 1) {
		fprintf(stderr, "ERROR: No subcommand given.\n");
		exit(EXIT_FAILURE);
	}

	progname = basename(argv[0]);
	if (strcmp(progname, "dhcp6c") == 0)
		return dhcp6c_main(argc, argv);
	if (strcmp(progname, "dhcp6s") == 0)
		return dhcp6s_main(argc, argv);

	fprintf(stderr, "dhcp6 multi-purpose version: "
	        "unknown subcommand \"%s\"\n", progname);
	exit(EXIT_FAILURE);
}
