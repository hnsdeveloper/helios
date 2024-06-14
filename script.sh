#!/usr/bin/env bash

kernel_image=./build/helios
hdd_image_name=hdd

echo $kernel_image

if [[ -f "$hdd_image_name.dmg" ]]; then  
    rm "$hdd_image_name.dmg"
fi

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    touch "$hdd_image_name.dmg"
    dd if=/dev/zero of="$hdd_image_name.dmg" bs=1M count=1
elif [[ "$OSTYPE" == "darwin"* ]]; then
    hdiutil create -size 1024m -fs fat32 -volname HeliOSQEMU $hdd_image_name
    hdiutil attach $hdd_image_name.dmg
    mkdir /Volumes/HELIOSQEMU/boot
    riscv64-unknown-elf-objcopy -O binary $kernel_image $kernel_image.bin
    cp $kernel_image           /Volumes/HELIOSQEMU/boot/helios.elf
    cp $kernel_image.bin       /Volumes/HELIOSQEMU/boot/helios.bin
    hdiutil detach /Volumes/HELIOSQEMU
else  
    exit
fi

qemu-system-riscv64 -serial stdio -monitor unix:qemu-monitor-socket,server,nowait -parallel none -machine virt -cpu rv64 -smp 1 -m 1G -machine virt -device ich9-ahci,id=ahci -drive if=none,file=hdd.dmg,format=raw,id=mydisk -device ide-hd,drive=mydisk,bus=ahci.0 -device qemu-xhci,id=xhci -device usb-kbd,bus=xhci.0 -bios u-boot-dtb.bin -device loader,file=u-boot.itb,addr=0x80200000 

rm $hdd_image_name.dmg

# -s -S  -device VGA -monitor stdio
