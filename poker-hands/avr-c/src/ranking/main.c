#include "poker.h"

#define BaudRate 115200 
#define MYUBRR (F_CPU / 16 / BaudRate) - 1
#define GROUP_CARD_SIZE 4

#define GROUP_SIZE 5
#define LED _BV(0)
#define OK write_buf_newline((byte *)"ok\0")
#define RDY write_buf_newline((byte *)"rdy\0")
#define WT write_buf_newline((byte *)"wt\0")

#define rank(c) (c & 0x0f)
#define suit(c) ((c & 0x30) >> 4)
#define wild(c) ((c & 0x80) >> 7)

typedef struct {
    byte     size;
    byte     rank;
    byte     cards[GROUP_CARD_SIZE];
} Group; 

Group groups[GROUP_SIZE];

byte serial_read() {
    // wait till RxComplete
    while ((UCSRA & _BV(RXC)) == 0);
    return UDR;
}

void serial_write(byte DataOut) {
    // wait till TxReady
    while ((UCSRA & _BV(UDRE)) == 0);
    UDR = DataOut;
}

void write_buf(byte *buf) {
    for (byte *b = buf; *b != '\0'; b++) serial_write(*b);
}

void write_buf_newline(byte *buf) {
    write_buf(buf);
    serial_write('\n'); 
}

void insert_into_groups(byte *card) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank > 0) continue;

        groups[i].rank = rank(*card);
        groups[i].cards[0] = *card;
        groups[i].size = 1;
        return;
    }
}

void add_to_group(byte *card, byte pos) {
    for (byte i = 0; i < GROUP_CARD_SIZE; i++) {
        if (groups[pos].cards[i] > 0) continue;

        groups[pos].cards[i] = *card;
        groups[pos].size++;
        return;
    } 
}

char card_in_group(byte *card) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank == 0) continue;
        if (rank(*card) == groups[i].rank)
            return i;
    }
    // negative number indicates not found
    return -1;
}

// if a.size is less than b.size, then b needs to go to the left
// this is usually the otherway around, but I want higher cards to
// go to the left
//
// if the sizes match then if the a.rank is less than b.rank b again
// needs to got to the left.
byte keep_shifting(Group *a, Group *b) {
    if (a->size == b->size && a->rank < b->rank) return 1;
    if (a->size < b->size) return 1;

    return 0;
}

// insertion sort. Higher group sizes or cards will be first.
void sort_groups() {
    byte j;
    Group tmp;
    for (byte i = 1; i < GROUP_SIZE; i++) {
        tmp = groups[i];
        j = i;
        while (j > 0 && keep_shifting(&groups[j - 1], &tmp)) {
           groups[j] = groups[j - 1]; 
           j--;
        }
        groups[j] = tmp;
    }
}

// given that at this point we have ruled out any duplicates by checking
// for a pair, two pair, three of a kind, four of a kind and a full-house
// and we have sorted by rank highest to lowest the following should be true:
//
// if the first card minus the last card is four then the cards in between
// have no other choice but to be in order and sequential.

byte is_straight() { 
    if (groups[0].rank - groups[GROUP_SIZE - 1].rank == 4) return 1;
    return 0; 
}

// an exception to the normal flow
// in that an ace can either be a high card or a low card
// which only effects the straight essentially.
byte is_ace_low_straight() {
    if (groups[0].rank == 14 && 
        groups[GROUP_SIZE - 1].rank == 2 &&
        groups[1].rank == 5) {

        Group grp = groups[0]; 
        for (byte i = 1; i < GROUP_SIZE; i++) {
            groups[i - 1] = groups[i];
        }
        groups[GROUP_SIZE - 1] = grp;

        return 1;
    }

    return 0;
}

byte is_same_suit() { 
    // take the first rank, and compare it to all the others
    // they should all match, otherwise return false
    for (byte i = 1; i < GROUP_SIZE; i++) {
        if (suit(groups[0].cards[0]) != suit(groups[i].cards[0])) return 0;
    }
    return 1; 
}


void read_card(byte *card) {
    char pos = card_in_group(card);

    if (pos >= 0) {
        add_to_group(card, pos);
        return;
    }

    insert_into_groups(card);
}

byte rank_hand() {
    byte fours, threes, twos, singles;
    fours = threes = twos = singles = 0;

    for (byte i = 0; i < GROUP_SIZE; i++) {
        switch (groups[i].size) {
            case 4:
                fours++;
                break;
            case 3:
                threes++;
                break;
            case 2:
                twos++;
                break;
            case 1:
                singles++;
                break;
            default:
                continue;
        }
    }

    if (fours == 1) { return FOUR_OF_A_KIND; }
    if (threes == 1 && twos == 0) { return THREE_OF_A_KIND; }
    if (threes == 1 && twos  == 1) { return FULL_HOUSE; }
    if (twos == 2) { return TWO_PAIR; }
    if (twos == 1 && singles == 3) { return PAIR; }
    if (singles == 5) {
        if (is_same_suit() && (is_straight() || is_ace_low_straight())) { 
            return STRAIGHT_FLUSH; 
        }
        if (!is_same_suit() && (is_straight() || is_ace_low_straight())) { 
            return STRAIGHT;
        }
        if (!is_straight() && is_same_suit()) { return FLUSH; }

        return HIGH_CARD;
    }

    return NONE;
}

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

    // give anyone who is listening a moment before broadcasting rdy.
    // probably should just send a rdy message in a loop till someone
    // responds. But for now this will do.
    _delay_us(100);
    for (;;) {
        // reset groups - do this each time. Want a clean slate.
        for (byte i = 0; i < GROUP_SIZE; i++) {
            groups[i] = (Group ) { .rank = 0,
                                   .size = 0,
                                   .cards = {0, 0, 0, 0 } };
        } 

        // reset buffer to zero, don't want junk data.
        for (byte i = 0; i < GROUP_SIZE; i++) buf[i] = 0;

        RDY; // tell who ever is listening that the ranker 4000 is ready

        // fill buffer.
        while (card_count < GROUP_SIZE) {
            buf[card_count] = serial_read();
            card_count++;
        }

        WT; // ranker 4000 is processing.

        // this is where read_card will go, and then following ranking
        // methods.
        for (byte i = 0; i < GROUP_SIZE; i++) {
            read_card(&buf[i]);
        }

        sort_groups();
        byte hand = rank_hand();
    
        for (byte i = 0; i < GROUP_SIZE; i++) {
            for (byte j = 0; j < GROUP_CARD_SIZE; j++) {
                if (groups[i].cards[j] == 0) continue;
                serial_write(groups[i].cards[j]);
            }
        }

        serial_write(hand);
        serial_write('\n');

        OK; // ranker 4000 has finished, and output has been sent back

        card_count = 0;
    }
}
