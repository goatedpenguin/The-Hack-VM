#!/bin/bash
set -euo pipefail

# build
gcc -g src/*.c -o hackvm

# run generator on tests
./hackvm tests/simple/BasicTest.vm
./hackvm tests/simple/TempTest.vm
./hackvm tests/simple/PointerTest.vm
./hackvm tests/simple/StaticTest.vm
./hackvm tests/simple/BranchTest.vm

tests=(BasicTest TempTest PointerTest StaticTest BranchTest)
pass_all=0
for t in "${tests[@]}"; do
    OUT=tests/simple/${t}.asm
    EXPECT=tests/simple/${t}.expected.asm
    if diff -u "$EXPECT" "$OUT" >/dev/null; then
        echo "PASS: $t"
    else
        echo "FAIL: $t - see diff"
        diff -u "$EXPECT" "$OUT" || true
        pass_all=1
    fi
done

rm hackvm
if [ "$pass_all" -eq 0 ]; then
    exit 0
else
    exit 2
fi
