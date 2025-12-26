#include "parser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Fill elementary command fields
static VMCommands determineCommandType(char* command);
static memorySegments getSegment(char* command);
static nativeOPS getNativeOP(char* command);
static int getOffset(char* command);

// Fill functional command info
static char* getFuncName(char* command);
static int getNumArgs(char* command);
static int getNumVars(char* command);