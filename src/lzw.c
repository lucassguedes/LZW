#include "lzw.h"

void add_to_dict(Item** dictionary, char* phrase, int* curr_code, int* curr_code_length){
    Token newtok;
    
    newtok.code.value = *curr_code;
    newtok.code.length = *curr_code_length;
    newtok.counter = 0;
    newtok.repr = (char*)malloc(sizeof(char)*strlen(phrase) + 1);
    strcpy(newtok.repr, phrase);
    add_item(dictionary, newtok);

    (*curr_code)++;

    if(log2(*curr_code) > *curr_code_length){
        (*curr_code_length)++;
    }
}

void compress_file(char* filepath, char* outfilepath){
    FILE* file = fopen(filepath, "r");
    FILE* outfile = fopen(outfilepath, "w");

    printf("filepath: %s\n", filepath);

    const int dict_size = 50000;

    Item** dictionary = malloc(sizeof(Item*)*dict_size);

    char buffer[100];
    


    int curr_code = 0;
    int curr_code_length = 8;

    char c = ' ';
    sprintf(buffer, "%c", c);
    
    add_to_dict(dictionary, buffer, &curr_code, &curr_code_length);

    for(char c = 'a'; c <= 'z'; c++){
        sprintf(buffer, "%c", c);
        add_to_dict(dictionary, buffer, &curr_code, &curr_code_length);
    }

    char byte;

    Token* item = NULL;
    Token* prev_item = NULL;
    int counter = 0; /*Contador de caracteres na frase*/

    uint8_t outbuffer;
    int remaining_bits = 8;
    while((byte = fgetc(file)) != EOF){
        printf("byte: %c\n", byte);
        getchar();

        if(!counter){
            sprintf(buffer, "%c", byte);
        }else{
            buffer[counter] = byte;
            buffer[counter + 1] = '\0';
        }

        prev_item = item;
        item = get_item(dictionary, buffer);

        if(item != NULL){// A frase já está no dicionário, continuamos a leitura
            printf("A frase \"%s\" está no dicionário...\n", buffer);
            counter++;
            continue;
        }
        
        /*Adicionando ao dicionário*/
        printf("Adicionando \"%s\" ao dicionário...\n", buffer);
        add_to_dict(dictionary, buffer, &curr_code, &curr_code_length);

        write_code_to_file(outfile, prev_item, &outbuffer, &remaining_bits);
    
        byte = buffer[counter];
        sprintf(buffer, "%c", byte);
        counter = 1;
        item = get_item(dictionary, buffer);
        getchar(); 
    }

    write_code_to_file(outfile, item, &outbuffer, &remaining_bits);
    
    fclose(file);
}