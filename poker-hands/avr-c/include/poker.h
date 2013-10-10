#ifndef POKER_INCLUDE
#define POKER_INCLUDE

#define F_CPU 14745600UL

#include <avr/io.h>
#include <util/delay.h>

typedef unsigned char byte;

#define NONE            (byte)0
#define HIGH_CARD       (byte)1
#define PAIR            (byte)2
#define TWO_PAIR        (byte)3
#define THREE_OF_A_KIND (byte)4
#define STRAIGHT        (byte)5
#define FLUSH           (byte)6 
#define FULL_HOUSE      (byte)7
#define FOUR_OF_A_KIND  (byte)8
#define STRAIGHT_FLUSH  (byte)9

#endif
