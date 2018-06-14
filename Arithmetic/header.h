#include <stdint.h>

#ifndef HEADER_H
#define HEADER_H

#define FREQ_MAX 0xFFFF
#define FREQ_MIN 0x0000

#include <stdio.h>

extern FILE *in, *out;

void write_header();
void read_header();

#endif
