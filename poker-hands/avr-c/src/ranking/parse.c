#include "ranking/parse.h"

void insert_into_groups(Group *groups, byte *card) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank != EMPTY) continue;

        groups[i].rank = rank(*card);
        groups[i].cards[0] = *card;
        groups[i].size = 1;
        return;
    }
}

void add_to_group(Group *groups, byte *card, byte pos) {
    for (byte i = 0; i < GROUP_CARD_SIZE; i++) {
        if (groups[pos].cards[i] != EMPTY) continue;

        groups[pos].cards[i] = *card;
        groups[pos].size++;
        return;
    }
}

char card_in_groups(Group *groups, byte *card) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank == 0) continue;
        if (rank(*card) == groups[i].rank)
            return i;
    }
    // negative number indicates not found
    return -1;
}

/**************************************************************************//**
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


/**************************************************************************//**
 Simple insertion sort.

 Only caveat is that large items are moved to the front.
******************************************************************************/
void sort_groups(Group *groups) {
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

void parse(Group *groups, byte *card) {
    char pos = card_in_groups(groups, card);

    if (pos >= 0) {
        add_to_group(groups, card, pos);
        return;
    }

    insert_into_groups(groups, card);
}

/**************************************************************************//**
 Simple validation of a hand.

 It ensures that the cards present are legal cards, however it doesn't
 Guarantee that the cards are legal over a series of hands. 

 For example you could have a legal hand according to this function,
 but in the same round of poker if another hand comes in with the same
 cards it wouldn't not know this, so some validation needs to happen
 from the client side. 

 Checks:
    - Ensure cards are between 0x0e and 0x02 inclusive. 0x0f and 0x02 are 
      invalid and 0x00 is reserved as empty.

    - Ensure that no group size is equal to 5 which would be 5 of the same face
      value.

    - Ensure that for a given group (same face value) that no two cards hane
      the same suit. Suit should be unique per group.

******************************************************************************/
byte valid_hand(Group *groups) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        Group *grp = &groups[i];

        if (grp->rank == EMPTY) continue;
        if (grp->rank ==  0x0f || grp->rank == 0x01) return INVALID_HAND;
        if (grp->size == 5) return INVALID_HAND;
        if (grp->size > 1) {
            for (byte j = 1; j < GROUP_CARD_SIZE; j++) {
                if (grp->cards[j] == EMPTY) continue;
                if (suit(grp->cards[j - 1]) == suit(grp->cards[j]))
                    return 0;
                
            } 
        }
    }    
    return 1;
}
