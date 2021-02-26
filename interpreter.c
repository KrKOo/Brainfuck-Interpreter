#include "avcall.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/socket.h"
#include "unistd.h"
#define ALLOC_SIZE 10
#define SYSCALL_MAX_ARG_COUNT 7

#define FILE_OPEN_ERROR 1
#define ALLOCATION_ERROR 2

typedef struct {
    unsigned char memory[30000];
    unsigned char *pointer;
} bf_state;

typedef struct {
    char *array;
    size_t size;
    size_t allocated;
} char_vector;

typedef struct {
    int *array;
    size_t size;
    size_t allocated;
} int_vector;

typedef struct {
    union {
        unsigned char value;
        char *pointer;
    } data;
    bool isPointer;
} char_type;

void bf_state_ctor(bf_state *bf_state) {
    bf_state->pointer = memset(bf_state->memory, 0, sizeof(bf_state->memory));
}

void char_vector_ctor(char_vector *vector) {
    vector->array = NULL;
    vector->size = 0;
    vector->allocated = 0;
}

void char_vector_dtor(char_vector *vector) {
    free(vector->array);
    vector->size = 0;
    vector->allocated = 0;
}

void char_vector_push(char_vector *vector, char c) {
    if (vector->size + 1 >= vector->allocated) {
        if ((vector->array = (char *)realloc(
                 vector->array, (vector->allocated + ALLOC_SIZE))) == NULL) {
            exit(ALLOCATION_ERROR);
        }
        vector->allocated += ALLOC_SIZE;
    }
    vector->array[vector->size] = c;
    vector->size++;
}

void char_vector_pop(char_vector *vector) { vector->size--; }

void int_vector_ctor(int_vector *vector) {
    vector->array = NULL;
    vector->size = 0;
    vector->allocated = 0;
}

void int_vector_dtor(int_vector *vector) {
    free(vector->array);
    vector->size = 0;
    vector->allocated = 0;
}

void int_vector_push(int_vector *vector, int num) {
    if (vector->size + 1 >= vector->allocated) {
        if ((vector->array = (int *)realloc(
                 vector->array,
                 (vector->allocated + ALLOC_SIZE) * sizeof(int))) == NULL) {
            exit(ALLOCATION_ERROR);
        }
        vector->allocated += ALLOC_SIZE;
    }
    vector->array[vector->size] = num;
    vector->size++;
}

void int_vector_pop(int_vector *vector) { vector->size--; }

char_type *parse_syscall(bf_state *state, char *argc) {
    *argc = state->pointer[1] + 1;

    char_type *arg_array = calloc(*argc, sizeof(char_type));
    if (arg_array == NULL) exit(ALLOCATION_ERROR);
    
    arg_array[0].isPointer = false;
    arg_array[0].data.value = state->pointer[0];

    int pointerCounter = 2;

    for (int i = 1; i < *argc; i++) {
        bool is_pointer = state->pointer[pointerCounter++];
        char arg_len = state->pointer[pointerCounter++];
        (void) arg_len;

        if (is_pointer) {
            arg_array[i].isPointer = true;
            arg_array[i].data.pointer =
                (char*)&state->memory[(int)state->pointer[pointerCounter++]];
        } else {
            arg_array[i].isPointer = false;
            arg_array[i].data.value = state->pointer[pointerCounter++];
        }
    }
    return arg_array;
}

int call_syscall(char argc, char_type *arg_array) {
    av_alist argList;
    int retVal;
    av_start_int(argList, syscall, &retVal);
    for (int i = 0; i < argc; i++) {
        if (arg_array[i].isPointer) {
            av_ptr(argList, char *, arg_array[i].data.pointer);
        } else {
            av_char(argList, arg_array[i].data.value);
        }
    }
    av_call(argList);
    return retVal;
}

void bf_execute(char_vector *code, bf_state *state) {
    int_vector jumps;
    int_vector_ctor(&jumps);
    for (size_t i = 0; i < code->size; i++) {
        switch (code->array[i]) {
            case '>':
                state->pointer++;
                break;
            case '<':
                state->pointer--;
                break;
            case '+':
                *(state->pointer) += 1;
                break;
            case '-':
                *(state->pointer) -= 1;
                break;
            case '.':
                putchar(*state->pointer);
                break;
            case ',':
                *(state->pointer) = getchar();
                break;
            case '[':
                if (*state->pointer == 0) {
                    int counter = 1;
                    for (i++; counter != 0; i++) {
                        if (code->array[i] == '[') counter++;
                        if (code->array[i] == ']') counter--;
                    }
                    i--;
                } else {
                    int_vector_push(&jumps, i);
                }
                break;
            case ']':
                if ((*state->pointer) != 0) {
                    i = jumps.array[jumps.size - 1] - 1;
                }

                int_vector_pop(&jumps);
                break;
            case '%': {
                char argc;
                char_type *arg_array = parse_syscall(state, &argc);
                *state->pointer = call_syscall(argc, arg_array);
                free(arg_array);
                break;
            }
            case '?':
                putchar('c');
                break;
        }
    }
    int_vector_dtor(&jumps);
}

char_vector parseFileToArray(char *fileName) {
    FILE *file = fopen(fileName, "r");
    if (file == NULL) {
        printf("Could not open the file");
    }

    char_vector code;
    char_vector_ctor(&code);

    char c;
    while ((c = fgetc(file)) != EOF) {
        char_vector_push(&code, c);
    }
    return code;
}

int main(int argc, char **argv) {
    bf_state bf;
    bf_state_ctor(&bf);

    if (argc < 1) return 2;

    char_vector code;
    char_vector_ctor(&code);
    code = parseFileToArray(argv[1]);

    bf_execute(&code, &bf);

    char_vector_dtor(&code);
    return 0;
}
