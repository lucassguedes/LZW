#include "file_formatter.h"

bool is_forbidden(char c){

    if(c == 0x20){ //space
        return false;
    }

    if((c >= 0x21 && c <= 0x40) || (c >= 0x5B && c <= 0x60)){
        return true;
    }
    return false;
}


bool format_file(const char* input_filepath, const char* output_filepath, char utf8_ascii_table[256][256]){

    FILE* file;
    FILE* outfile;

    file = fopen(input_filepath, "r");
    outfile = fopen(output_filepath, "w");

    char character;

    char last_char = '\0';
    while((character = fgetc(file)) != EOF){
        if(character == 0xD){
            fgetc(file);
            fprintf(outfile, " ");
        }else if(is_forbidden(character) || character == 0xD || (character & 0xFF) == 0xC2){
            fprintf(outfile, "");            
        }else if((character & 0x80) == 0){
            if(character == ' '){
                if(last_char == ' '){
                    continue;
                }
            }
            fprintf(outfile, "%c", tolower(character));
            last_char = character;
        }else if((character & 0xE0) == 0xE0){
            fgetc(file); fgetc(file);
        }else if((character & 0xc0) == 0xc0){
            char c2 = fgetc(file);
            fprintf(outfile, "%c", utf8_ascii_table[character & 0xFF][c2 & 0xFF]);
        }
    }

    fclose(file);
    fclose(outfile);

    return true;
}

void initialize_convertion_table(char utf8_ascii_table[256][256]){
    /*A*/
    utf8_ascii_table[0xc3][0x81] = 'a';
    utf8_ascii_table[0xc3][0x80] = 'a';
    utf8_ascii_table[0xc3][0x82] = 'a';
    utf8_ascii_table[0xc3][0x83] = 'a';
    utf8_ascii_table[0xc3][0x84] = 'a';

    /*a*/
    utf8_ascii_table[0xc3][0xA0] = 'a';
    utf8_ascii_table[0xc3][0xA1] = 'a';
    utf8_ascii_table[0xc3][0xA2] = 'a';
    utf8_ascii_table[0xc3][0xA4] = 'a';
    utf8_ascii_table[0xc3][0xA3] = 'a';

    /*E*/
    utf8_ascii_table[0xc3][0x8A] = 'e';
    utf8_ascii_table[0xc3][0x8B] = 'e';
    utf8_ascii_table[0xc3][0x88] = 'e';
    utf8_ascii_table[0xc3][0x89] = 'e';

    /*e*/
    utf8_ascii_table[0xc3][0xA8] = 'e';
    utf8_ascii_table[0xc3][0xA9] = 'e';
    utf8_ascii_table[0xc3][0xAA] = 'e';
    utf8_ascii_table[0xc3][0xAB] = 'e';

    /*I*/
    utf8_ascii_table[0xc3][0x8C] = 'i';
    utf8_ascii_table[0xc3][0x8D] = 'i';
    utf8_ascii_table[0xc3][0x8E] = 'i';
    utf8_ascii_table[0xc3][0x8F] = 'i';

    /*i*/
    utf8_ascii_table[0xc3][0xAC] = 'i';
    utf8_ascii_table[0xc3][0xAD] = 'i';
    utf8_ascii_table[0xc3][0xAE] = 'i';
    utf8_ascii_table[0xc3][0xAF] = 'i';

    /*O*/
    utf8_ascii_table[0xc3][0x92] = 'o';
    utf8_ascii_table[0xc3][0x93] = 'o';
    utf8_ascii_table[0xc3][0x94] = 'o';
    utf8_ascii_table[0xc3][0x95] = 'o';
    utf8_ascii_table[0xc3][0x96] = 'o';

    /*o*/
    utf8_ascii_table[0xc3][0xB2] = 'o';
    utf8_ascii_table[0xc3][0xB3] = 'o';
    utf8_ascii_table[0xc3][0xB4] = 'o';
    utf8_ascii_table[0xc3][0xB5] = 'o';
    utf8_ascii_table[0xc3][0xB6] = 'o';

    /*U*/
    utf8_ascii_table[0xc3][0x99] = 'u';
    utf8_ascii_table[0xc3][0x9A] = 'u';
    utf8_ascii_table[0xc3][0x9B] = 'u';
    utf8_ascii_table[0xc3][0x9C] = 'u';

    /*u*/
    utf8_ascii_table[0xc3][0xB9] = 'u';
    utf8_ascii_table[0xc3][0xBA] = 'u';
    utf8_ascii_table[0xc3][0xBB] = 'u';
    utf8_ascii_table[0xc3][0xBC] = 'u';

    /*Ç*/
    utf8_ascii_table[0xc3][0x87] = 'c';
    /*ç*/
    utf8_ascii_table[0xc3][0xA7] = 'c';

    /*Ñ*/
    utf8_ascii_table[0xc3][0x91] = 'n';

    /*ñ*/
    utf8_ascii_table[0xc3][0xB1] = 'n';
}

