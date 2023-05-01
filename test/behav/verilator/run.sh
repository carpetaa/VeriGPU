#!/bin/bash

set -ex
set -o pipefail

SCRIPTDIR=$(dirname $0)
BASENAME=$(basename $SCRIPTDIR)
TESTNAME=$1

cd ${SCRIPTDIR}

if [[ ! -d build ]]; then {
    mkdir build
} fi

cd build

mkdir -p sim.out/cov_logs
mkdir -p sim.out/wave_dumps

export VERILATOR_ROOT="/usr/local/share/verilator"
cmake .. -DBUILD_NETLIST:bool=false
make -j $(nproc) core_and_mem
./core_and_mem +verilator+rand+reset+0 "${TESTNAME}"_0
./core_and_mem +verilator+rand+reset+1 "${TESTNAME}"_1
./core_and_mem +verilator+rand+reset+2 +verilator+seed+$(($RANDOM * 65536 + $RANDOM)) "${TESTNAME}"_2
