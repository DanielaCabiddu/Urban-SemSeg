#!/bin/bash
SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

DATA_FOLDER=${SCRIPT_DIR}/data
BSP_CELL_SIZE=250000

mkdir -p ${SCRIPT_DIR}/output

cd ${SCRIPT_DIR}/build/bsp
./bsp -d ${DATA_FOLDER} -o ${SCRIPT_DIR}/output/ -v ${BSP_CELL_SIZE}

cd ${SCRIPT_DIR}/build/ransac

FILES="${SCRIPT_DIR}/output/*.xyz"
for f in $FILES
do
	echo "=============================="
	echo "RANSAC: $f ..."
	# take action on each file. $f store current file name
	./ransac -i $f -P -C
	echo "=============================="
done

