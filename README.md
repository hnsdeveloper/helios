# About the project

HeliOS is an operating system initially developed for the RISC-V ISA. It currently targets QEMU, but in the future it is intended that it run on real boards/computers.
It is developed mainly in C and C++, but additional support for other languages (such as Rust) might be added in the future. It is still on its baby-steps, thus it doens't do much.

## Contributing

The project currently doesn't accept contributions nor is looking for that (but it is open to suggestions/mentorship), given the way it is being developed (through sheer experimentation).

## Building and running

Building and running is currently only tested under MacOs. It should build on Linux as well, but not run (without doing any tweaks to the running script). 
For building, you will need the riscv64-unknown-elf toolchain, which can be obtained through HomeBrew or built manually. You will also need to have u-boot compiled with OpenSBI (see their respectives Github repos for this).
When building, set CXXPREFIX=riscv64-unknown-elf and run ```make helios.bin``` (you need to build a flat binary, given that the elf generated fails to run under u-boot, an issue which I am investigating). 
To run (after you place the generated files from u-boot/OpenSBI at the root directory of the project), just do ```./script.sh```. When you have the u-boot prompt ready, type in (and hit enter):

```
$> load scsi 0:1 0x84000000 /boot/helios.bin
$> go 0x84000000 --fdt ${fdtcontroladdr}
```

