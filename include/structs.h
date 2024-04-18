#ifndef COURSE_TASK_STRUCTS
#define COURSE_TASK_STRUCTS
#include <stddef.h>

#include "enums.h"

typedef struct Error {
    StatusCode status_code;
    char *info;
} Error;

typedef struct Engine {
    unsigned id;
    char *name;
    float volume;
    int power;
    struct Engine *next;
} Engine;

typedef struct Car {
    unsigned id;
    char *brand;
    char *model;
    int year_of_issue; /* In days */
    float cost;
    Engine *engine;
    struct Car *next;
} Car;

typedef struct ListHead {
    size_t count; /* List elements count */
    void *first;
    void *last;
} ListHead;

typedef struct ListElement {
    void *data;
    void *next;
} ListElement;

/* Frees Car and sets it's pointer to NULL */
void free_car(void **car) {
    Car *car_struct = *((Car **)car);
    if (car_struct) {
        if (car_struct->brand) {
            free(car_struct->brand);
        }
        if (car_struct->model) {
            free(car_struct->model);
        }
        free(car_struct);
        *car = NULL;
    }
}

/* Frees Engine and sets it's pointer to NULL */
void free_engine(void **engine) {
    Engine *engine_struct = *((Engine **)engine);
    if (engine_struct) {
        if (engine_struct->name) {
            free(engine_struct->name);
        }
        free(engine_struct);
        *engine = NULL;
    }
}

/* Frees ListElement and sets it's pointer to NULL */
void free_list_element(void **list_element) {
    if (*list_element) {
        free(*list_element);
        *list_element = NULL;
    }
}

/* Universal list free function. Uses free_func to free one element and
 * next_func to iterate over elements*/
void free_list(ListHead *list, void (*free_func)(void **),
               void **(*next_func)(void *)) {
    while ((list->last = list->first)) {
        list->first = *next_func(list->first);
        free_func(&list->last);
    }
    list->first = NULL;
    list->last = NULL;
    list->count = 0;
}

#endif