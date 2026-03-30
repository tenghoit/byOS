# run cmd
qemu-system-x86_64 -drive format=raw,file=floppya.img,index=0,if=floppy

If Ubuntu hangs when running QEMU, hit control-alt-f2 to get to a virtual terminal.  Log in.  Find & kill the qemu process.  Hit control-alt-f7 to return to Ubuntu's graphical display.

# assembling the bootloader
nasm bootload.asm

## to see the bootloader
hexedit bootload