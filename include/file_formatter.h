#ifndef __FILE_FORMATTER_H__
#define __FILE_FORMATTER_H__
#include <stdbool.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>

bool is_forbidden(char c);

void initialize_convertion_table(char utf8_ascii_table[256][256]);

bool format_file(const char* input_filepath, const char* output_filepath, char utf8_ascii_table[256][256]);







#endif 
