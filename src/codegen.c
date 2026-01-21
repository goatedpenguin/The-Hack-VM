#include <stdio.h>
#include <stdlib.h>
#include "types.h"
#include "codegen.h"
#include "parser.h"
#include "mnemonics.h"
#include <string.h>
#include <stdarg.h>

static char **emit_pushConstant(int value);
static char **emit_pushSegment(const char *segment, int index); // LCL, ARG, THIS, THAT
static char **emit_pushTemp(int index);                         // temp 0–7
static char **emit_pushPointer(int index);                      // 0=THIS, 1=THAT
static char **emit_pushStatic(const char *filename, int index);

static char **emit_popSegment(const char *segment, int index);
static char **emit_popTemp(int index);
static char **emit_popPointer(int index);
static char **emit_popStatic(const char *filename, int index);

static char **emit_binaryOp(const char *op);            // "+", "-", "&", "|"
static char **emit_unaryOp(const char *op);             // "-", "!"
static char **emit_compareOp(const char *jump, int id); // "JEQ","JGT","JLT"

static char **emit_labelStr(const char *label_name);
static char **emit_VMgotoStr(const char *label_name);
static char **emit_if_gotoStr(const char *label_name);

static char **emit_functionStr(const char *function_name, int nLocals, const char *filename);
static char **emit_callStr(const char *function_name, int nArgs, const char *filename);
static char **emit_VMreturnStr(void);

char *codeGen(VMPacket *vmPacket, const char *filename);

// Helper: join a formatted template (array of lines) into a single string.
static char *format_template(const char **tmpl, ...)
{
    if (!tmpl)
        return NULL;
    // combine template lines into one format string with newlines
    size_t cap = 256;
    size_t len = 0;
    char *fmt_comb = malloc(cap);
    fmt_comb[0] = '\0';
    for (int i = 0; tmpl[i] != NULL; i++)
    {
        size_t piece = strlen(tmpl[i]);
        if (len + piece + 2 > cap)
        {
            while (len + piece + 2 > cap)
                cap *= 2;
            fmt_comb = realloc(fmt_comb, cap);
        }
        memcpy(fmt_comb + len, tmpl[i], piece);
        len += piece;
        fmt_comb[len++] = '\n';
        fmt_comb[len] = '\0';
    }

    va_list args;
    va_start(args, tmpl);
    va_list args_copy;
    va_copy(args_copy, args);
    int needed = vsnprintf(NULL, 0, fmt_comb, args_copy);
    va_end(args_copy);

    char *out = malloc(needed + 1);
    vsnprintf(out, needed + 1, fmt_comb, args);
    va_end(args);
    free(fmt_comb);
    return out;
}

static char *join_two(const char *a, const char *b)
{
    size_t la = a ? strlen(a) : 0;
    size_t lb = b ? strlen(b) : 0;
    char *out = malloc(la + lb + 1);
    if (a)
        memcpy(out, a, la);
    if (b)
        memcpy(out + la, b, lb);
    out[la + lb] = '\0';
    return out;
}

static int unique_cmp_id = 0;
static int unique_call_id = 0;

