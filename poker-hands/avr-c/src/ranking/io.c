#include "ranking/io.h"

void serial_init() {
    DDRD ^= TXD; // make sure the TXD pin is set to out
    UBRRH = (byte) ((MYUBRR) >> 8);
    UBRRL = (byte) MYUBRR;
    UCSRB = (1 << RXEN) | (1 << TXEN);
    UCSRC = 3 << UCSZ0;
}

byte serial_read() {
    // wait till RxComplete
    while ((UCSRA & _BV(RXC)) == 0);
    return UDR;
}

void serial_write(byte DataOut) {
    // wait till TxReady
    while ((UCSRA & _BV(UDRE)) == 0);
    UDR = DataOut;
}

#ifdef DEBUG_MODE
void write_buf(byte *buf) {
    for (byte *b = buf; *b != '\0'; b++) serial_write(*b);
}

void write_buf_newline(byte *buf) {
    write_buf(buf);
    serial_write('\n');
}
#endif
