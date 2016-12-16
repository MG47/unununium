/* LICENSE INFO: TBD*/


/* utilbox primary header file */

#ifndef UTILBOX_H
#define UTILBOX_H

#include <stdio.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

#define UTILITY_COUNT 1

struct utility {
	char * util_name;
	int argc;
	int version;
	int revision;
	int (*util_main)(int, char **);
};

int dtsinfo_main(int argc, char **argv);
void print_classes();

#endif
