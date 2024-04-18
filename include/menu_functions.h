#ifndef COURSE_TASK_MENU_FUNCTIONS
#define COURSE_TASK_MENU_FUNCTIONS

#include <stdio.h>
#include <stdlib.h>

#include "csv_file.h"
#include "enums.h"
#include "getters.h"
#include "menu_functions/add_funcs.h"
#include "menu_functions/edit_funcs.h"
#include "menu_functions/exit_funcs.h"
#include "menu_functions/input_funcs.h"
#include "menu_functions/search_funcs.h"
#include "menu_functions/sort_funcs.h"
#include "menu_functions/utils.h"
#include "printers.h"
#include "row_to_struct.h"
#include "struct_to_row.h"
#include "structs.h"
#include "utils.h"

/* Force user to select one of variants from min_value up to max_value inclusive
 */
int select_prompt(char *prompt, int min_value, int max_value);

/* Shows short help with program usages */
void about() {
    printf(
        "... database.\n"
        "Usage:\n"
        "%d. Show this help\n"
        "%d. Add new row to the end of database table\n"
        "%d. Edit one of existing rows in database\n"
        "%d. Remove one row from database\n"
        "%d. Print all database tables one by one\n"
        "%d. Search in tables by one of properties\n"
        "%d. Print one of tables sorted by selected property\n"
        "%d. Exit from program and save (or not) changes\n",
        ABOUT, ADD, EDIT, DELETE, PRINT, SEARCH, SORT, EXIT);
}

/* Add new data to database */
Error add_new_data(ListHead *cars, ListHead *engines) {
    Error error = {OK, ""};
    int table_selection = -1;
    table_selection = select_prompt(
        "Select table to add element to:\n1. Cars\n2. Engines", 1, 2);

    switch (table_selection) {
        case 1:
            error = add_car(cars, *engines);
            break;
        case 2:
            error = add_engine(engines);
            break;
    }

    return error;
}

/* Edit data in database */
Error edit_data(ListHead cars, ListHead engines) {
    Error error = {OK, ""};
    int table_selection;
    void *element, *replacement;

    table_selection =
        select_prompt("Select table to edit data:\n1. Cars\n2. Engines", 1, 2);
    switch (table_selection) {
        case 1:
            print_cars_list(cars);
            element = select_element(cars, car_get_id, next_car, print_car);
            if (element) {
                replacement = input_car(engines);
                *car_get_id(replacement) = *car_get_id(element);
                ((Car *)replacement)->next = ((Car *)element)->next;
                free(((Car *)element)->brand);
                free(((Car *)element)->model);
                *((Car *)element) = *((Car *)replacement);
            }
            break;
        case 2:
            print_engines_list(engines);
            element = select_element(engines, engine_get_id, next_engine,
                                     print_engine);
            if (element) {
                replacement = input_engine();
                *engine_get_id(replacement) = *engine_get_id(element);
                ((Engine *)replacement)->next = ((Engine *)element)->next;
                free(((Engine *)element)->name);
                *((Engine *)element) = *((Engine *)replacement);
            }
            break;
    }
    if (replacement) {
        free(replacement);
    }
    return error;
}

/* Delete data from database */
Error delete_data(ListHead *cars, ListHead *engines) {
    Error error = {OK, ""};
    int table_selection = -1;
    unsigned id;
    void *iter;
    char found = 1;

    table_selection = select_prompt(
        "Select table to delete data from:\n1. Cars\n2. Engines", 1, 2);

    switch (table_selection) {
        case 1:
            print_cars_list(*cars);
            break;
        case 2:
            print_engines_list(*engines);
            break;
    }

    fputs("Enter id value (positive): ", stdout);
    scanf("%u", &id);
    fgetc(stdin);
    switch (table_selection) {
        case 1:
            iter = find_element_with_id(*cars, id, car_get_id, next_car);
            if (iter) {
                puts("Deleted element:");
                print_car((Car *)iter);
                delete_list_element(cars, iter, next_car, free_car);
            } else {
                iter = 0;
            }
            break;
        case 2:
            if ((iter = find_element_with_id(*cars, id, car_get_engine_id,
                                             next_car))) {
                printf(
                    "Can't delete engine with id=%u because "
                    "car with id=%u depends on it\n",
                    id, ((Car *)iter)->id);
            } else {
                iter = find_element_with_id(*engines, id, engine_get_id,
                                            next_engine);
                if (iter) {
                    puts("Deleted element:");
                    print_engine((Engine *)iter);
                    delete_list_element(engines, iter, next_engine,
                                        free_engine);
                } else {
                    found = 0;
                }
            }
            break;
    }
    if (!found) {
        printf("Element with id %u not found!\n", id);
    }
    puts("Press ENTER to continue...");
    fgetc(stdin);
    return error;
}

