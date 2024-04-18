#ifndef COURSE_TASK_CSV_FILE_TO_LIST
#define COURSE_TASK_CSV_FILE_TO_LIST
#include <stdlib.h>

#include "csv_file.h"
#include "enums.h"
#include "row_to_struct.h"
#include "structs.h"

/* Converts CSVFile csv to list by converting each row of csv via row_converter
function. free_func is used if error occured.

Element size passed to malloc, so it must be sizeof(ElementType) */
Error csv_file_to_list(CSVFile csv, ListHead *output, size_t element_size,
                       void **(*next_getter)(void *),
                       void (*free_func)(void **),
                       Error (*row_converter)(CSVFileLine, void *, void *),
                       void *additional_args) {
    Error error = {OK, ""};
    size_t i;

    output->first = NULL;
    output->last = NULL;
    output->count = 0;
    for (i = 0; i < csv.lines_count && error.status_code == OK; i++) {
        if (output->first) {
            *next_getter(output->last) = malloc(element_size);
            output->last = *next_getter(output->last);
        } else {
            output->last = malloc(element_size);
            output->first = output->last;
        }
        if (output->last) {
            *next_getter(output->last) = NULL;
            error = row_converter(csv.lines[i], output->last, additional_args);
        } else {
            error.status_code = MEMORY_ALLOCATION_ERROR;
            error.info = "Can't allocate memory for csv file's lines";
        }
    }

    if (error.status_code == MEMORY_ALLOCATION_ERROR) {
        free_list(output, free_func, next_getter);
    } else if (error.status_code == OK) {
        output->count = csv.lines_count;
    }

    return error;
}

#endif