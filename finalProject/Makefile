floppya.img: bootload kernel map.img dir.img loadFile cli tui
	dd if=/dev/zero of=floppya.img bs=512 count=2880
	dd if=bootload of=floppya.img bs=512 count=1 conv=notrunc
	dd if=map.img of=floppya.img bs=512 count=1 seek=1 conv=notrunc
	dd if=dir.img of=floppya.img bs=512 count=1 seek=2 conv=notrunc
	dd if=kernel of=floppya.img bs=512 conv=notrunc seek=3
	./loadFile cli
	./loadFile message.txt
	./loadFile tstpr2
	./loadFile tui
	
##########################################################################################

bootload: bootload.asm
	nasm bootload.asm

loadFile: loadFile.c
	gcc -o loadFile loadFile.c

lib_asm.o: lib.asm
	as86 lib.asm -o lib_asm.o

kernel_asm.o: kernel.asm
	as86 kernel.asm -o kernel_asm.o

##########################################################################################

kernel: kernel.o kernel_asm.o math.o string.o sector.o directory.o file.o
	ld86 -o kernel -d kernel.o kernel_asm.o math.o string.o sector.o directory.o file.o

cli: cli.o lib_asm.o shell.o history.o string.o
	ld86 -o cli -d cli.o lib_asm.o shell.o history.o string.o

tui: tui.o lib_asm.o shell.o history.o string.o
	ld86 -o tui -d tui.o lib_asm.o shell.o history.o string.o

##########################################################################################

math.o: math.c math.h
	bcc -ansi -c -o math.o math.c

string.o: string.c string.h
	bcc -ansi -c -o string.o string.c

sector.o: sector.c sector.h math.h
	bcc -ansi -c -o sector.o sector.c

directory.o: directory.c directory.h sector.h string.h 
	bcc -ansi -c -o directory.o directory.c

file.o: file.c file.h directory.h sector.h string.h math.h
	bcc -ansi -c -o file.o file.c

kernel.o: kernel.c
	bcc -ansi -c -o kernel.o kernel.c

history.o: history.c history.h string.h
	bcc -ansi -c -o history.o history.c

shell.o: shell.c string.h
	bcc -ansi -c -o shell.o shell.c

cli.o: cli.c shell.h history.h string.h
	bcc -ansi -c -o cli.o cli.c

tui.o: tui.c shell.h history.h string.h
	bcc -ansi -c -o tui.o tui.c