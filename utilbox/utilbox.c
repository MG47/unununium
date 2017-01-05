/* LICENSE INFO: TBD*/

/* The main binary which calls the repsective command-line utility */


#include "utilbox.h"

#define UTILBOX_VERSION 0
#define UTILBOX_REVISION 1

struct utility util_list[UTILITY_COUNT] = {
	{
		.util_name = "dtsinfo",
		.version = DTSINFO_VERSION,
		.revision = DTSINFO_REVISION,
		.util_main = &dtsinfo_main,
	},
	{
		.util_name = "tiper",
		.version = TIPER_VERSION,
		.revision = TIPER_REVISION,
		.util_main = &tiper_main,
	},
	{
		.util_name = "less",
		.version = TIPER_VERSION,
		.revision = TIPER_REVISION,
		.util_main = &tiper_main,
	},
};

int main(int argc, char **argv)
{
	printf("Welcome to utilbox (%d.%d)\n", UTILBOX_VERSION, UTILBOX_REVISION); 
	printf("executing utility %s\n", argv[0]);

	int i;
	for (i = 0; i < UTILITY_COUNT; i++) {
		if (!strcmp(util_list[i].util_name, argv[0])) {
			util_list[i].util_main(argc, argv);
			break;
		}
	}

	printf("Thanks for using utilbox\n");
	return 0;
}




