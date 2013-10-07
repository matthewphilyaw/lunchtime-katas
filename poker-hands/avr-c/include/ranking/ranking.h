#ifndef RANKING_INCLUDE
#define RANKING_INCLUDE

#include "poker.h"

#define BaudRate 9600 
#define MYUBRR (F_CPU / 16 / BaudRate) - 1
#define GROUP_CARD_SIZE 4

#define GROUP_SIZE 5
#define LED _BV(0)
#define OK write_buf_newline((byte *)"ok\0")
#define RDY write_buf_newline((byte *)"rdy\0")
#define WT write_buf_newline((byte *)"wt\0")

#define rank(c) (c & 0x0f)
#define suit(c) ((c & 0x70) >> 4)
#define wild(c) ((c & 0x80) >> 7)

typedef struct {
    byte     size;
    byte     rank;
    byte     cards[GROUP_CARD_SIZE];
} Group; 

byte seq();
byte same_suit();
byte ace_low_straight();
HAND rank_hand();
void reset_groups();
void write_buf(byte *buf);
void write_buf_newline(byte *buf);
void add_to_group(byte *card, byte pos);
byte card_in_group(byte *card);
void insert_into_groups(byte *card);
void read_card(byte *card);
void send_groups();

// Serial
byte serial_read(); 
void serial_write(byte DataOut);

#endif
