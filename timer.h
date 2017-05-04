#ifndef TIMER_H
#define TIMER_H

#define Wait_time(t)                                          \
        do {                                                  \
                volatile unsigned long i;                     \
                                                              \
                for (i = 0; i < 1000000UL * t; i++)           \
                        ;                                     \
        } while (0);   
#endif
