#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "codegen.h"
#include "parser.h"
#include "mnemonics.h"

static char* writeMemory(VMPacket * VMPacket) {

}

static char* writeBuiltin(VMPacket* vmPacket) {

}

static char* writeBranch(VMPacket* vmPacket) {

} 

static char* writeFunc(VMPacket* VMPacket) {
    
}

char* codeGen(VMPacket* vmPacket) {
    char* res;
    switch (vmPacket->type) {
        case MEMORY_CMD:
        res = writeMemory(vmPacket);
        break;
    case BUILTIN:
        res = writeBuiltin(vmPacket);
        break;
    case BRANCH:
        res = writeBranch(vmPacket);
        break;
    case FUNC_CMD:
        res = writeFunc(vmPacket);
        break;
    case SEGMENT:
        res = NULL;
        break;
    default:
        res = NULL;
        break;
    }
    return res;
}