#ifndef MNEMONICS_H
#define MNEMONICS_H
#include <stddef.h>

/* push constant:
 *  %d -> integer literal to push onto the stack
 */
static const char *pushConstant[] = {
    "@%d",
    "D=A",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL};

/* push segment (local/argument/this/that):
 *  %d -> offset/index into the segment
 *  %s -> segment symbol (LCL, ARG, THIS, THAT)
 */
static const char *pushSegment[] = {
    "@%d",
    "D=A",
    "@%s",
    "A=D+M",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL};

/* push temp:
 *  %d -> offset into temp segment (0..7)
 *  temp base is R5 (address 5)
 */
static const char *pushTemp[] = {
    "@R5",
    "D=A",
    "@%d",
    "A=D+A",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL};

/* push pointer:
 *  %s -> pointer symbol ("THIS" or "THAT")
 */
static const char *pushPointer[] = {
    "@%s",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL};

/* push static:
 *  %s -> file/namespace for static variables
 *  %d -> static index
 *  expands to @<filename>.<index>
 */
static const char *pushStatic[] = {
    "@%s.%d",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    NULL};

/* pop segment (local/argument/this/that):
 *  %s -> segment symbol (LCL, ARG, THIS, THAT)
 *  %d -> offset/index into the segment
 */
static const char *popSegment[] = {
    "@%s",
    "D=M",
    "@%d",
    "D=D+A",
    "@R13",
    "M=D",
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@R13",
    "A=M",
    "M=D",
    NULL};

/* pop temp:
 *  %d -> offset into temp segment (0..7)
 */
static const char *popTemp[] = {
    "@R5",
    "D=A",
    "@%d",
    "D=D+A",
    "@R13",
    "M=D",
    "@SP",
    "AM=M-1",
    "D=M",
    "@R13",
    "A=M",
    "M=D",
    NULL};

/* pop pointer:
 *  %s -> pointer symbol ("THIS" or "THAT") where the popped value will be stored
 */
static const char *popPointer[] = {
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@%s",
    "M=D",
    NULL};

/* pop static:
 *  %s -> file/namespace for static variables
 *  %d -> static index
 *  stores top-of-stack into @<filename>.<index>
 */
static const char *popStatic[] = {
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@%s.%d",
    "M=D",
    NULL};

/* binary operation:
 *  %s -> infix operator inserted between operands (e.g. "+", "-", "&", "|")
 *  produces D=M%sD (e.g. D=M+D, D=M-D)
 */
static const char *binaryOp[] = {
    "@SP",
    "AM=M-1",
    "D=M",
    "@SP",
    "AM=M-1",
    "D=M%sD",
    "M=D",
    "@SP",
    "M=M+1",
    NULL};

/* unary operation:
 *  %s -> unary operator applied to memory cell ("-" for neg, "!" for not)
 *  forms M=%sM (e.g. M=-M)
 */
static const char *unaryOp[] = {
    "@SP",
    "AM=M-1",
    "M=%sM",
    "@SP",
    "M=M+1",
    NULL};

/* compare operation:
 *  %d -> unique id to avoid label collisions (used in IF_TRUE.%d, END.%d)
 *  %s -> jump condition mnemonic (JEQ, JGT, JLT)
 *  emits branching code that pushes -1 for true and 0 for false
 */
static const char *compareOp[] = {
    "@SP",
    "AM=M-1",
    "D=M",
    "@SP",
    "AM=M-1",
    "D=M-D",
    "@IF_TRUE.%d",
    "D;%s",
    "D=0",
    "@SP",
    "A=M",
    "M=D",
    "@END.%d",
    "0;JMP",
    "(IF_TRUE.%d)",
    "D=-1",
    "@SP",
    "A=M",
    "M=D",
    "(END.%d)",
    "@SP",
    "M=M+1",
    NULL};

/* label declaration:
 *  %s -> label name to declare
 */
static const char *label[] = {
    "(%s)",
    NULL};

/* unconditional goto:
 *  %s -> destination label name
 */
static const char *VMgoto[] = {
    "@%s",
    "0;JMP",
    NULL};

/* if-goto:
 *  %s -> destination label to jump to if top-of-stack != 0
 */
static const char *if_goto[] = {
    "@SP",
    "M=M-1",
    "A=M",
    "D=M",
    "@%s",
    "D;JNE",
    NULL};

/* call sequence:
 *  %s -> function name used as namespace for return label
 *  %d -> unique return id
 *  also uses %s.%s in the jump target representing <class>.<function>
 */
static const char *call[] = {
    "@%s$ret.%d",
    "D=A",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@LCL",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@ARG",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@THIS",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@THAT",
    "D=M",
    "@SP",
    "A=M",
    "M=D",
    "@SP",
    "M=M+1",
    "@%d",
    "D=A",
    "@5",
    "D=D+A",
    "@SP",
    "D=M-D",
    "@ARG",
    "M=D",
    "@SP",
    "D=M",
    "@LCL",
    "M=D",
    "@%s.%s",
    "0;JMP",
    "(%s$ret.%d)",
    NULL};

/* function declaration / local initialization:
 *  %s.%s -> namespace-qualified function label (e.g. <class>.<function>)
 *  %d -> number of local variables to initialize to 0
 *  repeated %s.%s placeholders are used to name loop and exit labels
 */
static const char *function[] = {
    "(%s.%s)",
    "@%d",
    "D=A",
    "(%s.%s$LOOP)",
    "@%s.%s$EXIT",
    "D;JEQ",
    "@SP",
    "A=M",
    "M=0",
    "@SP",
    "M=M+1",
    "D=D-1",
    "@%s.%s$LOOP",
    "0;JMP",
    "(%s.%s$EXIT)",
    NULL};

static const char *VMreturn[] = {
    "@LCL",
    "D=M",
    "@R13",
    "M=D",
    "@5",
    "A=D-A",
    "D=M",
    "@R14",
    "M=D",
    "@SP",
    "AM=M-1",
    "D=M",
    "@ARG",
    "A=M",
    "M=D",
    "@ARG",
    "D=M",
    "D=D+1",
    "@SP",
    "M=D",
    "@R13",
    "AM=M-1",
    "D=M",
    "@THAT",
    "M=D",
    "@R13",
    "AM=M-1",
    "D=M",
    "@THIS",
    "M=D",
    "@R13",
    "AM=M-1",
    "D=M",
    "@ARG",
    "M=D",
    "@R13",
    "AM=M-1",
    "D=M",
    "@LCL",
    "M=D",
    "@R14",
    "A=M",
    "0;JMP",
    NULL};

#endif