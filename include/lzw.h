#ifndef __LZW_H__
#define __LZW_H__
#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "hash.h"
#include "encoder.h"

void add_to_dict(Item** dictionary, char* phrase, int* curr_code, int* curr_code_length);
void compress_file(char* filepath, char* outfilepath, char* model_filepath, bool save_model, bool load_model);
void decompress_file(char* filepath, char* outfilepath);


#endif