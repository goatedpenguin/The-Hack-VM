#define _BSD_SOURCE 1
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include "types.h"
#include "table.h"
#include "parser.h"

static tokLine *tokenize(const char *line)
{
    tokLine *tok = (tokLine *)malloc(sizeof(tokLine));
    char *str1 = strdup(line);
    char *str2 = strdup(line);
    char *temp;
    tok->numToks = 0;

    for ((temp = strtok(str1, " ")); temp != NULL; temp = strtok(NULL, " "))
    {
        tok->numToks++;
    }
    tok->tokens = (char **)malloc(tok->numToks * sizeof(char *));
    int i = 0;
    for ((temp = strtok(str2, " ")); temp != NULL; temp = strtok(NULL, " "))
    {
        tok->tokens[i] = strdup(temp);
        i++;
    }

    free(str1);
    free(str2);
    return tok;
}

static vmWordType determineCommandType(tokLine *toks, table *hashTable)
{
    return getCommand(hashTable, toks->tokens[0])->tag;
}

static memorySegments getSegment(tokLine *toks, table *hashTable)
{
    return getCommand(hashTable, toks->tokens[1])->vmCommandType.segment;
}

static builtins getNativeOP(tokLine *toks, table *hashTable)
{
    return getCommand(hashTable, toks->tokens[0])->vmCommandType.builtin;
}

static branch getBranchType(tokLine *toks, table *hashTable)
{
    return getCommand(hashTable, toks->tokens[0])->vmCommandType.branchOperator;
}

static int getOffset(tokLine *toks)
{
    return atoi(toks->tokens[2]);
}

static char *getFuncName(tokLine *toks)
{
    return strdup(toks->tokens[1]);
}

static int getNumArgs(tokLine *toks)
{
    return atoi(toks->tokens[2]);
}

static int getNumLocalVars(tokLine *toks)
{
    return atoi(toks->tokens[2]);
}

VMPacket *buildVMPacket(const char *command, table *hashTable)
{
    tokLine *toks = tokenize(command);
    VMPacket *vmPacket = (VMPacket *)malloc(sizeof(VMPacket));
    vmPacket->type = determineCommandType(toks, hashTable);
    vmPacket->toks = toks;
    switch (vmPacket->type)
    {
    case BUILTIN:
        vmPacket->commandContext.nativeOperations.nativeOPS = getNativeOP(toks, hashTable);
        break;
    case BRANCH:
        vmPacket->commandContext.branching.branchCommand = getBranchType(toks, hashTable);
        break;
    case MEMORY_CMD:
        vmPacket->commandContext.memManage.offset = getOffset(toks);
        vmPacket->commandContext.memManage.segment = getSegment(toks, hashTable);
        vmPacket->commandContext.memManage.stackInstr = (getCommand(hashTable, toks->tokens[0])->vmCommandType.command == C_PUSH) ? PUSH : POP;
        break;
    case FUNC_CMD:
    {
        // Distinguish between function, call, and return which share FUNC_CMD in the table
        vmWord *entry = getCommand(hashTable, toks->tokens[0]);
        if (entry && entry->vmCommandType.command == F_FUNCTION)
        {
            vmPacket->commandContext.functionInfo.functionName = getFuncName(toks);
            vmPacket->commandContext.functionInfo.nLocalVars = getNumLocalVars(toks);
            vmPacket->commandContext.functionInfo.nArgs = 0;
        }
        else if (entry && entry->vmCommandType.command == F_CALL)
        {
            vmPacket->commandContext.functionInfo.functionName = getFuncName(toks);
            vmPacket->commandContext.functionInfo.nArgs = getNumArgs(toks);
            vmPacket->commandContext.functionInfo.nLocalVars = 0;
        }
        else
        { // F_RETURN
            vmPacket->commandContext.functionInfo.functionName = NULL;
            vmPacket->commandContext.functionInfo.nArgs = 0;
            vmPacket->commandContext.functionInfo.nLocalVars = 0;
        }
    }
    break;
    }
    return vmPacket;
}