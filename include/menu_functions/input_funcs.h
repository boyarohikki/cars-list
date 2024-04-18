#ifndef COURSE_TASK_MENU_FUNCTIONS_INPUT_FUNCS
#define COURSE_TASK_MENU_FUNCTIONS_INPUT_FUNCS
#include <stdio.h>
#include <stdlib.h>

#include "../enums.h"
#include "../getters.h"
#include "../printers.h"
#include "../structs.h"
#include "../utils.h"
#include "utils.h"

#define MAX_STR_LEN 512

/* Allocating and reading from stdin new car */
Car *input_car(const ListHead engines) {
    Car *car;
    unsigned id;
    if (engines.count) {
        car = (Car *)malloc(sizeof(Car));
        if (car) {
            car->brand = (char *)malloc(sizeof(char) * MAX_STR_LEN);
            car->model = (char *)malloc(sizeof(char) * MAX_STR_LEN);
        }
        if (!car || !car->brand || !car->model) {
            free_car((void **)&car);
        } else {
            fputs("Enter car cost and year of issue: ", stdout);
            scanf("%f %d", &car->cost, &car->year_of_issue);
            fgetc(stdin);
            fputs("Enter car brand: ", stdout);
            fgets(car->brand, 21, stdin);
            remove_last_symbol(car->brand);
            fputs("Enter car model: ", stdout);
            fgets(car->model, MAX_STR_LEN, stdin);
            remove_last_symbol(car->model);
            do {
                print_engines_list(engines);
                fputs("Enter car's engine id: ", stdout);
                scanf("%u", &id);
                fgetc(stdin);
                car->engine = (Engine *)find_element_with_id(
                    engines, id, engine_get_id, next_engine);
                if (!car->engine) {
                    printf(
                        "Engine with id=%u not found. Please, "
                        "retry.\n",
                        id);
                }
            } while (!car->engine);
            car->next = NULL;
        }
    }

    return car;
}

/* Allocating and reading from stdin new engine */
Engine *input_engine() {
    Engine *engine;

    engine = (Engine *)malloc(sizeof(Engine));
    if (engine) {
        engine->name = (char *)malloc(sizeof(char) * MAX_STR_LEN);
    }
    if (!engine || !engine->name) {
        free_engine((void **)&engine);
    } else {
        fputs("Enter engine name (up to 20 characters): ", stdout);
        fgets(engine->name, 21, stdin);
        remove_last_symbol(engine->name);
        engine->next = NULL;
        fputs("Enter engine volume and power: ", stdout);
        scanf("%f %d", &engine->volume, &engine->power);
    }

    return engine;
}

#endif