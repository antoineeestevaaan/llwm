#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>

#include "ll.h"

ll_cell_t* cycle_init(void) { return NULL; }

void cycle_insert(ll_cell_t **cycle, int value) {
    ll_cell_t *new_cell = malloc(sizeof(ll_cell_t));
    new_cell->value = value;

    if (*cycle == NULL) {
        new_cell->next = new_cell;
    } else {
        new_cell->next = (*cycle)->next;
        (*cycle)->next = new_cell;
    }

    *cycle = new_cell;
}

void cycle_remove(ll_cell_t **cycle) {
    if (*cycle == NULL) return;

    // find the previous cell and put it in `cycle`
    ll_cell_t *current = *cycle;
    while (true) {
        if ((*cycle)->next == current) break;
        *cycle = (*cycle)->next;
    }

    if (*cycle == current) {
        free(*cycle);
        *cycle = NULL;
    } else {
        ll_cell_t *to_free = (*cycle)->next;
        (*cycle)->next = (*cycle)->next->next;
        free(to_free);
    }
}

void cycle_forward(ll_cell_t **cycle) {
    if (*cycle == NULL) return;
    *cycle = (*cycle)->next;
}

void cycle_print(ll_cell_t *cycle) {
    if (cycle == NULL) {
        printf("%p []\n", cycle);
        return;
    }

    ll_cell_t *current = cycle;

    printf("%p [", cycle);
    while (true) {
        printf("%d", cycle->value);
        cycle = cycle->next;
        if (cycle == current) break;
        printf(", ");
    }
    printf("]\n");
}