/* Print whole database. One table after another */
void print_data(const ListHead cars, const ListHead engines) {
    puts("\nEngines:");
    print_engines_list(engines);
    puts("\nCars:");
    print_cars_list(cars);
    puts("Press ENTER to continue...");
    fgetc(stdin);
}

/* Search in database by user selected field */
Error search_data(ListHead cars, ListHead engines) {
    Error error = {OK, ""};
    int table_selection;
    int search_criteria;
    static void *(*car_field_getters[])(void *) = {
        (void *(*)(void *))car_get_id,
        (void *(*)(void *))car_get_brand,
        (void *(*)(void *))car_get_model,
        (void *(*)(void *))car_get_year_of_issue,
        (void *(*)(void *))car_get_cost,
        (void *(*)(void *))car_get_engine_id,
    };
    static void *(*engine_field_getters[])(void *) = {
        (void *(*)(void *))engine_get_id,
        (void *(*)(void *))engine_get_name,
        (void *(*)(void *))engine_get_volume,
        (void *(*)(void *))engine_get_power,

    };
    char buf[1024];
    size_t value_size;
    char is_str, found;

    table_selection = select_prompt(
        "Select table to search data in:\n1. Cars\n2. Engines", 1, 2);
    switch (table_selection) {
        case 1:
            search_criteria = select_prompt(
                "Select criteria to search by:"
                "\n1. id"
                "\n2. Brand"
                "\n3. Model"
                "\n4. Year of issue"
                "\n5. Cost"
                "\n6. Engine id",
                1, 6);
            if (search_criteria == 6) {
                print_engines_list(engines);
            }
            fputs("Enter value:", stdout);
            switch (search_criteria) {
                case 1:
                case 6:
                    scanf("%u", (unsigned *)buf);
                    fgetc(stdin);
                    value_size = sizeof(unsigned);
                    is_str = 0;
                    break;
                case 2:
                case 3:
                    fgets(buf, sizeof(buf), stdin);
                    remove_last_symbol(buf);
                    is_str = 1;
                    break;
                case 4:
                    scanf("%d", (int *)buf);
                    fgetc(stdin);
                    value_size = sizeof(int);
                    is_str = 0;
                    break;
                case 5:
                    scanf("%f", (float *)buf);
                    fgetc(stdin);
                    value_size = sizeof(float);
                    is_str = 0;
                    break;
            }
            found = search(cars, buf, value_size,
                           car_field_getters[search_criteria - 1], next_car,
                           print_car, is_str);
            break;
        case 2:
            search_criteria = select_prompt(
                "Select criteria to search by:"
                "\n1. id"
                "\n2. Name"
                "\n3. Volume"
                "\n4. Power",
                1, 4);
            fputs("Enter value:", stdout);
            switch (search_criteria) {
                case 1:
                    scanf("%u", (unsigned *)buf);
                    fgetc(stdin);
                    is_str = 0;
                    value_size = sizeof(unsigned);
                    break;
                case 2:
                    fgets(buf, sizeof(buf), stdin);
                    remove_last_symbol(buf);
                    is_str = 1;
                    break;
                case 3:
                    scanf("%f", (float *)buf);
                    fgetc(stdin);
                    is_str = 0;
                    value_size = sizeof(float);
                    break;
                case 4:
                    scanf("%d", (int *)buf);
                    fgetc(stdin);
                    value_size = sizeof(int);
                    is_str = 0;
            }
            found = search(engines, buf, sizeof(unsigned),
                           engine_field_getters[search_criteria - 1],
                           next_engine, print_engine, is_str);
            break;
    }
    if (!found) {
        puts("Elements that meet such criteria not found");
    }
    puts("Press ENTER to continue...");
    fgetc(stdin);
    return error;
}

