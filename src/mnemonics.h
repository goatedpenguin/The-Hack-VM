#ifndef MNEMONICS_H
#define MNEMONICS_H

// push constant i: @i, D=A, @SP, A=M, M=D, @SP, M=M+1
static const char* pushConstant[] = {
    "@%d",        // @i
    "D=A",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

// push segment offset: @OFFSET, D=A, @SEGMENT, A=A+D, D=M, @SP, A=M, M=D, @SP, M=M+1
static const char* pushSegment[] = {
    "@%d",        // @OFFSET
    "D=A",
    "@%s",        // @SEGMENT (LCL, ARG, THIS, THAT, or R5 for temp)
    "A=M+D",      // A=A+D for temp (no M indirection), A=M+D for others
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

// push pointer 0/1: @THIS/@THAT, D=M, @SP, A=M, M=D, @SP, M=M+1
static const char* pushPointer[] = {
    "@%s",        // @THIS or @THAT
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

// push static i: @FILENAME.i, D=M, @SP, A=M, M=D, @SP, M=M+1
static const char* pushStatic[] = {
    "@%s.%d",     // @FILENAME.i
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL
};

// pop segment offset (complex arithmetic to avoid temp register)
static const char* popSegment[] = {
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",        // D = X
    "@%s",        // @SEGMENT
    "D=D+M",      // D = X + segment_base (or D=D+A for temp)
    "@%d",        // @OFFSET
    "D=D+A",      // D = X + POP_ADDR
    "@SP",
    "A=M",
    "A=M",        // A = X
    "A=A+D",      // A = 2X + POP_ADDR
    "D=A-D",      // D = X
    "A=A-D",      // A = X + POP_ADDR
    "A=A-D",      // A = POP_ADDR
    "M=D",        // *POP_ADDR = X
    NULL
};

// pop pointer 0/1: @SP, M=M-1, A=M, D=M, @THIS/@THAT, M=D
static const char* popPointer[] = {
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@%s",        // @THIS or @THAT
    "M=D",
    NULL
};

// pop static i: @SP, M=M-1, A=M, D=M, @FILENAME.i, M=D
static const char* popStatic[] = {
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@%s.%d",     // @FILENAME.i
    "M=D",
    NULL
};

#endif