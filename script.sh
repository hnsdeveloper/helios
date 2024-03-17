kernel_image=$1
hdd_image_name=hdd

if [[ -f "$hdd_image_name.dmg" ]]; then  
    rm "$hdd_image_name.dmg"
fi

if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    touch "$hdd_image_name.dmg"
    dd if=/dev/zero of="$hdd_image_name.dmg" bs=1M count=1
elif [[ "$OSTYPE" == "darwin"* ]]; then
    hdiutil create -size 1024m -fs fat32 -volname boot $hdd_image_name
else  
    exit
fi

qemu-system-riscv64 -machine virt -cpu rv64 -smp 1 -m 1G -machine virt -bios none  -device ich9-ahci,id=ahci -drive if=none,file=hdd.dmg,format=raw,id=mydisk -device ide-hd,drive=mydisk,bus=ahci.0 -monitor stdio -device qemu-xhci,id=xhci -device usb-kbd,bus=xhci.0 -kernel $kernel_image

# -bios u-boot-spl-dtb.bin -device loader,file=u-boot.itb,addr=0x80200000 -device VGA -serial stdio 