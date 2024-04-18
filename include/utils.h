#ifndef COURSE_TASK_UTILS
#define COURSE_TASK_UTILS

#include <stdio.h>
#include <stdlib.h>

#include "csv_file.h"
#include "csv_file_to_list.h"
#include "enums.h"
#include "structs.h"

/* Force usage of Linux's and MacOS clear command*/
/* #define FORCE_UNIX */

/* Force usage of Windows's clear command*/
/* #define FORCE_WINDOWS */

void clear_screen() {
#if defined(__linux__) || defined(FORCE_UNIX)
    system("clear");
#endif
#if defined(_WIN32) || defined(FORCE_WINDOWS)
    system("cls");
#endif
}

/* If file not exists, creates it, then open in binary update mode (r+b) and
 * seek start of file
 */
FILE *open_or_create_file(char *filename) {
    StatusCode error = OK;
    FILE *f;
    f = fopen(filename, "rb");
    if (f) {
        fclose(f);
    } else {
        f = fopen(filename, "wb");
        if (f) {
            fclose(f);
        } else {
            error = FILE_OPEN_ERROR;
        }
    }
    if (error == OK) {
        f = fopen(filename, "r+b");
        fseek(f, 0, SEEK_SET);
    } else {
        f = NULL;
    }
    return f;
}

/* Wrapper above CSVFileRead and row_converter. Reads file and converts it into
 * list via row_converter function. If file not exists it creates empty file.
 * element_size is size of one element of output, so it must be
 * sizeof(ElementType) */
Error csv_file_read(char *filename, ListHead *output, size_t element_size,
                    void **(*next_getter)(void *), void (*free_func)(void **),
                    Error (*row_converter)(CSVFileLine, void *output, void *),
                    void *additional_args) {
    FILE *f;
    Error error = {OK, ""};
    CSVFile csv = {0, 0, NULL};
    int csv_status;

    f = open_or_create_file(filename);
    if (f) {
        csv_status = CSVFileRead(f, &csv);
        if (csv_status != CSV_SUCCESS) {
            error.info = "Error parsing csv file";
            error.status_code = FILE_READ_ERROR;
        }
        fclose(f);
    } else {
        error.info = "Can't open csv file";
        error.status_code = FILE_OPEN_ERROR;
    }

    if (error.status_code == OK) {
        error = csv_file_to_list(csv, output, element_size, next_getter,
                                 free_func, row_converter, additional_args);
        CSVFileFree(&csv);
    }

    return error;
}

/* Removes last symbol from string by setting it to 0 */
void remove_last_symbol(char *str) {
    size_t len = strlen(str);
    if (len > 0) {
        str[len - 1] = 0;
    }
}

/* Removes leftover '\n' symbols from stdin */
void flush_stdin() {
    char c;
    while ((c = getchar()) != '\n' && c != EOF)
        ;
}

#endif