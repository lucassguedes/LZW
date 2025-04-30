#ifndef __HASH_H__
#define __HASH_H__
#include "encoder.h"
#include "utils.h"
#include <math.h>
#include <string.h>

#define A 0.6180339887
#define M pow(2, 14)

/*Representa um item de hash map (lista encadeada de símbolos)*/
typedef struct Item{
    struct Item* next;
    Token* value;
}Item;

int     hash(char* key);
void    add_item(Item* map[], Token symb);
void    add_item_at(Item* map[], Token symb, uint64_t index);
void    remove_item(Item* map[], char* key);
void    increment_item(Item* map[], char* key);
void    decrement_item(Item* map[], char* key);
Token*  get_item(Item* map[], char* key);
Token*  get_item_at(Item* map[], uint64_t index);
void    show_map(Item* map[], int size);
void    destroy_map(Item* map[], int size);


#endif