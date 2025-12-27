#include "parser.h"
#include "table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

// Fill elementary command fields
static VMCommands determineCommandType(const char* command);
static memorySegments getSegment(const char* command);
static nativeOPS getNativeOP(const char* command);
static int getOffset(const char* command);

// Fill functional command info
static char* getFuncName(const char* command);
static int getNumArgs(const char* command);
static int getNumVars(const char* command);

// Construct packet object from functions above for codegen
VMPacket* buildVMPacket(const char* command);