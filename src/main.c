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
	bool list = false;
	char *delstring = NULL;
	char *updatestring = NULL;  	// Comma separated value "Timmy H.,150"
	int c;
    int dbfd = -1;
	struct dbheader_t *dbhdr = NULL;
	struct employee_t *employees = NULL;

	while ((c = getopt(argc, argv, "nf:a:u:ld:")) != -1 ) {
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
			case 'l':
				list = true;
				break;
			case 'u':
				updatestring = optarg;
				break;
			case 'd':
				delstring = optarg;
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
		printf("Failed to read employees.\n");
		return 0;
	}

	if (addstring) {
		dbhdr->count++;
		employees = realloc(employees, (dbhdr->count*sizeof(struct employee_t)));
		if (add_employee(dbhdr, employees, addstring) == STATUS_ERROR) {
			printf("Adding employee failed.\n");
			return 0;
		}
	}

	if (updatestring) {
		if (update_hours(dbhdr, employees, updatestring) == STATUS_ERROR) {
			printf("Updating employee hours failed. Employee not found.\n");
			return 0;
		}	
	}

	if (delstring) {
		if (remove_employee(dbhdr, employees, delstring) == STATUS_ERROR) {
			printf("Removing employee failed. Employee not found.\n");
			return 0;
		}
		printf("Count following delete: %d\n", dbhdr->count);
	}

	if (list) {
		list_employees(dbhdr, employees);
	}

	if (output_file(dbfd, dbhdr, employees) == STATUS_ERROR) {
		return -1;
	}

	return 0;
}
