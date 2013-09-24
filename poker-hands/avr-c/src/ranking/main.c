#include "ranking.h"
#define GROUP_SIZE 7

Group groups[GROUP_SIZE];


int main(void) {
    // init groups with negate one, which indicates empty.
    for (byte i = 0; i < GROUP_SIZE; i++) {
        Group g = { .rank = -1, 
                    .size = -1 };
        for (byte ig = 0; ig < GROUP_CARD_SIZE; ig++) {
            g.cards[ig] = (Card) { .rank = -1,
                                   .suit = -1,
                                   .wild = -1 };
        }
        groups[i] = g;
    } 

    // set up ports for status lights.
    DDRB = 0xff;
    DDRD = _BV(1);

    // turn on a status light 
    // will go off when serial com is setup
    PORTB = _BV(0);

    // start serial 
    UBRRH = (byte) (MYUBRR >> 8);
    UBRRL = (byte) MYUBRR;
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = 3 << UCSZ0;

    establishContact();

    PORTB ^= _BV(0); 

    byte card_count = 0;
    byte buf[GROUP_SIZE];
    for (;;) {
        for (byte i = 0; i < GROUP_SIZE; i++) buf[i] = 0;
        if (serialCheckRxComplete() && card_count < 7) {
            buf[card_count] = serialRead();
            card_count++;
        }
        PORTB ^= _BV(0);
    }
}

void read_card(byte raw_card) {
    Card card = { .rank = raw_card & 0x0f,
                  .suit = (raw_card & 0x70) >> 4,
                  .wild = (raw_card & 0x80) >> 7 };

    byte pos = card_in_group(&card);

    if (pos >= 0) {
        add_to_group(card, pos);
        return;
    }

    insert_into_groups(card);
}

byte card_in_group(Card *current) {
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank < 0) continue;
        if (current->rank == groups[i].rank)
            return i;
    }
    // negative number indicates not found
    return -1;
}

void insert_into_groups(Card current) {
    // we don't have a group, we need to make one.
    Group grp = { .rank = current.rank,
                  .cards[0] = current,
                  .size = 1 };
    
    // find first available slot. 
    // the rank field was chosen here to indicate empty.
    // A negative one or any value less than zero is considered empty.
    for (byte i = 0; i < GROUP_SIZE; i++) {
        if (groups[i].rank < 0) continue;
        groups[i] = grp; 
    }
}

void add_to_group(Card current, byte pos) {
    for (byte i = 0; i < GROUP_CARD_SIZE; i++) {
        if (groups[pos].cards[i].rank < 0) continue;
        groups[pos].cards[i] = current;
        return;
    } 
}

void sort_groups() {

}

void delayLong() {
	unsigned int delayvar;
	delayvar = 0; 
	while (delayvar <=  65500U)		
	{ 
		__asm("nop");  
		delayvar++;
	} 
}

byte serialCheckRxComplete() {
	return(UCSRA & _BV(RXC));		
}

byte serialCheckTxReady() {
	return(UCSRA & _BV(UDRE));
}

byte serialRead() {
	while (serialCheckRxComplete() == 0) {;;} 
	return UDR;
}

void serialWrite(byte DataOut) {
	while (serialCheckTxReady() == 0) {;;} 
	UDR = DataOut;
}

void establishContact() {
	while (serialCheckRxComplete() == 0) { 
		serialWrite('A'); 
		delayLong();
		delayLong();
		delayLong();
		delayLong();
		delayLong();
		delayLong();
		delayLong();
	}
}
