#ifndef HAND_INCLUDE
#define HAND_INCLUDE

#include "common.h"

typedef struct {
    byte     face;
    byte     suit; 
    byte     rank;
} Card;

typedef struct {
    byte    player_name[35];
    Card    cards[7];
    byte    rank;
} Hand;

#endif
