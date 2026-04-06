# run cmd
qemu-system-x86_64 -drive format=raw,file=floppya.img,index=0,if=floppy

If Ubuntu hangs when running QEMU, hit control-alt-f2 to get to a virtual terminal.  Log in.  Find & kill the qemu process.  Hit control-alt-f7 to return to Ubuntu's graphical display.

# assembling the bootloader
nasm bootload.asm

## to see the bootloader
hexedit bootload

# compiling kernel

bcc -ansi -c -o kernel.o kernel.c

# assemble kernel.asm

as86 kernel.asm -o kernel_asm.o

# linking

ld86 -o kernel -d kernel.o kernel_asm.o

# kernel to img

dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3