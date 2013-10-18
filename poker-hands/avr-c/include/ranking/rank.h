#ifndef RANKING_RANK_H
#define RANKING_RANK_H

#include "ranking.h"

byte is_straight(Group *groups);
byte is_ace_low_straight(Group *groups);
byte is_same_suit(Group *groups);
byte rank_hand(Group *groups);

#endif
