#pragma once

typedef struct ll_cell_t ll_cell_t;
struct ll_cell_t {
    int value;
    ll_cell_t *next;
};

ll_cell_t* cycle_init(void);
void       cycle_insert(ll_cell_t **cycle, int value);
void       cycle_remove(ll_cell_t **cycle);
void       cycle_forward(ll_cell_t **cycle);
void       cycle_print(ll_cell_t *cycle);
