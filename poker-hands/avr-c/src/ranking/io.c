#include "ranking/io.h"

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
