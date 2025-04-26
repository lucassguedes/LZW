#include "hash.h"

void add_item(Item* map[], Token symb){
    int index = hash(symb.repr);    
    Item* new_item = (Item*)malloc(sizeof(Item));
    new_item->value = (Token*)malloc(sizeof(Token));
    new_item->value->code = symb.code;
    new_item->value->counter = symb.counter;
    new_item->value->repr = (char*)malloc(sizeof(char)*(strlen(symb.repr) + 1));
    new_item->next = NULL;
    strcpy(new_item->value->repr, symb.repr);

    if(map[index] != NULL){
        Item* it = map[index];

        while(it->next != NULL){
            it = it->next;
        }

        it->next = new_item;
    }else{
        map[index] = new_item;
    }
}

void remove_item(Item* map[], char* key){
    const int index = hash(key);
    
    Item* it = map[index];
    Item* prev = NULL;
    while(it != NULL){
        if(!strcmp(it->value->repr, key)){//Se encontrou o símbolo
            if(prev != NULL){
                prev->next = it->next;
            }else{
                map[index] = it->next;
            }

            free(it->value->repr);
            free(it->value);
            free(it);
            break;
        }
        prev = it;
        it = it->next;
    }
}

void decrement_item(Item* map[], char* key){
    const int index = hash(key);
    
    Item* it = map[index];
    Item* prev = NULL;
    while(it != NULL){
        if(!strcmp(it->value->repr, key)){//Se encontrou o símbolo
            it->value->counter--;

            if(!it->value->counter){
                if(prev != NULL){
                    prev->next = it->next;
                }else{
                    map[index] = it->next;
                }

                free(it->value->repr);
                free(it->value);
                free(it);
            }
            break;
        }
        prev = it;
        it = it->next;
    }
}

void increment_item(Item* map[], char* key){
    const int index = hash(key);
    Item* it = map[index];
    while(it != NULL){
        if(!strcmp(it->value->repr, key)){//Se encontrou o símbolo
            it->value->counter++;
            break;
        }
        it = it->next;
    }
}

Token* get_item(Item* map[], char* key){
    int index = hash(key);

    Item* it = map[index];

    while(it != NULL){
        if(!strcmp(it->value->repr, key))
        {
            return it->value;
        }
        it = it->next;
    }

    return NULL; 
}

void show_map(Item* map[], int size){
    int counter = 1;
    char buffer[33];
    for(int i = 0; i < size; i++){
        Item* it = map[i];
        while(it != NULL){
            printf("Item %d:\n", counter);
            printf("\tToken: %s\n", it->value->repr);
            printf("\tCounter: %d\n", it->value->counter);
            get_bin_str(it->value, buffer);
            printf("\tCode: %s\n", buffer);
            it = it->next;
            counter++;
        }
    }
}

void destroy_map(Item* map[], int size){
	for(int i = 0; i < size; i++){
		Item* it = map[i];
		Item* next;
		while(it != NULL){
			next = it->next;

			free(it->value->repr);
			free(it->value);
			free(it);
			it = next;
		}
        map[i] = NULL;
	}
    free(map);
}


int hash(char* key){
	/**Interpretando a string como um número natural.*/
	int n = strlen(key);
	int k = 0;
	for(int i = 0; i < n; i++){
		k += (int)key[i] * pow(128, n - i - 1);
	}

	/*Aplicando o método da multiplicação*/
	return (int)floor(M*(k*A - floor(k*A)));
}

int compare_Tokens(const void * a, const void * b){
    Token* sa = *(Token**)a;
    Token* sb = *(Token**)b;
	int counter_cmp = sa->counter - sb->counter;
	if(!counter_cmp){
		return strcmp(sb->repr, sa->repr);
	}

	return counter_cmp;
}
