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
#define GROUP_CARD_SIZE 4
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

#ifdef DEBUG_MODE
void write_buf(byte *buf) {
    for (byte *b = buf; *b != '\0'; b++) serial_write(*b);
}

void write_buf_newline(byte *buf) {
    write_buf(buf);
    serial_write('\n');
}
#endif

void insert_into_groups(byte *card) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank != EMPTY) continue;

        groups[i].rank = rank(*card);
        groups[i].cards[0] = *card;
        groups[i].size = 1;
        return;
    }
}

void add_to_group(byte *card, byte pos) {
    for (byte i = 0; i < GROUP_CARD_SIZE; i++) {
        if (groups[pos].cards[i] != EMPTY) continue;

        groups[pos].cards[i] = *card;
        groups[pos].size++;
        return;
    }
}

char card_in_groups(byte *card) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank == 0) continue;
        if (rank(*card) == groups[i].rank)
            return i;
    }
    // negative number indicates not found
    return -1;
}

/******************************************************************************
 Used with the insertion sort to push large items to the front of the list.

 A is an item in the list, B is the item we are inserting.

 First check group size, as large group size needs to go to the front,
 if the group size matches then use rank instead, again large ranks need to
 shift to the front.
******************************************************************************/
byte a_less_than_b(Group *a, Group *b) {
    if (a->size == b->size && a->rank < b->rank) return 1;
    if (a->size < b->size) return 1;

    return 0;
}


/******************************************************************************
 Simple insertion sort.

 Only caveat is that large items are moved to the front.
******************************************************************************/
void sort_groups() {
    byte j;
    Group tmp;
    for (byte i = 1; i < GROUP_SIZE; i++) {
        tmp = groups[i];
        j = i;
        while (j > 0 && a_less_than_b(&groups[j - 1], &tmp)) {
           groups[j] = groups[j - 1];
           j--;
        }
        groups[j] = tmp;
    }
}

/******************************************************************************
 Given that there are no duplicates and the cards are in order from high to low
 the following should be true:

 If the first card minus the last card is four then the cards in between
 have no other choice but to be in order and sequential.

 Example:
    sequential
    10 9 8 7 6 | 10 - 6 = 4

    just in order
    10 8 6 5 2 | 10 - 2 = 8

 Having the constraint of no duplicates eliminates the possibility for the
 three numbers between the first and last (assuming 5 cards) to be anything
 other than the natural sequence between first and last card given they are in
 order
******************************************************************************/
byte is_straight() {
    if (groups[0].rank - groups[GROUP_SIZE - 1].rank == 4) return 1;
    return 0;
}

/******************************************************************************
 This is an exception to the normal flow
 in that an ace can either be a high card or a low card
 which only effects the straight essentially.

 It is very similar to the is_straight function, but we have a more known
 conditions coming into this method.

 Given that a straight wasn't found, then we can look to see if the last card
 is a two and the first card is an ace to determine whether we have potential
 for an ace low straight. If both of those are found, then if the second card
 is a 5, then we have an ace low straight.

 This assume no duplicates are present and the cards are in order from high
 to low.
******************************************************************************/
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

/******************************************************************************
 Take the first rank, and compare it to all the others they should all match,
 otherwise return false
******************************************************************************/
byte is_same_suit() {
    for (byte i = 1; i < GROUP_SIZE; i++) {
        if (suit(groups[0].cards[0]) != suit(groups[i].cards[0])) return 0;
    }
    return 1;
}


void read_card(byte *card) {
    char pos = card_in_groups(card);

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

    // start serial
    DDRD ^= TXD; // make sure the TXD pin is set to out
    UBRRH = (byte) ((MYUBRR) >> 8);
    UBRRL = (byte) MYUBRR;
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = 3 << UCSZ0;

    byte card_count;
    byte buf[GROUP_SIZE];

    for (;;) {
        // reset groups - do this each time. Want a clean slate.
        for (byte i = 0; i < GROUP_SIZE; i++) {
            groups[i] = (Group ) { .rank = EMPTY,
                                   .size = EMPTY,
                                   .cards = { EMPTY, EMPTY, EMPTY, EMPTY }};
        }

        // reset buffer to zero, don't want junk data.
        for (byte i = 0; i < GROUP_SIZE; i++) buf[i] = EMPTY;

        while (serial_read() != RDY);

        // fill buffer.
        card_count = 0;
        while (card_count < GROUP_SIZE) {
            buf[card_count] = serial_read();
            card_count++;
        }

        serial_write(ACK);

        // this is where read_card will go, and then following ranking
        // methods.
        for (byte i = 0; i < GROUP_SIZE; i++) {
            read_card(&buf[i]);
        }

        sort_groups();
        byte hand = rank_hand();

        for (byte i = 0; i < GROUP_SIZE; i++) {
            for (byte j = 0; j < GROUP_CARD_SIZE; j++) {
                if (groups[i].cards[j] == EMPTY) continue;
                serial_write(groups[i].cards[j]);
            }
        }

        serial_write(hand);
        serial_write(ACK);
    }
}