/* Parses subset of RFC4180 csv files */
/*
What is supported:
RFC4180 csv scheme with some changes:
1. Only rectangular csvs supported. All lines must have the same fields count.
2. CRLF at the end of the file is not allowed. It creates ambiguous situations
   when csv contains one column that could be empty. Example:
Some text

   It can be parsed as two lines with "Some text" and "" or as one line with
   "Some text". Ambiguous is not a good thing, so this restriction appeared.
3. File must be opened in binary mode, so no change to line endings. The
   positive side of this change is whole utf-8 table supported.
*/
#ifndef CSV_FILE_H
#define CSV_FILE_H
#include <stdio.h>
#include <stdlib.h>

/* Error codes */
#define CSV_SUCCESS 0
#define CSV_EPARSE 1
#define CSV_ENOMEM 2

/* Main symbols */
#define CSV_COMMA 0x2C
#define CSV_CR 0x0d
#define CSV_DQUOTE 0x22
#define CSV_LF 0x0A

typedef struct CSVFileLine {
    char **fields;
} CSVFileLine;

typedef struct CSVFile {
    size_t fields_count;
    size_t lines_count;
    CSVFileLine *lines;
} CSVFile;

/* Frees file lines based on lines_count value */
void CSVFileFree(CSVFile *file);
/* Reads csv data from file fp to output csv struct */
int CSVFileRead(FILE *fp, CSVFile *output);
/* Output file must be opened in binary mode */
void CSVFileWrite(FILE *fp, CSVFile data);
/* Free individual file line */
void _CSVFileFreeLine(CSVFileLine *line, size_t fields_count);
/* Read one field from file */
int _CSVFileReadField(FILE *csv_file, char **output);
/* Skip line separator in file */
int _CSVFileSkipCRLF(FILE *csv_file);
/* Read one line from file */
int _CSVReadLine(FILE *csv_fiel, CSVFileLine *output, size_t fields_count);
/* Count fields in current line of file */
int _CSVFileCountFields(FILE *csv_file, size_t *fields_count);

/* Free memeory allocated for CSVFile file and it's rows*/
void CSVFileFree(CSVFile *file) {
    size_t i;

    if (file->lines_count) {
        for (i = 0; i < file->lines_count; i++) {
            _CSVFileFreeLine(&file->lines[i], file->fields_count);
        }
        free(file->lines);
    }
    file->lines = NULL;
    file->lines_count = 0;
}

/* Read file fp contents to CSVFile output. fp MUST be opened in binary mode and
has CRLF line endings */
int CSVFileRead(FILE *fp, CSVFile *output) {
    int error = CSV_SUCCESS, continue_loop = 1;
    size_t allocated = 0;
    CSVFileLine *realloc_result;
    char next_char;

    output->lines_count = 0;
    output->fields_count = 0;
    output->lines = NULL;

    error = _CSVFileCountFields(fp, &output->fields_count);
    continue_loop = error == CSV_SUCCESS;
    if (error != CSV_SUCCESS) {
        puts("Error on counting fields");
    }
    while (continue_loop) {
        if (output->lines_count + 1 > allocated) {
            allocated = allocated ? allocated * 2 : 1;
            realloc_result = (CSVFileLine *)realloc(
                output->lines, allocated * sizeof(CSVFileLine));
            if (realloc_result) {
                output->lines = realloc_result;
            } else {
                error = CSV_ENOMEM;
            }
        }
        if (error == CSV_SUCCESS) {
            error = _CSVReadLine(fp, &output->lines[output->lines_count],
                                 output->fields_count);
        }
        if (error == CSV_SUCCESS) {
            output->lines_count++;
            /* Skip line separator */
            if (_CSVFileSkipCRLF(fp) == CSV_EPARSE) {
                next_char = fgetc(fp);
                if (next_char == EOF) {
                    error = CSV_SUCCESS;
                    continue_loop = 0;
                } else {
                    error = CSV_EPARSE;
                }
            }
        }
        continue_loop = continue_loop && error == CSV_SUCCESS;
    }

    if (error != CSV_SUCCESS) {
        for (; output->lines_count > 0; output->lines_count--) {
            _CSVFileFreeLine(&output->lines[output->lines_count - 1],
                             output->fields_count);
        }
        free(output->lines);
        output->lines = NULL;
    }

    return error;
}

