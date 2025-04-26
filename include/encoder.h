#ifndef __ENCODER_H__
#define __ENCODER_H__
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

typedef struct Code{
    uint64_t value;
    int length;
}Code;

typedef struct Token{
    char* repr;
    int counter;
    Code code;
}Token;

void write_code_to_file(FILE* outfile, Token* sb, uint8_t* outbuffer, int* remaining_bits);

#endif