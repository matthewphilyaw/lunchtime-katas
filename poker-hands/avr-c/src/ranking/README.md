## Overview

The ranking module (aka ranker 4000) takes in a single hand, performs some simple validation on the hand to make sure cards are legal within some basic rules, and then sorts and ranks the hand sending it back to the client. 

This readme is broken up  into several sections starting with the protocol, and the usage/api and more detailed info on the algorithm used for ranking a hand. 

## Protocol

#### Wire Format:
Cards are encoded into a single byte with the following layout:
```
Wild N/A    Suit Rank
0    0      00   0000
```

MSB is the wild card flag. the next bit is unused, the next two are the suit 
(0-3) and the last four are the rank (2-14). 

The hand type is also encoded in one byte and is simply 0-9. 

```
INVALID_HAND        0
HIGH_CARD           1
PAIR                2
TWO_PAIR            3
THREE_OF_A_KIND     4
STRAIGHT            6
FLUSH               6 
FULL_HOUSE          7
FOUR_OF_A_KIND      8
STRAIGHT_FLUSH      9
```

#### Semantics:
When the ranking module spins up it will initialize a few things and then block listening for a ready signal from the client. The ready signal is one byte and the value is 0xff. Once the signal is received the ranking module will then send back one byte which is the acknowledgment and it's value is 0xfe. 

The code is fairly well commented and should be rather easy to follow. There are more details in the comments about how certain hands are found etc.


After the ready phase the ranking module sets up to read 5 cards off the bus and will send an ACK after each card. Once the cards are read in the it will then send the cards back with the rank tacked on the end. Validation happens before the ranking occurs and if the hand is invalid it will still send the cards back and have the INVALID_HAND type tacked on. 

The ranking module sends back the cards, because it will sort them in a particular way which can be used for a tie breaker when comparing two hands of the same type. More on that in a later section. 

## Validation

This module does some basic validation to ensure that the cards sent to the hardware module are legal within the context of the hand given.

The validation rules are: 
 + No 5 of a kind - as that would imply 5 cards of the same face value
 + Ensure that the cards passed in are valid cards.
 + Ensure that the suit is unique among any group of cards with the same face value. No two cards with the same face value should have the same suit

If any of those checks fail an INVALID_HAND type is sent back to the client. 

These checks are only in the context of the given hand, so it doesn't check to see if a particular card has been used by a previous hand thus the given hand should not contain it (for example this would apply to five card draw and seven card stud)

## Ranking
Ranking a poker hand turns out to be an interesting challenge. This section will attempt to cover the approach taken by the ranking module and why the approach was taken. 

#### Grouping:
The first thing the ranking module does with the cards once they are read in is group them by rank. The idea behind this will become clearer as this section progresses but it makes it easier to find hands that are made up of groups of the same face value cards.

The group structure consists of a rank field, a size field and an array of cards which are the raw cards in their byte form. As a card is processed the array of groups is checked to see if there is a group with the same rank. If there is one then that card is added to that group and the size is incremented by one. Otherwise a new group is setup and in the next available slot in the array of groups. 

A group can be a single a card, in which case it will have a size of one. 

Five of the nine hands can be found quickly by checking just the size of group alone in different combos. For example if you have one group of three and one group of two then you have a full house. The code is pretty straight forward.

#### Sorting: 
After the cards have been grouped a simple insertion sort is done to order the array of groups in order of largest size first, and if the size matches then highest rank first. This is done to aid in checking for certain hands (like straight and ace low straight) as well as a serving as a tie breaker for two tied hand types. 

If two hands are of the same type, say two pair, then a mechanism is needed to break the tie (in some cases there can be a true tie). 

Keeping with the example above if we order the cards according to large group size (in this case two for two pair) and by highest rank when the size matches we should get the following:


(c = clubs, d = diamonds, h = hearts, s = spades)
before:
```
hand 1:  3c 8d 9h 8c 3d
hand 2:  5s 4s 8h 4h 8s
```
after:
```
hand 1: 8c 8d 3d 3c 9h
hand 2: 8s 8h 4h 4s 5s
```

after sorting the two hands can be compared card by card and the winner will be the hand with the first card that doesn't match and is higher. Notice that in a two pair it's not the highest pair that wins but the highest pair that is unique, so the eights don't result in a tie, but rather hand 2 wins because of the fours. Had both pairs been tied then hand 1 would have won with a high card of a nine and if the high cards had matched then it would have been a true tie. 

Lets a consider a full house:

before:
```
hand 1:  3c 8d 3h 8c 3d
hand 2:  4s 4s 8h 4h 8s
```
after:
```
hand 1: 3d 3c 3h 8c 8d
hand 2: 4h 4s 4s 8s 8h
```

in this case notice that the large group size comes first. In this case the set of three threes for hand 1 and the set of fours for hand 2. This is the key to the tie breaker. In a full house it's really the set of three cards that matter as it's impossible for hand 1 and hand 2 two have the same set of three cards, therefore in this case the first card will determine the winner of the hand which is hand 2. It's a universal way to compare any two hands, due to how it's sorted.

I would like to point out that the ranking module does not keep track of multiple hands or determine a winner. It simply orders the card for this purpose along with sending back the rank. 

#### Determining hand:
After the grouping and sorting find the hand type for the given hand becomes trivial. 

The ranking module first looks for any hands that are comprised of groups with a size larger than one, and does so in this order: 

```
FOUR_OF_A_KIND;
THREE_OF_A_KIND;
FULL_HOUSE;
TWO_PAIR;
PAIR;
```

If none of those hands are found, then it checks for:
```
STRAIGHT_FLUSH
STRAIGHT
FLUSH
```

If nothing is found it returns INVALID_HAND, however that case should never happen due to the validation step prior to ranking the hand.
