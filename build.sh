#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

mkdir -p ${SCRIPT_DIR}/build
cd ${SCRIPT_DIR}/build
cmake ../src/
cmake --build . --parallel 16
