#!/usr/bin/env bash

SYSTEM=$1 make fclean
SYSTEM=$1 make -C . helios.bin


