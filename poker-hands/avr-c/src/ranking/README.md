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

#### Semantics
When the ranking module spins up it will initialize a few things and then block listening for a ready signal from the client. The ready signal is one byte and the value is 0xff. Once the signal is received the ranking module will then send back one byte which is the acknowledgment and it's value is 0xfe. 

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
