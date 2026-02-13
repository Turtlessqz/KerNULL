all: kernel

clean:
	rm -rf build/* isodir myos.iso

kernel:
	mkdir -p build

	i686-elf-as src/boot.s -o build/boot.o

	i686-elf-gcc -c src/kernel.c \
	-o build/kernel.o \
	-std=gnu99 -ffreestanding -O2 -Wall -Wextra

	i686-elf-gcc -c src/drivers/keyboard.c \
	-o build/keyboard.o \
	-std=gnu99 -ffreestanding -O2 -Wall -Wextra

	i686-elf-gcc -T src/linker.ld \
	-o build/kernullos \
	-ffreestanding -O2 -nostdlib \
	build/boot.o build/kernel.o build/keyboard.o \
	-lgcc

	grub-file --is-x86-multiboot build/kernullos

	mkdir -p isodir/boot/grub
	cp build/kernullos isodir/boot/kernull
	grub-mkrescue -o myos.iso isodir

run:
	qemu-system-i386 -cdrom myos.iso
