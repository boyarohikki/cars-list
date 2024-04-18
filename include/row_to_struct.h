#ifndef COURSE_TASK_ROW_TO_STRUCT
#define COURSE_TASK_ROW_TO_STRUCT

#include <stdarg.h>

#include "csv_file.h"
#include "csv_row_converters/csv_row_converters.h"
#include "enums.h"
#include "structs.h"

/* Converts CSVFileLine to Engine structure */
Error row_to_engine(CSVFileLine row, void *engine, ...) {
    Engine *pseudo = engine;
    return scan_row(row, "%u%s%f%d", &pseudo->id, &pseudo->name,
                    &pseudo->volume, &pseudo->power);
}

/* Converts CSVFileLine to Car structure */
Error row_to_car(CSVFileLine row, void *car, void *additional_args) {
    ListHead engines;
    unsigned engine_id;
    Engine *engine = NULL;
    Error error;
    Car *pseudo = car;

    engines = *(ListHead *)additional_args;

    error = scan_row(row, "%u%s%s%d%f%u", &pseudo->id, &pseudo->brand,
                     &pseudo->model, &pseudo->year_of_issue, &pseudo->cost,
                     &engine_id);
    if (error.status_code == OK) {
        pseudo->engine = NULL;
        engine = (Engine *)engines.first;
        while (engine) {
            if (engine->id == engine_id) {
                pseudo->engine = engine;
                engine = NULL;
            } else {
                engine = engine->next;
            }
        }

        if (pseudo->engine == NULL) {
            error.status_code = PARSE_ERROR;
            error.info = "Engine id not found in database";
        }
    }
    return error;
}

#endif