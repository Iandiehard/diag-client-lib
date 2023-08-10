#!/bin/bash

DIAG_LIB_PATH="./diag-client-lib"
CLANG_FORMAT="clang-format-14"

DIAG_LIB_LIST="$(find "$DIAG_LIB_PATH" | grep -E ".*(\.ino|\.cpp|\.c|\.h|\.hpp|\.hh)$")"

$CLANG_FORMAT --verbose -i --style=file $DIAG_LIB_LIST
