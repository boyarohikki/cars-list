#ifndef COURSE_TASK_MENU_FUNCTIONS_EXIT_FUNCS
#define COURSE_TASK_MENU_FUNCTIONS_EXIT_FUNCS

#include <stdio.h>

#include "../csv_file.h"
#include "../enums.h"
#include "../structs.h"

/* Saves list to file. Uses element_to_row convertation function to create
row from list element, next_getter to go through list. element_fields_count is
used just to set CSVFile's fields_count parameter */
Error save_list_to_file(ListHead list, char *output_filename,
                        size_t element_fields_count,
                        Error (*element_to_row)(void *element,
                                                CSVFileLine *row),
                        void **(*next_getter)(void *element)) {
    Error error = {OK, ""};
    void *element;
    CSVFile csv = {0, 0, NULL};
    FILE *output;
    size_t i;

    csv.fields_count = element_fields_count;

    if (error.status_code == OK) {
        csv.lines = (CSVFileLine *)malloc(sizeof(CSVFileLine) * list.count);
        if (csv.lines) {
            csv.lines_count = list.count;
        } else {
            error.status_code = MEMORY_ALLOCATION_ERROR;
            error.info = "Can't allocate memory for csv output struct";
            csv.lines_count = 0;
        }
    }
    element = list.first;
    i = 0;
    while (element && error.status_code == OK) {
        error = element_to_row(element, csv.lines + i++);
        if (error.status_code != OK) {
            csv.lines_count = i - 1;
        }
        element = *next_getter(element);
    }

    if (error.status_code == OK) {
        output = fopen(output_filename, "wb");
        if (output) {
            CSVFileWrite(output, csv);
            fclose(output);
        } else {
            error.status_code = FILE_OPEN_ERROR;
            error.info = "Can't open output file for write";
        }
    }

    CSVFileFree(&csv);

    return error;
}

#endif