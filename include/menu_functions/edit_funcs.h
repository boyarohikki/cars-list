#ifndef COURSE_TASK_MENU_FUNCTIONS_EDIT_FUNCS
#define COURSE_TASK_MENU_FUNCTIONS_EDIT_FUNCS
#include <stdio.h>
#include <stdlib.h>

#include "../enums.h"
#include "../printers.h"
#include "../structs.h"
#include "../utils.h"
#include "utils.h"

/* Returns pointer to element with id entered by user */
void *select_element(const ListHead list, unsigned *(*id_getter)(void *),
                     void **(next_getter)(void *),
                     void (*printer)(const void *)) {
    char confirm;
    void *element = NULL;
    unsigned id;

    do {
        fputs("Enter id: ", stdout);
        scanf("%u", &id);
        fgetc(stdin);
        element = find_element_with_id(list, id, id_getter, next_getter);
        if (element) {
            puts("You are going to edit next element:");
            printer(element);
            puts("Continue (y/n)?");
            confirm = fgetc(stdin);
            flush_stdin();
            if (confirm == 'n') {
                element = NULL;
            }
        } else {
            printf("Element with id=%u not found. Please retry or cancel.", id);
            puts("Retry (y/n)?");
            confirm = fgetc(stdin);
            flush_stdin();
            if (confirm == 'y') {
                confirm = 0;
            }
        }
    } while (confirm != 'y' && confirm != 'n');

    return element;
}

#endif