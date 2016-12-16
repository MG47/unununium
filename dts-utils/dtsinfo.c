/* LICENSE INFO: TBD*/

/* dtsinfo - show information about device tree and associated firmware in the system */

/* */

#include "utilbox.h"

#define VERSION 0
#define REVISION 1
#define TOOL_NAME "dtsinfo"

#define OPT_HELP 1
#define OPT_VERSION (1 << 1)
#define OPT_CLASSES (1 << 2)

/* TODO Find portable macro*/
#define LINE_MAX 1024

#define SYSFS_CLASS_PATH "/sys/class/"
#define SYSFS_DEVICE_PATH "/sys/devices/"
#define SYSFS_DEVICETREE_PATH "/sys/firmware/devicetree/base"

void usage()
{
	printf("dtsinfo:\n");
	printf("Usage: "
            "./dtsinfo [-c -h -v]\n"
            "-c - Print classes\n"
			"-h - Usage\n"
			"-v - Version\n"); 
}

void print_version()
{
	printf("%s version:%d.%d",TOOL_NAME, VERSION, REVISION);
}

void print_classes()
{
	DIR *dir;
	struct dirent *ent;

	printf("classes:\n");

	dir = opendir(SYSFS_CLASS_PATH);
	if (!dir) {
		printf("error: %s\n", strerror(errno));
		return;
	}

	while ((ent = readdir(dir)) != NULL) {
		if ((strcmp(ent->d_name, ".")) && strcmp((ent->d_name), ".."))
			printf("%s\n", ent->d_name);
	}

	closedir(dir);
}

void print_content(char *dirname, struct dirent *entry)
{
	FILE *stream;
	char pathname[PATH_MAX];
	char content[LINE_MAX];

	printf("%s:\n", entry->d_name);

	if (entry->d_type != DT_REG) {
		printf("error: file is not a regular file\n");
		return;
	}

	sprintf(pathname, "%s/%s", dirname, entry->d_name);
	stream = fopen(pathname, "r");
	if (!stream) {
		printf("error: %s\n", strerror(errno));
		return;
	}

	if(!fgets(content, LINE_MAX, stream)){
		printf("Infomration not available\n");
	}

	if (fputs(content, stdout) == EOF) {
		printf("error writing to standard out\n");
	}

	printf("\n\n");
}

void print_devicetree()
{
	DIR *dir;
	struct dirent *ent;
	struct dirent *entry[20];
	struct dirent *(*direntp)[];
	int i = 0, count;
	char *dirname = SYSFS_DEVICETREE_PATH;

	dir = opendir(dirname);
	if (!dir) {
		printf("error: %s\n", strerror(errno));
		return;
	}

	for (i = 0; i < 20; i++) {
		entry[i] = malloc(sizeof(struct dirent *));
	}
	direntp = &entry;

	i = 0;
	while (((ent = readdir(dir)) != NULL) && (i < 20)) {
		if ((strcmp(ent->d_name, ".")) && (strcmp((ent->d_name), ".."))) {
			(*direntp)[i] = ent;
		}
		i++;
	}

	count = i;
	i = 0;
	while ((strcmp((*direntp)[i]->d_name, "model")) && i < count) 
		i++;
	print_content(dirname, (*direntp)[i]);

	i = 0;
	while ((strcmp((*direntp)[i]->d_name, "compatible")) && i < count) 
		i++;
	print_content(dirname, (*direntp)[i]);

	// print cpus
	// print psci

	closedir(dir);
}

int dtsinfo_main(int argc, char **argv)
{
	int flags = 0;
    int opt;
    while ((opt = getopt(argc, argv, "hvc")) != -1) {
        switch (opt) {
            case 'h':
            	flags |= OPT_HELP;
                break;
            case 'v':
            	flags = OPT_VERSION;
                break;
            case 'c':
                flags |= OPT_CLASSES;
                break;
        }
    }

	if (flags & OPT_HELP)
		usage();

	if (flags & OPT_VERSION)
		print_version();

	if (flags & OPT_CLASSES)
		print_classes();

	if (!flags)
		print_devicetree();

	printf("\n");
	return 0;
}

