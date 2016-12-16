/* LICENSE INFO: TBD*/

/* tiper - text editor */

/* */

#include "utilbox.h"

#define VERSION 0
#define REVISION 1
#define TOOL_NAME "tiper"

#define OPT_HELP 1
#define OPT_VERSION 2

static void usage()
{
	printf("tiper:\n");
	printf("Usage: "
            "./tiper [OPTIONS] [FILE]\n"
            "OPTIONS:\n"
			"-h - Usage\n"
			"-v - Version\n"); 
}

static void print_version()
{
	printf("%s version:%d.%d",TOOL_NAME, VERSION, REVISION);
}

int tiper_main(int argc, char **argv)
{
    int opt;
	int flags = 0;

	printf("\nTiper Text Editor :\n\n");

	if (argc < 2) {
		usage();
		return 0;
	}

    while ((opt = getopt(argc, argv, "hv")) != -1) {
        switch (opt) {
		case 'h':
			flags |= OPT_HELP;
			break;
		case 'v':
			flags |= OPT_VERSION;
			break;
		}
	}

	if (flags & OPT_HELP)
		usage();

	if (flags & OPT_VERSION)
		print_version();

	printf("\n");
	return 0;
}
