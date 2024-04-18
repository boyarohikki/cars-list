#ifndef COURSE_TASK_MENU_FUNCTIONS_ADD_FUNCS
#define COURSE_TASK_MENU_FUNCTIONS_ADD_FUNCS
#include <stdio.h>
#include <stdlib.h>

#include "../enums.h"
#include "../printers.h"
#include "../structs.h"
#include "../utils.h"
#include "input_funcs.h"
#include "utils.h"

/* Adds new car to the end of cars list */
Error add_car(ListHead *cars, const ListHead engines) {
    Error error = {OK, ""};
    Car *new_car;
    if (engines.count) {
        new_car = input_car(engines);

        if (new_car) {
            *next_car(cars->last) = new_car;
            new_car->id = ((Engine *)cars->last)->id + 1;
            cars->last = new_car;
            cars->count++;
        } else {
            error.status_code = MEMORY_ALLOCATION_ERROR;
            error.info = "Can't allocate memory for car";
        }
    } else {
        puts("No engines. Can't add new car!");
        fgetc(stdin);
    }

    return error;
}

/* Adds engine type to the end of engines list */
Error add_engine(ListHead *engines) {
    Engine *new_engine;
    Error error = {OK, ""};

    new_engine = input_engine();
    if (new_engine) {
        *next_engine(engines->last) = new_engine;
        new_engine->id = ((Engine *)engines->last)->id + 1;
        engines->last = new_engine;
        engines->count++;
    } else {
        error.status_code = MEMORY_ALLOCATION_ERROR;
        error.info = "Can't allocate memory for engine";
    }

    return error;
}

#endif