static char **emit_pushConstant(int value)
{
    char *s = format_template(pushConstant, value);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_pushSegment(const char *segment, int index)
{
    char *s = format_template(pushSegment, index, segment);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_pushTemp(int index)
{
    char *s = format_template(pushTemp, index);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_pushPointer(int index)
{
    const char *sym = (index == 0) ? "THIS" : "THAT";
    char *s = format_template(pushPointer, sym);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_pushStatic(const char *filename, int index)
{
    char *s = format_template(pushStatic, filename, index);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_popSegment(const char *segment, int index)
{
    char *s = format_template(popSegment, segment, index);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_popTemp(int index)
{
    char *s = format_template(popTemp, index);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_popPointer(int index)
{
    const char *sym = (index == 0) ? "THIS" : "THAT";
    char *s = format_template(popPointer, sym);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_popStatic(const char *filename, int index)
{
    char *s = format_template(popStatic, filename, index);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_binaryOp(const char *op)
{
    char *s = format_template(binaryOp, op);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_unaryOp(const char *op)
{
    char *s = format_template(unaryOp, op);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_compareOp(const char *jump, int id)
{
    char *s = format_template(compareOp, id, jump, id, id, id);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_labelStr(const char *label_name)
{
    char *s = format_template(label, label_name);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_VMgotoStr(const char *label_name)
{
    char *s = format_template(VMgoto, label_name);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_if_gotoStr(const char *label_name)
{
    char *s = format_template(if_goto, label_name);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static void split_funcname(const char *fullname, char *left, size_t lcap, char *right, size_t rcap)
{
    const char *dot = strchr(fullname, '.');
    if (!dot)
    {
        strncpy(left, fullname, lcap - 1);
        left[lcap - 1] = '\0';
        right[0] = '\0';
    }
    else
    {
        size_t llen = dot - fullname;
        if (llen >= lcap)
            llen = lcap - 1;
        memcpy(left, fullname, llen);
        left[llen] = '\0';
        strncpy(right, dot + 1, rcap - 1);
        right[rcap - 1] = '\0';
    }
}

static char **emit_callStr(const char *function_name, int nArgs, const char *filename)
{
    char cls[128];
    char fn[128];
    split_funcname(function_name, cls, sizeof(cls), fn, sizeof(fn));
    int id = unique_call_id++;
    // call template expects: function_name, id, nArgs, cls, fn, function_name, id
    char *s = format_template(call, function_name, id, nArgs, cls, fn, function_name, id);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_functionStr(const char *function_name, int nLocals, const char *filename)
{
    char cls[128];
    char fn[128];
    split_funcname(function_name, cls, sizeof(cls), fn, sizeof(fn));
    // function template uses many repeated %s.%s placeholders and %d
    char *s = format_template(function, cls, fn, nLocals, cls, fn, cls, fn, cls, fn, cls, fn);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

static char **emit_VMreturnStr(void)
{
    char *s = format_template(VMreturn);
    char **arr = malloc(2 * sizeof(char *));
    arr[0] = s;
    arr[1] = NULL;
    return arr;
}

char *codeGen(VMPacket *vmPacket, const char *filename)
{
    if (!vmPacket)
        return NULL;
    char *out = NULL;
    char **lines = NULL;

    if (vmPacket->type == BUILTIN)
    {
        switch (vmPacket->commandContext.nativeOperations.nativeOPS)
        {
        case A_ADD:
            lines = emit_binaryOp("+");
            break;
        case A_SUB:
            lines = emit_binaryOp("-");
            break;
        case A_AND:
            lines = emit_binaryOp("&");
            break;
        case A_OR:
            lines = emit_binaryOp("|");
            break;
        case A_NEG:
            lines = emit_unaryOp("-");
            break;
        case A_NOT:
            lines = emit_unaryOp("!");
            break;
        case A_EQ:
            lines = emit_compareOp("JEQ", unique_cmp_id++);
            break;
        case A_GT:
            lines = emit_compareOp("JGT", unique_cmp_id++);
            break;
        case A_LT:
            lines = emit_compareOp("JLT", unique_cmp_id++);
            break;
        default:
            break;
        }
    }
    else if (vmPacket->type == BRANCH)
    {
        const char *lbl = vmPacket->toks->tokens[1];
        switch (vmPacket->commandContext.branching.branchCommand)
        {
        case B_LABEL:
            lines = emit_labelStr(lbl);
            break;
        case B_GOTO:
            lines = emit_VMgotoStr(lbl);
            break;
        case B_IFGOTO:
            lines = emit_if_gotoStr(lbl);
            break;
        default:
            break;
        }
    }
    else if (vmPacket->type == MEMORY_CMD)
    {
        int off = vmPacket->commandContext.memManage.offset;
        memorySegments seg = vmPacket->commandContext.memManage.segment;
        instr st = vmPacket->commandContext.memManage.stackInstr;
        if (st == PUSH)
        {
            switch (seg)
            {
            case MEM_CONSTANT:
                lines = emit_pushConstant(off);
                break;
            case MEM_LOCAL:
                lines = emit_pushSegment("LCL", off);
                break;
            case MEM_ARG:
                lines = emit_pushSegment("ARG", off);
                break;
            case MEM_THIS:
                lines = emit_pushSegment("THIS", off);
                break;
            case MEM_THAT:
                lines = emit_pushSegment("THAT", off);
                break;
            case MEM_TEMP:
                lines = emit_pushTemp(off);
                break;
            case MEM_POINTER:
                lines = emit_pushPointer(off);
                break;
            case MEM_STATIC:
                lines = emit_pushStatic(filename, off);
                break;
            default:
                break;
            }
        }
        else
        { // POP
            switch (seg)
            {
            case MEM_LOCAL:
                lines = emit_popSegment("LCL", off);
                break;
            case MEM_ARG:
                lines = emit_popSegment("ARG", off);
                break;
            case MEM_THIS:
                lines = emit_popSegment("THIS", off);
                break;
            case MEM_THAT:
                lines = emit_popSegment("THAT", off);
                break;
            case MEM_TEMP:
                lines = emit_popTemp(off);
                break;
            case MEM_POINTER:
                lines = emit_popPointer(off);
                break;
            case MEM_STATIC:
                lines = emit_popStatic(filename, off);
                break;
            default:
                break;
            }
        }
    }
    else if (vmPacket->type == FUNC_CMD)
    {
        // parser sets this for function/call/return; distinguish by command tokens
        // If functionName is present and nLocalVars >= 0 it's a function; for call the buildVMPacket sets functionInfo for both call and function
        // Heuristic: if vmPacket->toks->tokens[0] starts with "function" -> function, if "call" -> call, else if "return" -> return
        const char *kw = vmPacket->toks->tokens[0];
        if (strcmp(kw, "function") == 0)
        {
            lines = emit_functionStr(vmPacket->commandContext.functionInfo.functionName,
                                     vmPacket->commandContext.functionInfo.nLocalVars,
                                     filename);
        }
        else if (strcmp(kw, "call") == 0)
        {
            lines = emit_callStr(vmPacket->commandContext.functionInfo.functionName,
                                 vmPacket->commandContext.functionInfo.nArgs,
                                 filename);
        }
        else if (strcmp(kw, "return") == 0)
        {
            lines = emit_VMreturnStr();
        }
    }

    if (lines && lines[0])
    {
        out = strdup(lines[0]);
        free(lines[0]);
        free(lines);
    }
    return out;
}