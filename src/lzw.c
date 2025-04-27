#include "lzw.h"

void compress_file(char* filepath){
    FILE* file = fopen(filepath, "r");

    printf("filepath: %s\n", filepath);

    const int dict_size = 50000;

    Item** dictionary = malloc(sizeof(Item*)*dict_size);

    char buffer[100];
    Token newtok;

    char c = ' ';
    sprintf(buffer, "%c", c);
    newtok.code.length = newtok.code.value = 0;
    newtok.counter = 0;
    newtok.repr = (char*)malloc(sizeof(char)*strlen(buffer) + 1);
    strcpy(newtok.repr, buffer);
    add_item(dictionary, newtok);

    for(char c = 'a'; c <= 'z'; c++){
        sprintf(buffer, "%c", c);
        newtok.code.length = newtok.code.value = 0;
        newtok.counter = 0;
        newtok.repr = (char*)malloc(sizeof(char)*strlen(buffer) + 1);
        strcpy(newtok.repr, buffer);
        add_item(dictionary, newtok);
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
        newtok.code.length = 0;
        newtok.code.value = 0;
        newtok.counter = 0;
        newtok.repr = (char*)malloc(sizeof(char)*strlen(buffer) + 1);
        strcpy(newtok.repr, buffer);

        printf("Adicionando \"%s\" ao dicionário...\n", buffer);
        byte = buffer[counter];
        sprintf(buffer, "%c", byte);
        counter = 0;
        getchar(); 
        add_item(dictionary, newtok);

        counter++;
    }
    
    fclose(file);
}