#ifndef HAND_INCLUDE
#define HAND_INCLUDE

typedef struct {
    char     face;
    char     suit; 
    char     rank;
} Card;

typedef struct {
    char    player_name[35];
    Card    cards[7];
    char    rank;
    int     score;    
} Hand;

#endif
