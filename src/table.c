#include "table.h"
#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

commandTable* initTable() {
    commandTable* table = (commandTable *) malloc(sizeof(commandTable));
    table->entries = (entry **) malloc(TABLE_SIZE * sizeof(entry *));
    for (int i = 0; i < TABLE_SIZE; i++) {
        table->entries[i] = NULL;
    }
    return table;
}

uint32_t hash(const char* key) {
    uint32_t hash = 0;

    while (*key != '\0') {
        hash += (uint8_t)*key;
        hash += (hash << 10);
        hash ^= (hash >> 6);
        key++;
    }
    hash += (hash << 3);
    hash ^= (hash >> 11);
    hash += (hash << 15);

    return hash;
}

void addCommand(commandTable* table, const char* key, commandType* ct) {
    uint32_t idx = hash(key) % TABLE_SIZE;
    entry* temp = (entry *) malloc(sizeof(entry));
    temp->key = strdup(key);
    temp->type = ct;
    temp->next = table->entries[idx];
    table->entries[idx] = temp;  
}

commandType* getCommand(commandTable* table, const char* key) {
    uint32_t idx = hash(key) % TABLE_SIZE;
    entry* cur = table->entries[idx];
    while (cur != NULL) {
        if (strcmp(cur->key, key) == 0) {
            return cur->type;
        }
        cur = cur->next;
    }
    return NULL;
}

void buildCommandTable(commandTable* table) {
    addCommand(table, )
}
