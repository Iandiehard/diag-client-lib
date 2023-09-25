#!/bin/bash

DIAG_LIB_SRC_PATH="./diag-client-lib"
DIAG_LIB_EXAMPLE_PATH="./examples"
DIAG_LIB_TEST_PATH="./test"
CLANG_FORMAT_VERSION="14"

DIAG_LIB_SRC_LIST="$(find "$DIAG_LIB_SRC_PATH" | grep -E ".*(\.ino|\.cpp|\.c|\.h|\.hpp|\.hh)$")"
DIAG_LIB_EXAMPLE_LIST="$(find "$DIAG_LIB_EXAMPLE_PATH" | grep -E ".*(\.ino|\.cpp|\.c|\.h|\.hpp|\.hh)$")"
DIAG_LIB_TEST_LIST="$(find "$DIAG_LIB_TEST_PATH" | grep -E ".*(\.ino|\.cpp|\.c|\.h|\.hpp|\.hh)$")"

clang-format-$CLANG_FORMAT_VERSION--verbose -i --style=file "$DIAG_LIB_SRC_LIST" "$DIAG_LIB_EXAMPLE_LIST" "$DIAG_LIB_TEST_LIST"