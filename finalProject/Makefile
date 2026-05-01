floppya.img: bootload kernel map.img dir.img loadFile shell tui
	dd if=/dev/zero of=floppya.img bs=512 count=2880
	dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
	dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
	dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
	dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
	./loadFile shell
	./loadFile message.txt
	./loadFile tstpr2
	./loadFile tui
	

kernel: kernel.o kernel_asm.o 
	ld86 -o kernel -d kernel.o kernel_asm.o

kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c



bootload: bootload.asm
	nasm bootload.asm

loadFile: loadFile.c
	gcc -o loadFile loadFile.c


tui: tui.o lib_asm.o
	ld86 -o tui -d tui.o lib_asm.o

shell: shell.o lib_asm.o
	ld86 -o shell -d shell.o lib_asm.o

lib_asm.o: lib.asm
	as86 lib.asm -o lib_asm.o

tui.o: tui.c
	bcc -ansi -c -o tui.o tui.c

shell.o: shell.c
	bcc -ansi -c -o shell.o shell.c

