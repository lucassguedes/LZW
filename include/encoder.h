#ifndef __ENCODER_H__
#define __ENCODER_H__
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct Code{
    uint64_t value;
    int length;
}Code;

typedef struct Symbol{
    char* repr;
    int counter;
    Code code;
}Symbol;

void write_code_to_file(FILE* outfile, Symbol* sb, uint8_t* outbuffer, int* remaining_bits);

#endif