#!/usr/bin/env bash

export ARCH=riscv64
export CROSS_COMPILE=riscv64-linux-gnu-
SYSTEM=$1 make clean
SYSTEM=$1 make -C . all 


