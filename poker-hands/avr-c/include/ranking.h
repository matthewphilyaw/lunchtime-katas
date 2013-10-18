#ifndef RANKING_H
#define RANKING_H

#include "poker.h"

/*********************
 serial parameters
**********************/
#define BaudRate 115200
#define MYUBRR (F_CPU / 16 / BaudRate) - 1

/*********************
 pin aliases
**********************/
#define TXD (1 << PIND1)

/*********************
 constants
**********************/
#define GROUP_SIZE 5
#define GROUP_CARD_SIZE 5
#define EMPTY 0

#define RDY 0xff
#define ACK 0xfe
#define ERR 0xfb

/*********************
 helper macros
**********************/
#define rank(c) (c & 0x0f)
#define suit(c) ((c & 0x30) >> 4)
#define wild(c) ((c & 0x80) >> 7)

typedef struct {
    byte     size;
    byte     rank;
    byte     cards[GROUP_CARD_SIZE];
} Group;

#endif