/* Saves CSVFile data to fp. fp MUST be opened in binary mode */
void CSVFileWrite(FILE *fp, CSVFile data) {
    size_t i, j;
    char CRLF[] = {CSV_CR, CSV_LF}, *ch;

    for (i = 0; i < data.lines_count; i++) {
        for (j = 0; j < data.fields_count; j++) {
            fputc(CSV_DQUOTE, fp);
            ch = data.lines[i].fields[j];
            while (*ch) {
                fputc(*ch, fp);
                if (*ch == CSV_DQUOTE) {
                    fputc(*ch, fp);
                }
                ch++;
            }
            fputc(CSV_DQUOTE, fp);
            if (j != data.fields_count - 1) {
                fputc(CSV_COMMA, fp);
            }
        }
        if (i != data.lines_count - 1) {
            fwrite(CRLF, sizeof(*CRLF), sizeof(CRLF) / sizeof(*CRLF), fp);
        }
    }
}

/* Free single line of CSVFile */
void _CSVFileFreeLine(CSVFileLine *line, size_t fields_count) {
    size_t i;
    for (i = 0; i < fields_count; i++) {
        free(line->fields[i]);
    }
    free(line->fields);
    line->fields = NULL;
}

/* Adds character to the end of string (overwrites it's 0 terminator). Used to
control allocated size of string and realloc if needed. */
int _CSVFileAddCharToOutput(char character, char **output, size_t *string_len,
                            size_t *allocated) {
    int error = CSV_SUCCESS;
    char *realloc_result;

    (*string_len)++;
    if (*string_len + 1 > *allocated) {
        *allocated *= 2;
        realloc_result = (char *)realloc(*output, sizeof(char) * *allocated);
        if (realloc_result) {
            *output = realloc_result;
            (*output)[*string_len - 1] = character;
        } else {
            free(*output);
            *output = NULL;
            error = CSV_ENOMEM;
        }
    } else {
        (*output)[*string_len - 1] = character;
    }
    return error;
}

/* Read one csv field */
int _CSVFileReadField(FILE *csv_file, char **output) {
    size_t string_len = 0, allocated = 1, i;
    char quoted = 0, continue_loop = 1;
    char *realloc_result = NULL;
    char current_char;
    int error = CSV_SUCCESS;

    *output = (char *)malloc(sizeof(char) * allocated);

    /* Test if it's quoted field and read first*/
    if ((current_char = fgetc(csv_file)) != EOF) {
        if (current_char == CSV_DQUOTE) {
            quoted = 1;
        } else {
            fseek(csv_file, -1, SEEK_CUR);
        }
    }

    continue_loop = current_char != EOF;

    while (continue_loop) {
        current_char = fgetc(csv_file);
        if (quoted) {
            switch (current_char) {
                case CSV_DQUOTE:
                    i = 1;
                    while ((current_char = fgetc(csv_file)) == CSV_DQUOTE &&
                           error == CSV_SUCCESS) {
                        if (++i % 2 == 0) {
                            error = _CSVFileAddCharToOutput(
                                current_char, output, &string_len, &allocated);
                        }
                    }

                    if (error == CSV_SUCCESS) {
                        if (current_char == EOF) {
                            continue_loop = 0;
                        } else {
                            if (i % 2) {
                                if (current_char == CSV_COMMA ||
                                    current_char == CSV_CR) {
                                    continue_loop = 0;
                                } else {
                                    error = CSV_EPARSE;
                                }
                            } else {
                                fseek(csv_file, -1, SEEK_CUR);
                            }
                        }
                    }
                    break;
                case EOF:
                    error = CSV_EPARSE;
                    break;
                default:
                    error = _CSVFileAddCharToOutput(current_char, output,
                                                    &string_len, &allocated);
            }
        } else {
            switch (current_char) {
                case CSV_COMMA:
                case CSV_CR:
                case EOF:
                    continue_loop = 0;
                    break;
                case CSV_DQUOTE:
                    error = CSV_EPARSE;
                    break;
                default:
                    error = _CSVFileAddCharToOutput(current_char, output,
                                                    &string_len, &allocated);
                    break;
            }
        }
        continue_loop = continue_loop && error == CSV_SUCCESS;
    }

    switch (error) {
        case CSV_EPARSE:
            free(*output);
            *output = NULL;
            break;
        case CSV_SUCCESS:
            (*output)[string_len] = 0;
            realloc_result =
                (char *)realloc(*output, (string_len + 1) * sizeof(char));
            if (realloc_result) {
                *output = realloc_result;
            }
            if (current_char != EOF) {
                /* Go to separator symbol */
                fseek(csv_file, -1, SEEK_CUR);
            }
            break;
    }

    return error;
}

