#ifndef RANKING_IO_H
#define RANKING_IO_H

#include "ranking.h"

byte serial_read();
void serial_write(byte DataOut);
#ifdef DEBUG_MODE
void write_buf(byte *buf);
void write_buf_newline(byte *buf);
#endif

#endif
