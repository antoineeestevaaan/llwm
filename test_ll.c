#include <limits.h>
#include <stdlib.h>
#include <stdio.h>

#include "ll.h"

#define ARRAY_LEN(array) (sizeof(array) / sizeof(array[0]))

typedef enum {
    // all values before will be used to insert a value
    INIT    = INT_MAX - 4,
    PRINT   = INT_MAX - 3,
    REMOVE  = INT_MAX - 2,
    FORWARD = INT_MAX - 1,
} ll_cell_opt_t;

int main(void) {
    ll_cell_t *cycle;

    ll_cell_opt_t ops[] = {
        INIT            , PRINT,
        0               , PRINT,
        1               , PRINT,
        2               , PRINT,
        3               , PRINT,
        FORWARD         , PRINT,
        FORWARD         , PRINT,
        REMOVE          , PRINT,
        REMOVE, FORWARD , PRINT,
        REMOVE          , PRINT,
        REMOVE          , PRINT,
        REMOVE          , PRINT,
    };

    for (size_t i = 0; i < ARRAY_LEN(ops); i++) {
        switch (ops[i]) {
            case INIT:
                printf("init: ");
                cycle = cycle_init();
                break;
            case FORWARD:
                printf("forward: ");
                cycle_forward(&cycle);
                break;
            case REMOVE:
                printf("remove: ");
                cycle_remove(&cycle);
                break;
            case PRINT:
                cycle_print(cycle);
                break;
            default:
                printf("insert %d: ", ops[i]);
                cycle_insert(&cycle, ops[i]);
                break;
        }
    }

    return 0;
}
