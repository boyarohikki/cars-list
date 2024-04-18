#ifndef COURSE_TASK_GETTERS
#define COURSE_TASK_GETTERS
#include "structs.h"

/* Getters for created structures */

/* Car field getters*/
unsigned *car_get_engine_id(void *car) { return &((Car *)car)->engine->id; }

char **car_get_engine_name(void *car) { return &((Car *)car)->engine->name; }

float *car_get_cost(void *car) { return &((Car *)car)->cost; }

char **car_get_model(void *car) { return &((Car *)car)->model; }

unsigned *car_get_id(void *car) { return &((Car *)car)->id; }

char **car_get_brand(void *car) { return &((Car *)car)->brand; }

int *car_get_year_of_issue(void *car) { return &((Car *)car)->year_of_issue; }

void **next_car(void *car) { return (void **)&((Car *)car)->next; }

/* Engine field getters*/
unsigned *engine_get_id(void *engine) { return &((Engine *)engine)->id; }

char **engine_get_name(void *engine) { return &((Engine *)engine)->name; }

float *engine_get_volume(void *engine) { return &((Engine *)engine)->volume; }

int *engine_get_power(void *engine) { return &((Engine *)engine)->power; }

void **next_engine(void *engine) { return (void **)&((Engine *)engine)->next; }

/* ListElement field getters */
void **list_element_get_data(void *list_element) {
    return (void **)&((ListElement *)list_element)->data;
}
void **next_list_element(void *list_element) {
    return (void **)&((ListElement *)list_element)->next;
}

#endif