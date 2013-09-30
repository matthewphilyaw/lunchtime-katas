#include "ranking.h"

#define GROUP_SIZE 7
#define LED _BV(0)
#define OK write_buf(ok)
#define RDY write_buf(rdy)
#define WT write_buf(wt)

byte ok[3] = "ok\0";
byte rdy[4] = "rdy\0";
byte wt[3] = "wt\0";

Group groups[GROUP_SIZE];

int main(void) {
    // set up ports for status lights.
    DDRB = 0xff;
    DDRD = _BV(1);

    // turn on a status light 
    // will go off when serial com is setup
    PORTB = LED;

    // start serial 
    UBRRH = (byte) (MYUBRR >> 8);
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
        Group g = { .rank = -1, 
                    .size = -1 };
        for (byte ig = 0; ig < GROUP_CARD_SIZE; ig++) {
            g.cards[ig] = (Card) { .rank = -1,
                                   .suit = -1,
                                   .wild = -1 };
        }
        groups[i] = g;
    } 
}

void write_buf(byte *buf) {
    for (byte *b = buf; *b != '\0'; b++) serialWrite(*b);
    serialWrite('\r'); 
    serialWrite('\n'); 
}

void read_card(byte raw_card) {
    Card card = { .rank = raw_card & 0x0f,
                  .suit = (raw_card & 0x70) >> 4,
                  .wild = (raw_card & 0x80) >> 7 };

    byte pos = card_in_group(&card);

    if (pos >= 0) {
        add_to_group(card, pos);
        return;
    }

    insert_into_groups(card);
}

byte card_in_group(Card *current) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank < 0) continue;
        if (current->rank == groups[i].rank)
            return i;
    }
    // negative number indicates not found
    return -1;
}

void insert_into_groups(Card current) {
    // we don't have a group, we need to make one.
    Group grp = { .rank = current.rank,
                  .cards[0] = current,
                  .size = 1 };
    
    // find first available slot. 
    // the rank field was chosen here to indicate empty.
    // A negative one or any value less than zero is considered empty.
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank < 0) continue;
        groups[i] = grp; 
    }
}

void add_to_group(Card current, byte pos) {
    for (byte i = 0; i < GROUP_CARD_SIZE; i++) {
        if (groups[pos].cards[i].rank < 0) continue;
        groups[pos].cards[i] = current;
        return;
    } 
}

byte serialRead() {
    // wait till RxComplete
    while (UCSRA & _BV(RXC) == 0) {;;} 
    return UDR;
}

void serialWrite(byte DataOut) {
    // wait till TxReady
    while (UCSRA & _BV(UDRE) == 0) {;;} 
    UDR = DataOut;
}
