#include "ranking/ranking.h"

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
            buf[card_count] = serial_read();
            card_count++;
        }

        WT; // ranker 4000 is processing.

        // this is where read_card will go, and then following ranking
        // methods.
        for (byte i = 0; i < GROUP_SIZE; i++) {
            read_card(&buf[i]);
        }

        HAND h = rank_hand();
        serial_write((byte)h);
        serial_write('\r');
        serial_write('\n');

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
    for (byte *b = buf; *b != '\0'; b++) serial_write(*b);
}

void write_buf_newline(byte *buf) {
    write_buf(buf);
    serial_write('\r'); 
    serial_write('\n'); 
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
    // during insertion sort
    // keep best 5 cards.
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
        groups[pos].size++;
        return;
    } 
}

HAND rank_hand() {
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

    if (fours ==1) { return four_of_a_kind; }
    if (threes == 1 && twos == 0) { return three_of_a_kind; }
    if (threes == 1 && twos  == 1) { return full_house; }
    if (twos == 2) { return two_pair; }
    if (twos == 1 && singles == 3) { return pair; }
    if (singles == 5) {
        if ((seq() || ace_low_straight()) && same_suit()) { 
            return straight_flush; 
        }
        if ((seq() || ace_low_straight()) && !same_suit()) { 
            return straight;
        }
        if (!seq() && same_suit()) { return flush; }
    }

    return none;
}

// given that at this point we have ruled out any duplicates by checking
// for a pair, two pair, three of a kind, four of a kind and a full-house
// and we have sorted by rank highest to lowest the following should be true:
//
// if the first card minus the last card is four then the cards in between
// have no other choice but to be in order and sequential.

byte seq() { 
    if (groups[0].rank - groups[GROUP_SIZE - 1].rank == 4) return 1;
    return 0; 
}

// an exception to the normal flow
// in that an ace can either be a high card or a low card
// which only effects the straight essentially.
byte ace_low_straight() {
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

byte same_suit() { 
    // take the first rank, and compare it to all the others
    // they should all match, otherwise return false
    for (byte i = 1; i < GROUP_SIZE; i++) {
        if (suit(groups[0].cards[0]) != suit(groups[i].cards[0])) return 0;
    }
    return 1; 
}

byte serial_read() {
    // wait till RxComplete
    while ((UCSRA & _BV(RXC)) == 0) {;;} 
    return UDR;
}

void serial_write(byte DataOut) {
    // wait till TxReady
    while ((UCSRA & _BV(UDRE)) == 0) {;;} 
    UDR = DataOut;
}

void send_groups() {
    byte *comma = (byte *) ",\0";
    
    write_buf((byte *) "[\0");
    for (byte i = 0; i < GROUP_SIZE; i++) {
        write_buf((byte *) "{\0");
        serial_write(groups[i].rank);
        write_buf(comma);
        serial_write(groups[i].size);
        write_buf(comma);
        write_buf((byte *) "[\0");
        for (byte j = 0; j < GROUP_CARD_SIZE; j++) {
            write_buf((byte *) "{\0");
            serial_write(groups[i].cards[j]);
            if ((j + 1) < GROUP_CARD_SIZE) write_buf(comma);
        }
        write_buf((byte *) "]\0");
        write_buf((byte *) "}\0");
        if ((i + 1) < GROUP_SIZE) write_buf(comma);
        write_buf((byte *) "]\0");
    }
}
