#include "lzw.h"

void add_to_dict(Item** dictionary, char* phrase, int* curr_code, int* curr_code_length){
    Token newtok;
    
    newtok.code.value = *curr_code;
    newtok.code.length = *curr_code_length;
    newtok.counter = 0;
    newtok.repr = (char*)malloc(sizeof(char)*strlen(phrase) + 1);
    strcpy(newtok.repr, phrase);
    add_item(dictionary, newtok);

    curr_code++;

    if(log2(*curr_code) > *curr_code_length){
        (*curr_code_length)++;
    }
}

void compress_file(char* filepath){
    FILE* file = fopen(filepath, "r");

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


    int counter = 0; /*Contador de caracteres na frase*/
    while((byte = fgetc(file)) != EOF){
        printf("byte: %c\n", byte);
        getchar();

        if(!counter){
            sprintf(buffer, "%c", byte);
        }else{
            buffer[counter] = byte;
            buffer[counter + 1] = '\0';
        }

        if(get_item(dictionary, buffer) != NULL){ // Se a frase já está no dicionário, mandamos para a sáida
            printf("A frase \"%s\" está no dicionário...\n", buffer);
            counter++;
            continue;
        }
        
        /*Adicionando ao dicionário*/
        printf("Adicionando \"%s\" ao dicionário...\n", buffer);
        add_to_dict(dictionary, buffer, &curr_code, &curr_code_length);
    
        byte = buffer[counter];
        sprintf(buffer, "%c", byte);
        counter = 1;
        getchar(); 
    }
    
    fclose(file);
}