/* Sort database data and print sorted. Than return data to previous state */
Error sort_data(ListHead cars, ListHead engines) {
    Error error = {OK, ""};
    int table_selection, sort_criteria;
    VariableType sort_type;
    ListHead sorted = {0, NULL, NULL};
    ListElement *element;
    static void *(*car_field_getters[])(void *) = {
        (void *(*)(void *))car_get_id,
        (void *(*)(void *))car_get_brand,
        (void *(*)(void *))car_get_model,
        (void *(*)(void *))car_get_year_of_issue,
        (void *(*)(void *))car_get_cost,
        (void *(*)(void *))car_get_engine_name,
    };
    static void *(*engine_field_getters[])(void *) = {
        (void *(*)(void *))engine_get_id,
        (void *(*)(void *))engine_get_name,
        (void *(*)(void *))engine_get_volume,
        (void *(*)(void *))engine_get_power,

    };
    void (*printer)(const void *);
    void (*separator_printer)();
    void (*header_printer)();

    table_selection = select_prompt(
        "Select table to sort data:\n1. Cars\n2. "
        "Engines",
        1, 2);
    switch (table_selection) {
        case 1:
            printer = print_car;
            separator_printer = print_cars_list_separator;
            header_printer = print_cars_list_header;
            sort_criteria = select_prompt(
                "Select criteria to sort by:"
                "\n1. id"
                "\n2. Brand"
                "\n3. Model"
                "\n4. Year of issue"
                "\n5. Cost"
                "\n6. Engine name",
                1, 6);
            switch (sort_criteria) {
                case 1:
                    sort_type = UNSIGNED;
                    break;
                case 5:
                    sort_type = INT;
                    break;
                case 2:
                case 3:
                case 6:
                    sort_type = STRING;
                    break;
                case 4:
                    sort_type = FLOAT;
                    break;
            }
            error =
                sort_list(cars, &sorted, car_field_getters[sort_criteria - 1],
                          next_car, sort_type);
            break;
        case 2:
            printer = print_engine;
            separator_printer = print_engines_list_separator;
            header_printer = print_engines_list_header;
            sort_criteria = select_prompt(
                "Select criteria to sort by:"
                "\n1. id"
                "\n2. Name"
                "\n3. Volume"
                "\n4. Power",
                1, 4);
            switch (sort_criteria) {
                case 1:
                    sort_type = UNSIGNED;
                    break;
                case 2:
                    sort_type = STRING;
                    break;
                case 3:
                    sort_type = FLOAT;
                    break;
                case 4:
                    sort_type = INT;
                    break;
            }
            error = sort_list(engines, &sorted,
                              engine_field_getters[sort_criteria - 1],
                              next_engine, sort_type);
            break;
    }
    if (error.status_code == OK) {
        puts("Sorted:");
        element = (ListElement *)sorted.first;
        separator_printer();
        header_printer();
        separator_printer();
        while (element) {
            printer(*list_element_get_data(element));
            element = (ListElement *)*next_list_element(element);
        }
        separator_printer();
    }
    free_list(&sorted, free_list_element, next_list_element);
    puts("Press ENTER to continue...");
    fgetc(stdin);
    return error;
}

/* Exit from program and save data to database if user wants*/
Error exit_handler(ListHead cars, ListHead engines) {
    Error error = {OK, ""};
    char confirm;
    do {
        fputs("Save changes (y/n): ", stdout);
        confirm = fgetc(stdin);
        fgetc(stdin);
        if (confirm != 'y' && confirm != 'n') {
            printf("Unknown selection: %c. Please, retry.\n", confirm);
        }
    } while (confirm != 'y' && confirm != 'n');
    if (confirm == 'y') {
        error = save_list_to_file(engines, ENGINE_FILENAME, 4, engine_to_row,
                                  next_engine);
        if (error.status_code == OK) {
            error =
                save_list_to_file(cars, CAR_FILENAME, 6, car_to_row, next_car);
        }
    }
    return error;
}

int select_prompt(char *prompt, int min_value, int max_value) {
    int table_selection;
    do {
        puts(prompt);
        scanf("%d", &table_selection);
        fgetc(stdin);
        if (table_selection < min_value || table_selection > max_value) {
            printf("Wrong selection: %d. Please, retry\n", table_selection);
        }
    } while (table_selection < min_value || table_selection > max_value);

    return table_selection;
}

#endif