#ifndef __HASH_H__
#define __HASH_H__
#include "encoder.h"

#define A 0.6180339887
#define M pow(2, 14)

/*Representa um item de hash map (lista encadeada de símbolos)*/
typedef struct Item{
    struct Item* next;
    Symbol* value;
}Item;

int     hash(char* key);
void    add_item(Item* map[], Symbol symb);
void    remove_item(Item* map[], char* key);
void    increment_item(Item* map[], char* key);
void    decrement_item(Item* map[], char* key);
Symbol* get_item(Item* map[], char* key);
void    show_map(Item* map[], int size);
void    destroy_map(Item* map[], int size);


#endif