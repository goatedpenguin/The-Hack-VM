The hack vm implementation in C
Usage: ./hackvm <file.vm|directory> -> produces <file.asm> or <directory>/<directory>.asm
Build (optimized): gcc -O3 -march=native -flto src/*.c -o hackvm
