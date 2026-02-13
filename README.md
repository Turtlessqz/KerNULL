# KerNULL OS

KerNULL OS is a minimal x86 hobby operating system written in C, featuring a basic shell, VGA text mode, and framebuffer graphics capabilities.  

---

## Features

- 80x25 VGA text terminal with color support  
- Basic shell with command execution  
- Keyboard input handling  
- Simple framebuffer graphics mode (draw pixels, rectangles)  
- Commands:
  - `help` — list available commands  
  - `clear` — clear the framebuffer  
  - `red`, `green`, `blue` — draw colored rectangles  

---

## Build Instructions

You need an **i686-elf toolchain** (GCC + binutils) installed. On Arch Linux, you can use:

```bash
sudo pacman -S base-devel i686-elf-gcc i686-elf-binutils qemu grub
