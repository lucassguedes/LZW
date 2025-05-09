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
    free(newtok.repr);
}

/*Utilizado no processo de descompressão*/
void add_to_dict_at(Item** dictionary, int* dict_size, uint64_t index, char* phrase, int* curr_code_length){
    Token newtok;
    
    newtok.code.value = 0;
    newtok.code.length = *curr_code_length;
    newtok.counter = 0;
    newtok.repr = (char*)malloc(sizeof(char)*(strlen(phrase) + 1));
    strcpy(newtok.repr, phrase);
    add_item_at(dictionary, newtok, index);
    (*dict_size)++;

    if((pow(2,*curr_code_length)) == *dict_size){
        printf("\033[0;31mDicionário deve crescer para %d bits..\033[0m\n", *curr_code_length + 1);
        (*curr_code_length)++;
    }
    free(newtok.repr);
}

void compress_file(char* filepath, char* outfilepath, char* model_filepath, bool save_model, bool load_model){
    FILE* file = fopen(filepath, "r");
    FILE* outfile = fopen(outfilepath, "w");

    printf("filepath: %s\n", filepath);

    const int dict_size = 50000;

    Item** dictionary = malloc(sizeof(Item*)*dict_size);

    char buffer[100];
    


    int curr_code = 0;
    int curr_code_length = 8;

    if(!load_model){
        char c = ' ';
        sprintf(buffer, "%c", c);
        
        add_to_dict(dictionary, buffer, &curr_code, &curr_code_length);

        for(char c = 'a'; c <= 'z'; c++){
            sprintf(buffer, "%c", c);
            add_to_dict(dictionary, buffer, &curr_code, &curr_code_length);
        }
    }else{
        FILE* modelfile = fopen(model_filepath, "r");

        char byte;

        printf("Abrindo arquivo de modelo: %s\n", model_filepath);

        Code code;
    
        char code_buff[100];
        char length_buff[100];
        char phrase_buff[100];
        char character_str[100];

        sprintf(code_buff, "");
        sprintf(length_buff, "");
        sprintf(phrase_buff, "");

        while (fgets(buffer, sizeof(buffer), modelfile) != NULL) {
            int i;
            for(i = 0; i < strlen(buffer) && buffer[i] != ','; i++){
                sprintf(character_str, "%c", buffer[i]);
                strcat(code_buff, character_str);
            }

            // printf("Code: %s, ", code_buff);

            int j;
            for(j = i + 1; j < strlen(buffer) && buffer[j] != ','; j++){
                sprintf(character_str, "%c", buffer[j]);
                strcat(length_buff, character_str);
            }

            // printf("Length: %s, ", length_buff);
            
            int k;
            for(k = j + 1; k < strlen(buffer) && buffer[k] != '\n'; k++){
                sprintf(character_str, "%c", buffer[k]);
                strcat(phrase_buff, character_str);
            }

            // printf("Phrase: %s\n", phrase_buff);

            int code_value = atoi(code_buff);
            
            add_to_dict(dictionary, phrase_buff, &code_value, &curr_code_length);
            curr_code++;

            sprintf(code_buff, "");
            sprintf(length_buff, "");
            sprintf(phrase_buff, "");

        }

        fclose(modelfile);
    }

    char byte;

    Token* item = NULL;
    Token* prev_item = NULL;
    int counter = 0; /*Contador de caracteres na frase*/

    uint8_t outbuffer;
    int remaining_bits = 8;
    int written_bits = 0;

    fseek(file, 0L, SEEK_END);

    // Calculando o tamanho do arquivo (em bytes)
    uint32_t file_size = ftell(file);
    printf("Size of file: %d\n", file_size);
    fseek(file, 0L, SEEK_SET);


    while((byte = fgetc(file)) != EOF){
        printf("curr_code: %d, byte: %c\n", curr_code, byte);

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

        written_bits += curr_code_length;
        if(!save_model){
            write_code_to_file(outfile, prev_item, curr_code_length, &outbuffer, &remaining_bits);
        }
        getchar();
    
        byte = buffer[counter];
        sprintf(buffer, "%c", byte);
        counter = 1;
        item = get_item(dictionary, buffer);
    }

    written_bits += curr_code_length;
    if(!save_model){
        write_code_to_file(outfile, item, curr_code_length, &outbuffer, &remaining_bits);
    }

    printf("Outbuffer available space: %d\n", remaining_bits);

    if(remaining_bits){
        written_bits += curr_code_length;
        if(!save_model){
            outbuffer = outbuffer << remaining_bits;
            fputc(outbuffer, outfile);
        }
    }

    if(save_model){
        for(int i = 0; i < dict_size; i++){
            if(dictionary[i] != NULL){
                fprintf(outfile, "%ld,%d,%s\n", dictionary[i]->value->code.value, dictionary[i]->value->code.length, dictionary[i]->value->repr);
            }
        }
    }

    printf("Bits escritos: %d\n", written_bits);
    printf("Comprimento médio: %.2f bits/símbolo\n", written_bits / (double) file_size);


    destroy_map(dictionary, dict_size);
    
    fclose(outfile);
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

    const int dict_size = 5000000;

    Item** dictionary = malloc(sizeof(Item*)*dict_size);

    for(int i = 0; i < dict_size; i++) dictionary[i] = NULL;

    uint64_t curr_code = 0;
    int curr_code_length = 8;

    char c = ' ';
    sprintf(buffer, "%c", c);
    
    int actual_dict_size = 0;

    add_to_dict_at(dictionary, &actual_dict_size, 0, buffer, &curr_code_length);

    printf("Dicionário:\n");
    printf("0 -  \n");

    for(char c = 'a'; c <= 'z'; c++){
        sprintf(buffer, "%c", c);
        printf("%d - %s\n", c - 'a' + 1, buffer);
        add_to_dict_at(dictionary, &actual_dict_size, c - 'a' + 1, buffer, &curr_code_length);
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
    int ignored_bits;

    int last_add_idx;
    bool first_symbol = true;
    while(byte_counter <= file_size){
        printf("curr_code: %d\n", curr_code);
        byte = fgetc(file);
        printf("Progress: %d/%d\n", byte_counter, file_size);
        uint8_t ubyte = byte;
        printf("Unsigned byte: %u, Dict size: %d\n", ubyte, actual_dict_size);

        if(remaining_bits < 8){
            ignored_bits = 8 - remaining_bits; //Número de bits ignorados (mais significativos)
            printf("\033[0;35mMáscara:\033[0m %d\n", (255 - ((int)pow(2, ignored_bits) - 1)));
            curr_code |= (ubyte & (255 - ((int)pow(2, ignored_bits) - 1))) >> ignored_bits;
            extracted = remaining_bits;
        }else{
            curr_code |= ubyte;
            extracted = 8;
        }
        remaining_bits -= extracted;
        byte_counter++;


        if(!remaining_bits){//Se os bits do código atual foram todos lidos
            //Verifica se o símbolo cujo código foi lido já está no dicionário
            prev_item = item;
            printf("curr_code: %lu, current byte: %u\n", curr_code, ubyte);
            item = get_item_at(dictionary, curr_code);

            if(prev_item == NULL){
                prev_item = item;
            }else{
                printf("Corrigindo no dicionário, o que era \033[0;35m\"%s\"\033[0m tornou-se ", dictionary[last_add_idx]->value->repr);
                dictionary[last_add_idx]->value->repr[strlen(dictionary[last_add_idx]->value->repr) - 1] = item->repr[0];
                printf("\033[0;35m\"%s\"\033[0m\n", dictionary[last_add_idx]->value->repr);
            }

            sprintf(buffer, "%s", item->repr);


            printf("Símbolo encontrado: %s, index = %lu, bits: %d\n", item->repr, curr_code, curr_code_length);
            
            strcat(buffer, "?");
            add_to_dict_at(dictionary, &actual_dict_size, ++newcode, buffer, &curr_code_length);

            if(buffer[4] == '?'){
                printf("\033[0;33mBuffer: %s\033[0m\n", buffer);
                getchar();
            }

            printf("Adicionando \033[0;35m\"%s\"\033[0m ao dicionário, com código %d\n", buffer, newcode);
            
            if(!first_symbol){
                if(byte_counter == file_size){
                    printf("dictionary[last_add_idx]->value->repr: %s\n", dictionary[last_add_idx]->value->repr);
                    fprintf(outfile, "%s", dictionary[last_add_idx]->value->repr);
                    printf("[byte_counter = %d, file_size = %d] - Escrevendo \033[0;35m\"%s\"\033[0m na saída\n", byte_counter, file_size, dictionary[last_add_idx]->value->repr);
                }else{
                    fprintf(outfile, "%s", prev_item->repr);
                    printf("[byte_counter = %d, file_size = %d] - Escrevendo \033[0;35m\"%s\"\033[0m na saída\n", byte_counter, file_size, prev_item->repr);
                    for(int k = 0; k < strlen(prev_item->repr); k++){
                        if(prev_item->repr[k] == '?'){
                            getchar();
                        }
                    }
                }
            }
            last_add_idx = newcode;

            first_symbol = false;
            
            // getchar();

            
            sprintf(buffer, "%s", item->repr);
            printf("String encontrada no dicionário: \033[0;35m\"%s\"\033[0m\n", item->repr);
            
            remaining_bits = curr_code_length;
            
            if(extracted < 8){

                printf("\033[0;33m[Ramo 1] O byte NÃO foi completamente lido (só foram lidos %d bits). Faltam %d bits, inserindo valor restante no próximo...\033[0m\n", extracted , ignored_bits);
                curr_code = ubyte & ((int)pow(2, ignored_bits) - 1);
                remaining_bits -= ignored_bits;
                printf("\tcodigo: %d\n", curr_code);
                printf("\tBits restantes: %d\n", remaining_bits);

                int shift = (remaining_bits < 8) ? remaining_bits : 8; 
    
                if(remaining_bits == 0){
                    printf("\033[0;31m[Ramo 1] ALERTA: remaining_bits = 0\033[0m\n");
                    // getchar();
                }

                curr_code = curr_code << shift;    
                continue;
            }
            curr_code = 0;
            continue;
        }

        //Se chegou até aqui, é porque ainda faltam bits a serem lidos para completar o código
        //Determinando quantos bits devem ser liberados no código, para receber os que estão por vir
        int shift = (remaining_bits < 8) ? remaining_bits : 8; 

        if(extracted == 8){// SE o byte atual foi completamente lido
            printf("\033[0;32mO byte foi completamente lido...\033[0m\n");
            printf("\tcodigo: %d\n", curr_code);
            printf("\tBits restantes: %d\n", remaining_bits);
            curr_code = curr_code << shift; //Liberamos espaço para os bits que estão por vir
        }else{//Caso contrário, ainda sobraram bits no byte atual, colocamos o restante em um novo código
            printf("\033[0;33mO byte NÃO foi completamente lido (só foram lidos %d bits). Faltam %d bits, inserindo valor restante no próximo...\033[0m\n", extracted , ignored_bits);
            curr_code = ubyte & ((int)pow(2, ignored_bits) - 1);
            remaining_bits -= ignored_bits;
            printf("\tcodigo: %d\n", curr_code);
            printf("\tBits restantes: %d\n", remaining_bits);

            shift = (remaining_bits < 8) ? remaining_bits : 8; 

            if(remaining_bits == 0){
                printf("\033[0;31m[Ramo 2] ALERTA: remaining_bits = 0\033[0m\n");
                // getchar();
            }


            curr_code = curr_code << remaining_bits;
        }
        


    }

    destroy_map(dictionary, dict_size);

    fclose(file);
    fclose(outfile);

    
}