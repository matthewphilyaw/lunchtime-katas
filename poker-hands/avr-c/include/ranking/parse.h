#ifndef RANKING_PARSE_H
#define RANKING_PARSE_H

#include "ranking.h"

void insert_into_groups(Group *groups, byte *card);
void add_to_group(Group *groups, byte *card, byte pos);
char card_in_groups(Group *groups, byte *card);
byte a_less_than_b(Group *a, Group *b);
void sort_groups(Group *groups);
void parse(Group *groups, byte *card);
byte validate_hand(Group *groups);

#endif
