#include <stdio.h>
#include <stdlib.h>

#include "include/csv_file.h"
#include "include/csv_file_to_list.h"
#include "include/csv_row_converters/csv_row_converters.h"
#include "include/enums.h"
#include "include/menu_functions.h"
#include "include/row_to_struct.h"
#include "include/structs.h"
#include "include/utils.h"

Error main_menu(ListHead *cars, ListHead *engines);

int main() {
    Error error = {OK, ""};
    ListHead engines = {0, NULL, NULL}, cars = {0, NULL, NULL};

    /* Database reading */
    if (error.status_code == OK) {
        error = csv_file_read(
            ENGINE_FILENAME, &engines, sizeof(Engine), next_engine, free_engine,
            (Error(*)(CSVFileLine, void *output, void *))row_to_engine, NULL);
    }

    if (error.status_code == OK) {
        error = csv_file_read(CAR_FILENAME, &cars, sizeof(Car), next_car,
                              free_car, row_to_car, &engines);
    }

    if (error.status_code == OK) {
        print_data(cars, engines);
        error = main_menu(&cars, &engines);
    }

    switch (error.status_code) {
        case OK:
            break;
        case MEMORY_ALLOCATION_ERROR:
            fputs("Error in memory alocation: ", stderr);
            fputs(error.info, stderr);
            fputc('\n', stderr);
            break;
        case FILE_OPEN_ERROR:
        case FILE_READ_ERROR:
        case UNKNOWN:
        case PARSE_ERROR:
            fputs(error.info, stderr);
            fputc('\n', stderr);
            break;
    }

    free_list(&cars, free_car, next_car);
    free_list(&engines, free_engine, next_engine);

    return 0;
}

Error main_menu(ListHead *cars, ListHead *engines) {
    MenuSelection menu_selection;
    Error error = {OK, ""};

    do {
        about();
        fputs("Select operation: ", stdout);
        scanf("%d", (int *)&menu_selection);
        fgetc(stdin);
        switch (menu_selection) {
            case ABOUT:
                break;
            case ADD:
                error = add_new_data(cars, engines);
                break;
            case EDIT:
                error = edit_data(*cars, *engines);
                break;
            case DELETE:
                error = delete_data(cars, engines);
                break;
            case PRINT:
                print_data(*cars, *engines);
                break;
            case SEARCH:
                error = search_data(*cars, *engines);
                break;
            case SORT:
                error = sort_data(*cars, *engines);
                break;
            case EXIT:
                error = exit_handler(*cars, *engines);
                break;
            default:
                printf("Unknown selection (%d), please retry\n",
                       menu_selection);
                break;
        }
        if (error.status_code != OK) {
            fgetc(stdin);
        }

        clear_screen();
    } while (menu_selection != EXIT && error.status_code == OK);

    return error;
}