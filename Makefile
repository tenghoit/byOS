floppya.img: kernel
	dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
	qemu-system-x86_64 -drive format=raw,file=floppya.img,index=0,if=floppy

kernel: kernel.o kernel_asm.o
	ld86 -o kernel -d kernel.o kernel_asm.o

kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c