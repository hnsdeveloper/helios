#!/usr/bin/env bash

export ARCH=riscv64
export CXXPREFIX=riscv64-unknown-elf-
SYSTEM=$1 make clean
SYSTEM=$1 make -C . all CPLUS_INCLUDE_PATH=/usr/riscv64-linux-gnu/include/c++/12/riscv64-linux-gnu/:/usr/riscv64-linux-gnu/include:/usr/include/c++/12:/usr/include:/usr/local/include:/usr/local/include/efi C_INCLUDE_PATH=/usr/riscv64-linux-gnu/include/c++/12/riscv64-linux-gnu/:/usr/riscv64-linux-gnu/include:/usr/include/c++/12:/usr/include:/usr/local/include:/usr/local/include/efi


