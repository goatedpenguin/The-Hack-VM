#ifndef PARSER_H
#define PARSER_H
#include "types.h"
#include "table.h"

typedef struct {
    char** tokens;
    int numToks;
} tokLine;

typedef struct {
    vmWordType type;
    tokLine* toks;
    union {
        struct {
            builtins nativeOPS;
        } nativeOperations;

        struct {
            instr stackInstr;
            memorySegments segment;
            int offset;              // base index offset of memory segment
        } memManage;
        
        struct {
            branch branchCommand;
        } branching;
        
        struct {
            char* functionName;     // label for function management           
            int nArgs;              // function arg count
            int nLocalVars;              // lcl function vars
        } functionInfo;
    } commandContext;
} VMPacket;

// Builds the VMPakcet using static function in the source file
// Takes in a 3 tables for segments, nativeOPS, and commands.
// indicies of the table: 0->commands, 1->segments, 2->builtins 
VMPacket* buildVMPacket(const char* command, table* hashTable);

#endif