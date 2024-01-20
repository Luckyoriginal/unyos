SRC_DIR := src
ARCH := i386
BUILD_DIR := build
C_FLAGS := -ffreestanding -O2 -Wall -Wextra -fno-exceptions
LD_FLAGS := -O2 -nostdlib
# Liste de fichiers source
SRC_FILES := src/core/kernel.c src/core/tty/tty.c src/vga/i386/vga.c src/disk/ata_pio/disk.c src/keyboard/8042/keyboard.c src/time/i386/time.c src/memory/memory.c
BOOTLOADER := src/bootloader/i386/boot
# Transformer les fichiers source en cibles de compilation
OBJ_FILES := $(patsubst %.c, %.o, $(SRC_FILES))

# Règle générique de compilation
%.o: %.c
	$(CC) -m32 -c $< -o $@ $(C_FLAGS)

# Règle pour construire tous les fichiers objets
all: $(OBJ_FILES) kasm.o kernel 

# Utilisation de la fonction foreach pour créer des règles individuelles
$(foreach src,$(SRC_FILES),$(eval $(src:.c=.o): $(src)))

#disque 
f32.disk: 
	dd if=/dev/zero of=f32.disk bs=1M count=100
	sudo mkfs.fat -F32 f32.disk -s 1

$(BOOTLOADER).o: $(BOOTLOADER).asm
	nasm -f elf32 $< -o $@ 

kernel: $(OBJ_FILES) $(BOOTLOADER).o link.ld
	ld -m elf_i386 -T link.ld -o kernel $(OBJ_FILES) $(BOOTLOADER).o $(LD_FLAGS)

run: all f32.disk
	qemu-system-i386 -kernel kernel -drive file=f32.disk,format=raw -m size=4096 --enable-kvm
clean:
	rm -f $(OBJ_FILES)

.PHONY: all clean

