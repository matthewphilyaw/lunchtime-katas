#ifndef POKER_INCLUDE
#define POKER_INCLUDE

#define F_CPU 8000000UL

#include <avr/io.h>
#include <util/delay.h>


typedef unsigned char byte;

typedef enum {
    none,
    high_card,
    pair,
    two_pair,
    three_of_a_kind,
    straight,
    flush,
    full_house,
    four_of_a_kind,
    straight_flush
} HAND;

#endif
