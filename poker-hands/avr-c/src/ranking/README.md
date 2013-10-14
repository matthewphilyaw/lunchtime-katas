## Overview

The ranking module (aka ranker 4000) takes in a sinle hand, performs some simple validation on the hand to make sure cards
are legal within some basic rules, and then sorts and ranks the hand sending it back to the client.

## Validation

This module does some basic validation to ensure that the cards sent to the hardware module are legal within the context of
the hand given. 

####The checks this module performs are:
+ Ensure there is not a 5 of a kind, which implies that 5 cards with the same face value were passed to the module
+ Ensure that the cards passed in are valid cards. 
+ Ensure that the suit is unique among any group of cards with the same face value. No two cards with the same face value
should ever have the same suit

If any of those checks fail and INVALID_HAND is sent back to the calling client. 

These checks are only in the context of the given hand, so it doesn't check to see if a particular card has been used by
a previous hand thus the given hand should not contain it (for example this would apply to five card draw and seven card stud)


