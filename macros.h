#pragma once

#ifdef DEBUG
#include <stdio.h>
#define        log(fmt, ...) printf(fmt, ##__VA_ARGS__)
#define        logln(fmt, ...) printf(fmt "\n", ##__VA_ARGS__)
#else // DEBUG
#define        log(fmt, ...) ((void)0)
#define        logln(fmt, ...) ((void)0)
#endif // DEBUG

#define bool_to_cstr(b) b ? "true" : "false"