/* Count fields in file by first line info */
int _CSVFileCountFields(FILE *csv_file, size_t *fields_count) {
    long start = ftell(csv_file);
    char quoted = 0, current_char, previous_char = 0, continue_loop = 1;
    int error = CSV_SUCCESS;
    size_t quotes_count;

    *fields_count = 0;
    while (continue_loop) {
        current_char = fgetc(csv_file);
        switch (current_char) {
            case EOF:
                if (quoted) {
                    error = CSV_EPARSE;
                } else {
                    (*fields_count)++;
                    continue_loop = 0;
                }
                break;
            case CSV_COMMA:
                if (!quoted) {
                    (*fields_count)++;
                }
                break;
            case CSV_CR:
                if (!quoted) {
                    (*fields_count)++;
                    continue_loop = 0;
                }
                break;
            case CSV_DQUOTE:
                quotes_count = 1;
                while ((current_char = fgetc(csv_file)) == CSV_DQUOTE) {
                    quotes_count++;
                }
                if (current_char != EOF) {
                    fseek(csv_file, -1, SEEK_CUR);
                }
                if (quotes_count % 2) {
                    if (quoted) {
                        quoted = 0;
                    } else {
                        if (previous_char == CSV_COMMA || previous_char == 0) {
                            quoted = 1;
                        } else {
                            error = CSV_EPARSE;
                        }
                    }
                }
                break;
        }
        previous_char = current_char;
        continue_loop = continue_loop && error == CSV_SUCCESS;
    }

    if (error != CSV_SUCCESS) {
        *fields_count = 0;
    }

    fseek(csv_file, start, SEEK_SET);

    return error;
}

/* Skip line ending with strict characters check */
int _CSVFileSkipCRLF(FILE *csv_file) {
    int error = CSV_SUCCESS;
    char next;
    if ((next = fgetc(csv_file)) == CSV_CR) {
        if ((next = fgetc(csv_file)) == CSV_LF) {
        } else {
            error = CSV_EPARSE;
            if (next == EOF) {
                fseek(csv_file, -1, SEEK_CUR);
            } else {
                fseek(csv_file, -2, SEEK_CUR);
            }
        }
    } else {
        error = CSV_EPARSE;
        if (next != EOF) {
            fseek(csv_file, -1, SEEK_CUR);
        }
    }

    return error;
}

/* Read one csv line */
int _CSVReadLine(FILE *csv_file, CSVFileLine *output, size_t fields_count) {
    int error = CSV_SUCCESS;
    size_t i;
    char continue_loop = 1, next_char;

    output->fields = (char **)malloc(sizeof(char *) * fields_count);

    if (!output->fields) {
        error = CSV_ENOMEM;
        continue_loop = 0;
    }

    i = 0;
    while (continue_loop) {
        if (i < fields_count) {
            error = _CSVFileReadField(csv_file, &output->fields[i]);
            if (error == CSV_SUCCESS) {
                next_char = fgetc(csv_file); /* skip separator */
                if (next_char == EOF) {
                    continue_loop = 0;
                } else {
                    if (next_char == CSV_CR) {
                        continue_loop = 0;
                        fseek(csv_file, -1, SEEK_CUR);
                    }
                }
                i += 1;
            }
        } else {
            error = CSV_EPARSE;
            puts("Out of fields bounds!");
        }
        continue_loop = continue_loop && error == CSV_SUCCESS;
    }

    /* If read less than needed, than it's wrong */
    if (error == CSV_SUCCESS) {
        if (i != fields_count) {
            error = CSV_EPARSE;
        }
    }
    if (error != CSV_SUCCESS) {
        for (; i > 0; i--) {
            free(output->fields[i - 1]);
        }
        free(output->fields);
        output->fields = NULL;
    }

    return error;
}

#endif