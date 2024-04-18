#ifndef COURSE_TASK_CSV_ROW_CONVERTERS_UTILS
#define COURSE_TASK_CSV_ROW_CONVERTERS_UTILS

#define COURSE_TASK_SPECIFIER_TYPE \
    enum Specifier { c = 0, d, f, hd, hu, ld, lf, lu, s, u }

#include <stdlib.h>

#include "../enums.h"
#include "../structs.h"

int specifier_to_enum(char specifier[3]);

/* Converts format string to specifier enum array
output is specifier array
count is count of specifiers */
Error format_str_to_enum_list(const char *format, int **output, size_t *count) {
    COURSE_TASK_SPECIFIER_TYPE;
    char buf[3];
    size_t allocated, i, j;
    int temp, *temp_specifiers_ptr;
    Error error = {OK, ""};

    *count = 0;
    allocated = 100;
    *output = (int *)malloc(sizeof(enum Specifier) * 100);
    if (!*output) {
        error.status_code = MEMORY_ALLOCATION_ERROR;
        error.info = "Can't allocate memory for specifiers array";
    }

    i = 0;
    j = 0;
    while (format[i] && error.status_code == OK) {
        if (format[i] != '%') {
            buf[j++] = format[i];
            if (j > 2) {
                error.status_code = UNKNOWN;
                error.info = "Wrong format";
            }
        }
        if (error.status_code == OK &&
            (format[i] == '%' || format[i + 1] == 0)) {
            if (j != 0) {
                buf[j] = 0;
                j = 0;
                temp = specifier_to_enum(buf);
                if (temp == -1) {
                    error.status_code = UNKNOWN;
                    error.info = "Wrong format";
                } else {
                    if (*count == allocated) {
                        allocated *= 2;
                        temp_specifiers_ptr = (int *)realloc(
                            *output, allocated * sizeof(enum Specifier));
                        if (temp_specifiers_ptr) {
                            *output = temp_specifiers_ptr;
                        } else {
                            free(*output);
                            *output = NULL;
                            error.status_code = MEMORY_ALLOCATION_ERROR;
                            error.info =
                                "Can't realloc more memory for specifiers "
                                "array";
                        }
                    }
                    if (error.status_code == OK) {
                        (*output)[*count] = temp;
                        (*count)++;
                    }
                }
            }
        }
        i++;
    }

    return error;
}

size_t specifier_to_size(int int_specifier) {
    COURSE_TASK_SPECIFIER_TYPE specifier = (enum Specifier)int_specifier;
    size_t size;

    switch (specifier) {
        case c:
            size = sizeof(char);
            break;
        case d:
            size = sizeof(int);
            break;
        case f:
            size = sizeof(float);
            break;
        case s:
            size = sizeof(char *);
            break;
        case u:
            size = sizeof(unsigned);
            break;
        case hd:
            size = sizeof(short);
            break;
        case hu:
            size = sizeof(unsigned short);
            break;
        case ld:
            size = sizeof(long);
            break;
        case lu:
            size = sizeof(unsigned long);
            break;
        case lf:
            size = sizeof(double);
            break;
    }

    return size;
}

int specifier_to_enum(char specifier[3]) {
    size_t i;
    COURSE_TASK_SPECIFIER_TYPE specifier_enum = -1;

    i = 0;

    switch (specifier[i]) {
        case 'c':
            specifier_enum = c;
            break;
        case 'd':
            specifier_enum = d;
            break;
        case 'e':
        case 'f':
        case 'g':
            specifier_enum = f;
            break;
        case 's':
            specifier_enum = s;
            break;
        case 'u':
            specifier_enum = u;
            break;
        case 'h':
            i++;
            switch (specifier[i]) {
                case 'd':
                    specifier_enum = hd;
                    break;
                case 'u':
                    specifier_enum = hu;
                    break;
            }
            break;
        case 'l':
            i++;
            switch (specifier[i]) {
                case 'd':
                    specifier_enum = ld;
                    break;
                case 'u':
                    specifier_enum = lu;
                    break;
                case 'e':
                case 'f':
                case 'g':
                    specifier_enum = lf;
                    break;
            }
            break;
    }

    return specifier_enum;
}

#endif