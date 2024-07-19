# About the project

HeliOS is an operating system initially developed for the RISC-V ISA. It currently targets QEMU, but in the future it is intended that it run on real boards/computers.
It is developed mainly in C and C++, but additional support for other languages (such as Rust) might be added in the future. It is still on its baby-steps, thus it doens't do much.

## Contributing

The project currently doesn't accept contributions nor is looking for that (but it is open to suggestions/mentorship), given the way it is being developed (through sheer experimentation).

## Building 

Building is currently done through a Debian Linux container, which already contains the necessary environment to build the kernel. If you want to do it locally, you can analize the files under `.github`.

[![RISC-V 64 building status](https://github.com/heliobatimarqui/helios/actions/workflows/riscv64build.yml/badge.svg)](https://github.com/heliobatimarqui/helios/actions/workflows/riscv64build.yml)


## Running 
Running the kernel is supported on QEMU and on the Starfive Visionfive 2 board.
To run it on QEMU, you simply have to run `./script.sh`. Be aware that you must have u-boot SPL as the bios/firmware for QEMU to run it. Instructions are provided at u-boot's github on how to. Then, when the u-boot prompt shows up, you type in:

```
$> load scsi 0:1 0x84000000 /boot/helios.bin
$> go 0x84000000 --fdt ${fdtcontroladdr}
```
