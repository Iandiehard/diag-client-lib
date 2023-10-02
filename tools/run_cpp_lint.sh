#!/bin/bash
DIAG_LIB_SRC_PATH="../diag-client-lib"

cpplint --quiet --counting=detailed --linelength=120 --recursive $DIAG_LIB_SRC_LIST