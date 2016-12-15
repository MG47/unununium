/* LICENSE INFO: TBD*/

/* The main binary which calls the repsective command-line utility */


#include "utilbox.h"

#define UTILBOX_VERSION 0
#define UTILBOX_REVISION 1

struct util_list *util_list;

void util_init()
{
	int i;

	util_list = malloc(sizeof(struct util_list));

	for (i = 0; i < UTILITY_COUNT; i++)
		util_list->utils[i] = malloc(sizeof(struct utility));

	util_list->utils[0]->util_name = "dtsinfo";
	util_list->utils[0]->util_main = &dtsinfo_main;

}

int main(int argc, char **argv)
{
	printf("Welcome to utilbox (%d.%d)", UTILBOX_VERSION, UTILBOX_REVISION); 
	printf("executing utility %s\n", argv[0]);

	util_init();

	util_list->utils[0]->util_main(argc, argv);

	printf("Thanks for using utilbox\n");
	return 0;
}




