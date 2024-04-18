#ifndef COURSE_TASK_MENU_FUNCTIONS_SORT_FUNCS
#define COURSE_TASK_MENU_FUNCTIONS_SORT_FUNCS

#include <stdlib.h>
#include <string.h>

#include "../enums.h"
#include "../getters.h"
#include "../structs.h"

/* Compares two elements based on their type. Returns -1 if value1 < value2,
0 if value1 == value2 and 1 if value1 > value2 */
int compare(void *value1, void *value2, VariableType type);

/* List universal sorter. Returns list of ListElement elements.
That elements' data fields are pointers to to_sort array elements */
Error sort_list(ListHead to_sort, ListHead *output,
                void *(*field_getter)(void *), void **(*next_getter)(void *),
                VariableType type) {
    Error error = {OK, ""};
    void *element = NULL, *element2 = NULL;
    void *temp = NULL;

    output->count = 0;
    output->first = NULL;
    output->last = NULL;

    /* Copy elements to output array */
    element = to_sort.first;
    while (element && error.status_code == OK) {
        if (output->last) {
            *next_list_element(output->last) = malloc(sizeof(ListElement));
            output->last = *next_list_element(output->last);
        } else {
            output->last = malloc(sizeof(ListElement));
            output->first = output->last;
        }
        if (output->last) {
            *list_element_get_data(output->last) = element;
            output->count++;
        } else {
            error.status_code = MEMORY_ALLOCATION_ERROR;
            error.info = "Can't allocate memory for new list element";
        }
        element = *next_getter(element);
    }
    *next_list_element(output->last) = NULL;

    /* Sort elements by selected field (bubble sort) */
    if (error.status_code == OK) {
        element = output->first;
        while (element) {
            element2 = *next_list_element(element);
            while (element2) {
                if (compare(field_getter(*list_element_get_data(element)),
                            field_getter(*list_element_get_data(element2)),
                            type) > 0) {
                    temp = *list_element_get_data(element);
                    *list_element_get_data(element) =
                        *list_element_get_data(element2);
                    *list_element_get_data(element2) = temp;
                }
                element2 = *next_list_element(element2);
            }
            element = *next_list_element(element);
        }
    } else {
        free_list(output, free_list_element, next_list_element);
    }

    return error;
}

int compare(void *value1, void *value2, VariableType type) {
    int result;
    switch (type) {
        case CHAR:
            result = *((char *)value1) > *((char *)value2)
                         ? 1
                         : (*((char *)value1) < *((char *)value2) ? -1 : 0);
            break;
        case DOUBLE:
            result = *((double *)value1) > *((double *)value2)
                         ? 1
                         : (*((double *)value1) < *((double *)value2) ? -1 : 0);
            break;
        case FLOAT:
            result = *((float *)value1) > *((float *)value2)
                         ? 1
                         : (*((float *)value1) < *((float *)value2) ? -1 : 0);
            break;
        case INT:
            result = *((int *)value1) > *((int *)value2)
                         ? 1
                         : (*((int *)value1) < *((int *)value2) ? -1 : 0);
            break;
        case STRING:
            result = strcmp(*((char **)value1), *((char **)value2));
            break;
        case UNSIGNED:
            result =
                *((unsigned *)value1) > *((unsigned *)value2)
                    ? 1
                    : (*((unsigned *)value1) < *((unsigned *)value2) ? -1 : 0);
            break;
    }
    return result;
}

#endif