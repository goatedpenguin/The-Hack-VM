#ifndef PARSER_H
#define PARSER_H

typedef enum {
    MEM_TEMP,
    MEM_STATIC,
    MEM_LOCAL,
    MEM_ARG,
    MEM_THIS,
    MEM_THAT,
    MEM_POINTER,
    MEM_CONSTANT
} memorySegments;
typedef enum {
    A_ADD,
    A_SUB,
    A_NEG,
    A_EQ,
    A_NE,
    A_GT,
    A_LT,
    A_GTE,
    A_LTE,
    A_AND,
    A_OR,
    A_NOT
} nativeOPS;
typedef enum {
    A_NATIVEOPS,
    C_PUSH,
    C_POP,
    B_LABEL,
    B_GOTO,
    B_IFGOTO,
    F_FUNCTION,
    F_CALL,
    F_RETURN
} VMCommands;


typedef enum {
    SEGMENT,
    NATIVE,
    COMMAND
} tag;

typedef struct {
    tag commandContext;
    union {
        memorySegments segment;
        nativeOPS op;
        VMCommands command;
    } context;
} commandType;


typedef struct {
    VMCommands type;
    union {
        struct { 
            memorySegments segment;
            nativeOPS native;
            int offset;              // base index offset of memory segment
        } commandInfo;
        struct {
            char* functionName;     // label for function management           
            int nArgs;              // function arg count
            int nVars;              // lcl function vars
        } functionInfo;
    } commandContext;
} VMPacket;

// Builds the VMPakcet using static function in the source file
VMPacket* buildVMPacket(const char* command);


#endif