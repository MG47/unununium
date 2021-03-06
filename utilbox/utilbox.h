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
#include <signal.h>
#include <ncurses.h>
#include <fcntl.h>

#define UTILITY_COUNT 3

struct utility {
	char *util_name;
	int argc;
	int version;
	int revision;
	int (*util_main)(int, char **);
};

/* Utility-specific macros and function declarations */

/* dtsinfo */
int dtsinfo_main(int argc, char **argv);
#define DTSINFO_VERSION 0
#define DTSINFO_REVISION 1

/* tiper */
int tiper_main(int argc, char **argv);
#define TIPER_VERSION 0
#define TIPER_REVISION 1

/* less */
int less_main(int argc, char **argv);
#define LESS_VERSION 0
#define LESS_REVISION 1

#endif
