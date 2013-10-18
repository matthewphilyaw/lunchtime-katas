#include "ranking/rank.h"

/**************************************************************************//**
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
byte is_straight(Group *groups) {
    if (groups[0].rank - groups[GROUP_SIZE - 1].rank == 4) return 1;
    return 0;
}

/**************************************************************************//**
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
byte is_ace_low_straight(Group *groups) {
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

/**************************************************************************//**
 Take the first rank, and compare it to all the others they should all match,
 otherwise return false
******************************************************************************/
byte is_same_suit(Group *groups) {
    for (byte i = 1; i < GROUP_SIZE; i++) {
        if (suit(groups[0].cards[0]) != suit(groups[i].cards[0])) return 0;
    }
    return 1;
}

byte rank_hand(Group *groups) {
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

    if (fours == 1)                return FOUR_OF_A_KIND;
    if (threes == 1 && twos == 0)  return THREE_OF_A_KIND;
    if (threes == 1 && twos  == 1) return FULL_HOUSE;
    if (twos == 2)                 return TWO_PAIR;
    if (twos == 1 && singles == 3) return PAIR;
    if (singles == 5) {
        if (is_same_suit(groups) && 
            (is_straight(groups) || is_ace_low_straight(groups))) {
            return STRAIGHT_FLUSH;
        }
        if (!is_same_suit(groups) && 
            (is_straight(groups) || is_ace_low_straight(groups))) {
            return STRAIGHT;
        }
        if (!is_straight(groups) && is_same_suit(groups)) return FLUSH;

        return HIGH_CARD;
    }

    return INVALID_HAND;
}
