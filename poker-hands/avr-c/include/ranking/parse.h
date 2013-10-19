#ifndef RANKING_PARSE_H
#define RANKING_PARSE_H

#include "ranking.h"

void sort_groups(Group *groups);
void parse(Group *groups, byte *card);
byte valid_hand(Group *groups);

#endif
