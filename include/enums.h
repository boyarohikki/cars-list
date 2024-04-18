#ifndef COURSE_TASK_ENUMS
#define COURSE_TASK_ENUMS

#define ENGINE_FILENAME "db/engines.csv"
#define CAR_FILENAME "db/cars.csv"

/* General enums */

typedef enum StatusCode {
    OK,
    MEMORY_ALLOCATION_ERROR,
    FILE_OPEN_ERROR,
    FILE_READ_ERROR,
    PARSE_ERROR,
    UNKNOWN
} StatusCode;

typedef enum MenuSelection {
    ABOUT,
    ADD,
    EDIT,
    DELETE,
    PRINT,
    SEARCH,
    SORT,
    EXIT
} MenuSelection;

typedef enum VariableType {
    CHAR,
    DOUBLE,
    FLOAT,
    INT,
    STRING,
    UNSIGNED
} VariableType;

#endif