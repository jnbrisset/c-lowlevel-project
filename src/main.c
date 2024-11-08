#include <stdio.h>
#include <stdbool.h>
#include <getopt.h>
#include <stdlib.h>

#include "common.h"
#include "file.h"
#include "parse.h"

void print_usage(char *argv[]) {
	printf("Usage: %s -n -f <database file>\n", argv[0]);
	printf("\t -n   -   create new database\n");
	printf("\t -f   -   (required) path to database file\n");
	return;
}

int main(int argc, char *argv[]) {
	char *filepath = NULL;
	bool newfile = false;
	char *addstring = NULL;
	int c;
    int dbfd = -1;
	struct dbheader_t *dbhdr = NULL;
	struct employee_t *employees = NULL;

	while ((c = getopt(argc, argv, "nf:a:")) != -1 ) {
		switch (c) {
			case 'n':
				newfile = true;
				break;
			case 'f':
				filepath = optarg;
				break;
			case 'a':
				addstring = optarg;
				break;
			case '?':
				printf("Unknown option -%c\n", c);
				break;	
			default:
				return -1;
		}
	}	

	if (filepath == NULL) {
		printf("Filepath is a required argument.\n");
		print_usage(argv);
        return 0;
	}
	
    if (newfile) {
        dbfd = create_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to create database file.\n");
            return -1;
        }

		if (create_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
			printf("Failed to create database header.\n");
			return -1;
		}
    } else {
        dbfd = open_db_file(filepath);
        if (dbfd == STATUS_ERROR) {
            printf("Unable to open database file.\n");
            return -1;
        }

		if (validate_db_header(dbfd, &dbhdr) == STATUS_ERROR) {
			printf("Failed to validate database header.");
			return -1;
		}
    }

	if (read_employees(dbfd, dbhdr, &employees) != STATUS_SUCCESS) {
		printf("failed to read employees.\n");
		return 0;
	}

	if (addstring) {
		dbhdr->count++;
		employees = realloc(employees, (dbhdr->count*sizeof(struct employee_t)));
		add_employee(dbhdr, employees, addstring);
	}

	if (output_file(dbfd, dbhdr, NULL) == STATUS_ERROR) {
		return -1;
	}

	return 0;
}
