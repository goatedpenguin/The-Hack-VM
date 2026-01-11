; PUSH IMPLEMENTATION

; push segment offset

@OFFSET
D=A

@SEGMENT
A=A+D
D=M

@SP
A=M
M=D

@SP
M=M+1

; statc push

push static i:
@FILENAME.i
D=M

@SP
A=M
M=D

@SP
M=M+1

;constant push

push constant i:
@i
D=A

@SP
A=M
M=D

@SP
M=M+1

; pointer push(0 is this and 1 is that)
; pointer 0 accesses the THIS pointer
; pointer 1 accesses the THAT pointer

; push pointer 0:

@THIS
D=M

@SP
A=M
M=D

@SP
M=M+1

; push pointer 1:
@THAT
D=M

@SP
A=M
M=D

@SP
M=M+1




; POP IMPLEMENTATION:
; pop segment offset

; save top stack val
; put in RAM[segment + (offset)]

@SP
M=M-1
A=M
D=M // D = X
@SEGMENT
D=D+M 
@OFFSET
D=D+A // D = X + POP ADDR
@SP
A=M
A=M // A = X

A=A+D // A = 2X + POP ADDR
D=A-D // D = X
A=A-D // A = X + POP ADDR 
A=A-D // A = POP ADDR
M=D  


; static pop implementation
; pop static 2 -> store val in Foo.2
; filename = Foo

@SP
M=M-1
A=M
D=M // D = x

@Foo.i
M=D


; pop pointer 0:
@SP
M=M-1
A=M
D=M 

@THIS
M=D

; pop pointer 1:
@SP
M=M-1
A=M
D=M 

@THAT
M=D


; 1. Arithmetic & Logical Operations (Builtins)

; Binary operations (pop two values, compute, push result):

; The following works for add, sub, and, or -> A %s B %s = op

@SP
AM=M-1
D=M // D=B (top of the stack)

@SP
AM=M-1
D=D%sM // D = A %s B

M=D

; Unary operations (operate on top of stack):

; neg

; not

@SP
AM=M-1
M=%sM


; Comparison operations (pop two values, compute boolean, push result):

; eq

; gt

; lt

comp(A, B) -> true = -1(0xFF), false = 0(0x0)

@SP
AM=M-1
D=M // D = B

@SP
AM=M-1
D=M-D // D = A - B

@if-true
%s ;%

D=0
@SP
A=M
M=D
@end
JMP

(if-true)
D=-1
@SP
A=M
M=D

(end)



; 2. Branching / Labels

; label <X> → define a label for jumps.

(labelName)

; goto <X> → unconditional jump.

@(labelName)
JMP

; if-goto <X> → pop top stack value, jump if non-zero.
@SP
M=M-1
A=M

D=M
D=D+1
@labelName
JEQ

; 3. Function keyword implementations:

; call <f> nArgs
; need static int n to inc everytime
; push returnAddr ; same label (@functionName$ret.n) is used by "return" keyword
@functionName$ret.n
D=A
@SP
A=M
M=D
@SP
M=M+1
; push lcl
@LCL
A=M
D=M
@SP
A=M
M=D
@SP
M=M+1
; push arg
@ARG
A=M
D=M
@SP
A=M
M=D
@SP
M=M+1
; push this
@THIS
A=M
D=M
@SP
A=M
M=D
@SP
M=M+1
; push that
@THAT
A=M
D=M
@SP
A=M
M=D
@SP
M=M+1
; repos arg so that the function can access them when it begins executing
; ARG = SP - 5 - nArgs
@nArgs ; given by call <f> nArgs
D=A
@5
D=D+A
@SP
D=M-D
@ARG
M=D
; Set lcl = sp to push the local variables of the called function 
; LCL = SP
@SP
D=M
@LCL
M=D

; jump to called function
@className.function
JMP

(functionName$ret.n) ; label for return addr since after call we have other vm commands i.e. call bar.mult then neg then so and so forth 
                ; we need to remeber the return address of neg

; function <f> nLocals

(className.function) ; declare the function label for jumping into later (done via call) 
; LOOP nVars times
    ; push 0


(LOOP)



@LOOP
JMP

; exectue the normal function commands inside the function body as per normal

; return 

endFrame = *LCL ; temp var endFrame
retAddr = *(endFrame - 5)
*ARG = pop() ; ret val points to arg 0
SP = *ARG + 1 ; restore SP
THAT = *(endFrame - 1)
THIS = *(endFrame - 2)
ARG = *(endFrame - 3)
LCL = *(endFrame - 4)
goto retAddr






; 4. Optional / Helpers

; Sys.init bootstrap code (to set SP and optionally call the first function).
