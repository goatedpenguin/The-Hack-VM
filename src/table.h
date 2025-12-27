#ifndef TABLE_H
#define TABLE_H

#define TABLE_SIZE 37
#include "parser.h"
#include <stdint.h>

typedef struct commandEntry {
    char* key;
    commandType* type;
    struct commandEntry* next;
} entry;

// Note this is a read only table hence, the fixed entry size.
typedef struct {
    entry** entries;
} commandTable;

commandTable* initTable();
uint32_t hash(const char* key);
void addCommand(commandTable* table, const char* key, commandType* ct);
commandType* getCommand(commandTable* table, const char* key);
void buildCommandTable(commandTable* table);


#endif