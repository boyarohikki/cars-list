#ifndef COURSE_TASK_STRUCT_TO_ROW
#define COURSE_TASK_STRUCT_TO_ROW

#include "csv_row_converters/csv_row_converters.h"
#include "enums.h"
#include "structs.h"

/* Converts Car to CSVFileLine */
Error car_to_row(void *car, CSVFileLine *row) {
    Car *car_struct = (Car *)car;
    return create_row(row, "%u%s%s%d%f%u", car_struct->id, car_struct->brand,
                      car_struct->model, car_struct->year_of_issue,
                      car_struct->cost, car_struct->engine->id);
}

/* Converts Engine to CSVFileLine */
Error engine_to_row(void *engine, CSVFileLine *row) {
    Engine *engine_struct = (Engine *)engine;
    return create_row(row, "%u%s%f%d", engine_struct->id, engine_struct->name,
                      engine_struct->volume, engine_struct->power);
}

#endif