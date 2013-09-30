#include "ranking.h"

#define GROUP_SIZE 7
#define LED _BV(0)
#define OK write_buf((byte *)"ok\0")
#define RDY write_buf((byte *)"rdy\0")
#define WT write_buf((byte *)"wt\0")

#define rank(c) (c & 0x0f)
#define suit(c) ((c & 0x70) >> 4)
#define wild(c) ((c & 0x80) >> 7)

Group groups[GROUP_SIZE];

int main(void) {
    // set up ports for status lights.
    DDRB = 0xff;
    DDRD = _BV(1);

    // turn on a status light 
    // will go off when serial com is setup
    PORTB = LED;

    // start serial 
    UBRRH = (byte) ((MYUBRR) >> 8);
    UBRRL = (byte) MYUBRR;
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = 3 << UCSZ0;

    // toggle light off - serial setup. Will say hello shortly
    PORTB ^= LED; 
    
    byte card_count = 0;
    byte buf[GROUP_SIZE];
    for (;;) {
        // reset groups - do this each time. Want a clean slate.
        reset_groups();

        // reset buffer to zero, don't want junk data.
        for (byte i = 0; i < GROUP_SIZE; i++) buf[i] = 0;

        RDY; // tell who ever is listening that the ranker 4000 is ready

        // fill buffer.
        while (card_count < 7) {
            buf[card_count] = serialRead();
            card_count++;
        }

        WT; // ranker 4000 is processing.

        // this is where read_card will go, and then following ranking
        // methods.
        for (byte i = 0; i < GROUP_SIZE; i++) serialWrite(buf[i]);
        serialWrite('\r');
        serialWrite('\n');

        OK; // ranker 4000 has finished, and output has been sent back

        card_count = 0;
    }
}

void reset_groups() {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        groups[i] = (Group ) { .rank = 0, 
                               .size = 0,
                               .cards = { 0, 0, 0, 0 } };
    } 
}

void write_buf(byte *buf) {
    for (byte *b = buf; *b != '\0'; b++) serialWrite(*b);
    serialWrite('\r'); 
    serialWrite('\n'); 
}

void read_card(byte *card) {
    byte pos = card_in_group(card);

    if (pos >= 0) {
        add_to_group(card, pos);
        return;
    }

    insert_into_groups(card);
}

byte card_in_group(byte *card) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank == 0) continue;
        if (rank(*card) == groups[i].rank)
            return i;
    }
    // negative number indicates not found
    return -1;
}

void insert_into_groups(byte *card) {
    // find first empty group. 
    // zero on the rank indicates empty. 
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank > 0) continue;

        groups[i] = (Group) { .rank = rank(*card),
                              .cards[0] = *card,
                              .size = 1 };
    }
}

void add_to_group(byte *card, byte pos) {
    for (byte i = 0; i < GROUP_CARD_SIZE; i++) {
        if (groups[pos].cards > 0) continue;
        groups[pos].cards[i] = *card;
        return;
    } 
}

byte serialRead() {
    // wait till RxComplete
	while ((UCSRA & _BV(RXC)) == 0) {;;} 
	return UDR;
}

void serialWrite(byte DataOut) {
    // wait till TxReady
	while ((UCSRA & _BV(UDRE)) == 0) {;;} 
	UDR = DataOut;
}
