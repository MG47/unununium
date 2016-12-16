/* LICENSE INFO: TBD*/

/* The main binary which calls the repsective command-line utility */


#include "utilbox.h"

#define UTILBOX_VERSION 0
#define UTILBOX_REVISION 1

struct utility util_list[UTILITY_COUNT] = {
	{
		.util_name = "dtsinfo",
		.version = 0,
		.revision = 1,
		.util_main = &dtsinfo_main,
	}
};

int main(int argc, char **argv)
{
	printf("Welcome to utilbox (%d.%d)", UTILBOX_VERSION, UTILBOX_REVISION); 
	printf("executing utility %s\n", argv[0]);

	int i;
	for (i = 0; i < UTILITY_COUNT; i++) {
		if (!strcmp(util_list->util_name, argv[0])) {
			util_list[i].util_main(argc, argv);
			break;
		}
	}

	printf("Thanks for using utilbox\n");
	return 0;
}




