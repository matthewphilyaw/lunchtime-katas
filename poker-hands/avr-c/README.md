##Overview

poker-hands/avr-c is a take on [this kata] (http://codingdojo.org/cgi-bin/wiki.pl?KataPokerHands). 
It aims to take it a little bit further and produce a complete hardware solution for running a poker game
based around the attiny micro's from atmel.

##High Level Design

Primary design would be a three tier design where the first tier would handle the communication between the client and hardware
module using a common serial protocol. The client would stream the hands for a poker round to the hardware module and that
module would eventually return the results of the round. 

The first tier would delegate to the second tier for game rules to handle variations of the game poker. An example would be
that in tier two you have texas hold em module, a seven card stud module and a 5 card draw module and depending on the game
sent be the client the first tier would route the hands to the correct module on in tier to for the corresponding game chosen.

Tier three would be ranking, which would only consist of a single hand sent to this tier and the hand would be sent back
ranked/scored. This process would repeat till all hands have been ranked/scored and then the second tier could score the game
and send back the result to tier one, which would the notify the client of the result. 
