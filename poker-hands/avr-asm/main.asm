; Program   :   Poker hand kata
;
;

.include "tn2313def.inc"

.equ baud = 115200
.equ F_CPU = 14745600

.def trbuf r18

    rjmp reset
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt
    reti ; unused interrupt

reset:
    ldi r16, low(RAMEND)
    out SPL, r16
    rjmp main

usart_init:
    ldi r16, (1 << DDD1) ; Ensure TX pin is set to output
    out DDRD, r16

    ldi r17, high(F_CPU/(16*baud)-1)
    ldi r16, low(F_CPU/(16*baud)-1)
    out UBRRH, r17
    out UBRRL, r16

    ldi r16, (1 << RXEN)|(1 << TXEN) ; Enable receiver and transmitter
    out UCSRB, r16
    ldi r16, (0 << USBS)|(3 << UCSZ0)
    out UCSRC, r16
    ret

tx:
    sbi UCSRA, UDRE
    rjmp tx
    out UDR, trbuf
    ret

rx:
    sbi UCSRA, RXC
    rjmp rx
    in trbuf, UDR
    ret

main:
    RCALL usart_init
    rjmp echo

echo:
    RCALL rx
    RCALL tx
    rjmp echo
