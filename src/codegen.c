#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "codegen.h"
#include "parser.h"
#include "mnemonics.h"
#include <string.h>
#include <stdarg.h>

static int labelCounter = 0;
static int returnCounter = 0;

static char *emit_pushConstant(int value);
static char *emit_pushSegment(const char *segment, int index); // LCL, ARG, THIS, THAT
static char *emit_pushTemp(int index);                         // temp 0–7
static char *emit_pushPointer(int index);                      // 0=THIS, 1=THAT
static char *emit_pushStatic(const char *filename, int index);

static char *emit_popSegment(const char *segment, int index);
static char *emit_popTemp(int index);
static char *emit_popPointer(int index);
static char *emit_popStatic(const char *filename, int index);

static char *emit_binaryOp(const char *op);            // "+", "-", "&", "|"
static char *emit_unaryOp(const char *op);             // "-", "!"
static char *emit_compareOp(const char *jump, int id); // "JEQ","JGT","JLT"

static char *emit_labelStr(const char *label_name);
static char *emit_VMgotoStr(const char *label_name);
static char *emit_if_gotoStr(const char *label_name);

static char *emit_functionStr(const char *function_name, int nLocals, const char *filename);
static char *emit_callStr(const char *function_name, int nArgs, const char *filename);
static char *emit_VMreturnStr(void);

char *codeGen(VMPacket *vmPacket, const char *filename);