#ifndef COURSE_TASK_PRINTERS
#define COURSE_TASK_PRINTERS
#include <stdio.h>

#include "structs.h"

/* Print single car */
void print_car(const void *car) {
    const Car *car_struct = car;
    printf("| %5u | %20s | %40s | %10.2f | %8d | %20s |\n", car_struct->id,
           car_struct->brand, car_struct->model, car_struct->cost,
           car_struct->year_of_issue, car_struct->engine->name);
}

/* Print cars list separator. Also used as table top and bottom borders */
void print_cars_list_separator() {
    puts(
        "+-------+----------------------+--------------------------------------"
        "----+------------+----------+----------------------+");
}

/* Print cars table header */
void print_cars_list_header() {
    printf("| %5s | %20s | %40s | %10s | %8s | %20s |\n", "ID", "Brand",
           "Model", "Cost", "Issue", "Engine");
}

/* Print list of cars */
void print_cars_list(const ListHead cars) {
    Car *car = cars.first;
    print_cars_list_separator();
    print_cars_list_header();
    print_cars_list_separator();
    while (car) {
        print_car(car);
        car = car->next;
    }
    print_cars_list_separator();
}

/* Print single engine */
void print_engine(const void *engine) {
    const Engine *engine_struct = engine;
    printf("| %5u | %20s | %10.2f | %5d |\n", engine_struct->id,
           engine_struct->name, engine_struct->volume, engine_struct->power);
}

/* Print engines list separator. Also used as table top and bottom borders */
void print_engines_list_separator() {
    puts("+-------+----------------------+------------+-------+");
}

/* Print engines table header */
void print_engines_list_header() {
    printf("| %5s | %20s | %10s | %5s |\n", "ID", "Name", "Volume", "Power");
}

/* Print engines table */
void print_engines_list(const ListHead engines) {
    Engine *engine = engines.first;
    print_engines_list_separator();
    print_engines_list_header();
    print_engines_list_separator();
    while (engine) {
        print_engine(engine);
        engine = engine->next;
    }
    print_engines_list_separator();
}

#endif
