#ifndef RANKING_INCLUDE
#define RANKING_INCLUDE

#include "poker.h"

#define BaudRate 9600 
#define MYUBRR (F_CPU / 16 / BaudRate) - 1
#define GROUP_CARD_SIZE 4

typedef struct {
    byte     size;
    byte     rank;
    byte     cards[GROUP_CARD_SIZE];
} Group; 

void reset_groups();
void write_newline();
void write_buf(byte *buf);
void add_to_group(byte *card, byte pos);
byte card_in_group(byte *card);
void insert_into_groups(byte *card);
void sort_groups();
void read_card(byte *card);

// Serial
byte serialRead(); 
void serialWrite(byte DataOut);

#endif
