/* VERY strange code that uses enum inside macro to not mess with global
 * namespace*/
#ifndef COURSE_TASK_CSV_ROW_CONVERTERS
#define COURSE_TASK_CSV_ROW_CONVERTERS

#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdarg.h>
#include <string.h>

#include "../csv_file.h"
#include "../enums.h"
#include "../structs.h"
#include "utils.h"

/* Use standard format specifiers like %s, %c %d etc.
 * Format example: "%s%d%c%d%d".
 * This function doesn't check fields count.
 * Supported specifiers:
 * c
 * d
 * e
 * f
 * g
 * hu
 * hd
 * ld
 * le
 * lf
 * lg
 * lu
 * s
 * u
 */

/* Works as scanf except for char* - it needs char** to auto malloc memory.
Support only type specifiers, no separators and other format specifiers — only
such that listed in first forty lines.
*/
Error scan_row(CSVFileLine row, const char *format, ...) {
    COURSE_TASK_SPECIFIER_TYPE *specifiers = NULL;
    size_t count, i;
    Error error;
    va_list args;
    char **str;
    int temp;

    va_start(args, format);
    error = format_str_to_enum_list(format, (int **)&specifiers, &count);
    for (i = 0; i < count && error.status_code == OK; i++) {
        switch (specifiers[i]) {
            case c:
                temp = sscanf(row.fields[i], "%c", va_arg(args, char *));
                break;
            case d:
                temp = sscanf(row.fields[i], "%d", va_arg(args, int *));
                break;
            case f:
                temp = sscanf(row.fields[i], "%f", va_arg(args, float *));
                break;
            case hd:
                temp = sscanf(row.fields[i], "%hd", va_arg(args, short int *));
                break;
            case hu:
                temp = sscanf(row.fields[i], "%hu",
                              va_arg(args, short unsigned *));
                break;
            case ld:
                temp = sscanf(row.fields[i], "%ld", va_arg(args, long *));
                break;
            case lf:
                temp = sscanf(row.fields[i], "%lf", va_arg(args, double *));
                break;
            case lu:
                temp =
                    sscanf(row.fields[i], "%lu", va_arg(args, long unsigned *));
                break;
            case s:
                temp = 1;
                str = va_arg(args, char **);
                *str =
                    (char *)malloc(sizeof(char) * (strlen(row.fields[i]) + 1));
                if (*str) {
                    strcpy(*str, row.fields[i]);
                } else {
                    error.status_code = MEMORY_ALLOCATION_ERROR;
                    error.info =
                        "Can't allocate memory for one of csv str "
                        "fields";
                }
                break;
            case u:
                temp = sscanf(row.fields[i], "%u", va_arg(args, unsigned *));
                break;
        }

        if (temp != 1) {
            error.status_code = PARSE_ERROR;
            error.info = "Can't parse one of arguments in scan row";
        }
    }
    va_end(args);

    if (specifiers) {
        free(specifiers);
    }

    return error;
}

/* Works as printf, but output is row's fields. For format restrictions check
scan_row function documentation */
Error create_row(CSVFileLine *row, const char *format, ...) {
    COURSE_TASK_SPECIFIER_TYPE *specifiers = NULL;
    size_t count, i, length;
    Error error;
    va_list args;
    char *str;
    int temp;

    va_start(args, format);
    error = format_str_to_enum_list(format, (int **)&specifiers, &count);
    if (error.status_code == OK) {
        row->fields = malloc(sizeof(char *) * count);
        if (!row->fields) {
            error.status_code = MEMORY_ALLOCATION_ERROR;
            error.info = "Can't allocate memory for row";
        }
    }
    for (i = 0; i < count && error.status_code == OK; i++) {
        /* Calculation of field's length to guarantely contain value */
        switch (specifiers[i]) {
            case c:
                length = 1;
                break;
            case s:
                str = va_arg(args, char *);
                length = strlen(str) + 1;
                break;
            case lf:
                length = DBL_MAX_10_EXP + 2;
                break;
            case f:
                length = FLT_MAX_10_EXP + 2;
                break;
            case d:
            case hd:
            case hu:
            case ld:
            case lu:
            case u:
                /* Threat other integer types as unsigned longs */
                length = (int)ceil(log10(ULONG_MAX) + 0.5) + 2;
                break;
            default:
                error.status_code = PARSE_ERROR;
                error.info = "Unknown specifier found";
                length = 0;
                break;
        }
        if (error.status_code == OK) {
            row->fields[i] = malloc(sizeof(char) * (length + 1));
            if (!row->fields[i]) {
                error.status_code = MEMORY_ALLOCATION_ERROR;
                error.info =
                    "Can't allocate memory for one of csv str "
                    "fields";
            }
        }
        if (error.status_code == OK) {
            switch (specifiers[i]) {
                case c:
                    temp = sprintf(row->fields[i], "%c", va_arg(args, int));
                    break;
                case d:
                    temp = sprintf(row->fields[i], "%d", va_arg(args, int));
                    break;
                case f:
                    temp = sprintf(row->fields[i], "%f", va_arg(args, double));
                    break;
                case hd:
                    temp = sprintf(row->fields[i], "%hd", va_arg(args, int));
                    break;
                case hu:
                    temp = sprintf(row->fields[i], "%hu", va_arg(args, int));
                    break;
                case ld:
                    temp = sprintf(row->fields[i], "%ld", va_arg(args, long));
                    break;
                case lf:
                    temp = sprintf(row->fields[i], "%f", va_arg(args, double));
                    break;
                case lu:
                    temp = sprintf(row->fields[i], "%lu",
                                   va_arg(args, long unsigned));
                    break;
                case s:
                    temp = 1;
                    strcpy(row->fields[i], str);
                    break;
                case u:
                    temp =
                        sprintf(row->fields[i], "%u", va_arg(args, unsigned));
                    break;
                default:
                    temp = 0;
                    break;
            }

            if (temp == 0) {
                error.status_code = PARSE_ERROR;
                error.info = "Can't parse one of arguments in scan row";
            }
        }
    }
    va_end(args);

    if (error.status_code != OK) {
        for (; i > 0; i--) {
            if (row->fields[i - 1] != NULL) {
                free(row->fields[i - 1]);
            }
        }
        free(row->fields);
        row->fields = NULL;
    }

    if (specifiers) {
        free(specifiers);
    }

    return error;
}

#endif