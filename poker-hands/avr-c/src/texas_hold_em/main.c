#include "poker.h"
#include "hand.h"

int main(void)
{
    DDRB = (1 << DDB0);
    PORTB = 0;

    while(1)
    {
        int clock = 0;
        PORTB = (1 << DDB0); 
        while (clock < 100) {
            clock++;
            _delay_ms(10);
        }

        clock = 0;
        PORTB ^= (1 << DDB0); 
        while (clock < 100) {
            clock++;
            _delay_ms(10);
        }
    }
}
