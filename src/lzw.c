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

/*Utilizado no processo de descompressão*/
void add_to_dict_at(Item** dictionary, uint64_t index, char* phrase, int* curr_code_length){
    Token newtok;
    
    newtok.code.value = 0;
    newtok.code.length = *curr_code_length;
    newtok.counter = 0;
    newtok.repr = (char*)malloc(sizeof(char)*(strlen(phrase) + 1));
    strcpy(newtok.repr, phrase);
    add_item_at(dictionary, newtok, index);

    free(newtok.repr);
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

void decompress_file(char* filepath, char* outfilepath){
    FILE* file = fopen(filepath, "r");
    FILE* outfile = fopen(outfilepath, "w");

    char byte;
    char buffer[100];

    if(file == NULL){
        printf("\033[0;31mErro ao acessar o arquivo de entrada!\033[0m\n");
        exit(-1);
    }

    if(outfile == NULL){
        printf("\033[0;31mErro ao acessar o arquivo de saída!\033[0m\n");
        exit(-1);
    }

    const int dict_size = 50000;

    Item** dictionary = malloc(sizeof(Item*)*dict_size);

    for(int i = 0; i < dict_size; i++) dictionary[i] = NULL;

    uint64_t curr_code = 0;
    int curr_code_length = 8;

    char c = ' ';
    sprintf(buffer, "%c", c);
    
    add_to_dict_at(dictionary, 0, buffer, &curr_code_length);

    printf("Dicionário:\n");
    printf("0 -  \n");

    for(char c = 'a'; c <= 'z'; c++){
        sprintf(buffer, "%c", c);
        printf("%d - %s\n", c - 'a' + 1, buffer);
        add_to_dict_at(dictionary, c - 'a' + 1, buffer, &curr_code_length);
    }

    int remaining_bits = curr_code_length;
    int extracted;
    Token* item = NULL;
    Token* prev_item = NULL;

    sprintf(buffer, "");
    int newcode = 26;


    fseek(file, 0L, SEEK_END);

    // Calculando o tamanho do arquivo (em bytes)
    uint32_t file_size = ftell(file);
    printf("Size of file: %d\n", file_size);
    fseek(file, 0L, SEEK_SET);

    uint32_t byte_counter = 0;
    while((byte = fgetc(file)) != EOF){
        printf("Progress: %d/%d\n", byte_counter, file_size);
        if(remaining_bits < 8){
            curr_code |= byte & ((int)pow(2, remaining_bits) - 1);
            extracted = remaining_bits;
            byte = byte >> remaining_bits;
        }else{
            curr_code |= byte;
            extracted = 8;
        }
        remaining_bits -= extracted;
        byte_counter++;


        if(!remaining_bits){//Se os bits do código atual foram todos lidos
            //Verifica se o símbolo cujo código foi lido já está no dicionário
            prev_item = item;
            item = get_item_at(dictionary, curr_code);

            strcat(buffer, item->repr);

            if(prev_item == NULL) {
                prev_item = item;
            }else{
                printf("Símbolo encontrado: %s\n", item->repr);
                add_to_dict_at(dictionary, ++newcode, buffer, &curr_code_length);
                printf("Adicionando \033[0;35m\"%s\"\033[0m ao dicionário, com código %d\n", buffer, newcode);
                
                if(byte_counter == file_size){
                    fprintf(outfile, "%s", buffer);
                    printf("[byte_counter = %d, file_size = %d] - Escrevendo \033[0;35m\"%s\"\033[0m na saída\n", byte_counter, file_size, buffer);
                }else{
                    fprintf(outfile, "%s", prev_item->repr);
                    printf("[byte_counter = %d, file_size = %d] - Escrevendo \033[0;35m\"%s\"\033[0m na saída\n", byte_counter, file_size, prev_item->repr);
                }
            }
            sprintf(buffer, "%s", item->repr);
            printf("String encontrada no dicionário: \033[0;35m\"%s\"\033[0m\n", item->repr);
            getchar();

            remaining_bits = curr_code_length;
            curr_code = 0;
            continue;
        }
        curr_code = byte;
        curr_code = curr_code << extracted;
    }

    fclose(file);
    fclose(outfile);

    
}