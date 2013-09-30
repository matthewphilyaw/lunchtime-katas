#ifndef RANKING_INCLUDE
#define RANKING_INCLUDE

#include "poker.h"

#define BaudRate 9600 
#define MYUBRR (F_CPU / 16 / BaudRate) - 1
#define GROUP_CARD_SIZE 4

#define GROUP_SIZE 7
#define LED _BV(0)
#define OK write_buf((byte *)"ok\0")
#define RDY write_buf((byte *)"rdy\0")
#define WT write_buf((byte *)"wt\0")

#define rank(c) (c & 0x0f)
#define suit(c) ((c & 0x70) >> 4)
#define wild(c) ((c & 0x80) >> 7)

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
void read_card(byte *card);

// Serial
byte serialRead(); 
void serialWrite(byte DataOut);

#endif
