#ifndef VGA_H
#define VGA_H

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xB8000

static inline void vga_set_mode13h(void) {
    __asm__ volatile(
        "mov $0x13, %%ax\n"
        "int $0x10\n"
        :
        :
        : "ax"
    );
}

// Return to 80x25 text mode
static inline void vga_set_textmode(void) {
    __asm__ volatile(
        "mov $0x03, %%ax\n"
        "int $0x10\n"
        :
        :
        : "ax"
    );
}

#endif
