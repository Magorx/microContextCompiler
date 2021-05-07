#ifndef GENERAL_TIMER
#define GENERAL_TIMER

#include <stdio.h>
#include <time.h>

//=============================================================================
//<KCTF> Global_timer =========================================================
time_t GLOBAL_TIMER;
double GLOBAL_TIMER_INTERVAL;

#define TIMER_START() do {GLOBAL_TIMER = clock();} while (0)
#define TIMER_BREAK() do {GLOBAL_TIMER_INTERVAL = (double)(clock() - GLOBAL_TIMER) / CLOCKS_PER_SEC;} while(0)
#define TIMER_END() do {TIMER_BREAK(); TIMER_START();} while (0)

#define TIMER_PRINT() do {printf("[...]<Timer>: %lf\n", GLOBAL_TIMER_INTERVAL);} while (0)
#define TIMER_END_AND_PRINT() do {TIMER_END(); TIMER_PRINT();} while (0)
#define TIMER_BREAK_AND_PRINT() do {TIMER_BREAK(); TIMER_PRINT();} while(0)

#endif // GENERAL_TIMER