#include "ranking.h"
#include "ranking/io.h"
#include "ranking/parse.h"
#include "ranking/rank.h"

Group grps[GROUP_SIZE];

void send_cards(Group *groups) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        for (byte j = 0; j < GROUP_CARD_SIZE; j++) {
            if (groups[i].cards[j] == EMPTY) continue;
            serial_write(groups[i].cards[j]);
        }
    }
}

int main(void) {

    // start serial
    serial_init();

    byte card_count;
    byte buf[GROUP_SIZE];
    byte hand;
    byte i = 0;
    for (;;) {
        // reset groups - do this each time. Want a clean slate.
        for (i = 0; i < GROUP_SIZE; i++) {
            grps[i] = (Group) { .rank = EMPTY,
                                .size = EMPTY,
                                .cards = { EMPTY, EMPTY, EMPTY, EMPTY }};
        }

        // reset buffer to zero, don't want junk data.
        for (i = 0; i < GROUP_SIZE; i++) buf[i] = EMPTY;

        while (serial_read() != RDY);
        serial_write(ACK);

        // fill buffer.
        card_count = 0;
        while (card_count < GROUP_SIZE) {
            buf[card_count] = serial_read();
            card_count++;
            serial_write(ACK);
        }

        serial_write(ACK);

        // this is where read_card will go, and then following ranking
        // methods.
        for (i = 0; i < GROUP_SIZE; i++) {
            parse(grps, &buf[i]);
        }

        sort_groups(grps);
        
        // if we have an invalid hand, write cards back
        // and send back the INVALID_HAND type.
        // then send an ACK, and then wait for a ready state.
        if (validate_hand(grps) == INVALID_HAND) {
            send_cards(grps);
            serial_write(INVALID_HAND);
            serial_write(ACK);
            continue;
        }

        hand = rank_hand(grps);
        send_cards(grps);

        serial_write(hand);
        serial_write(ACK);
    }
}
