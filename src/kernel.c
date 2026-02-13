#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "drivers/keyboard.h"
#include "drivers/vga.h"
#include "drivers/keyboard.h"
#include "drivers/io.h"   // optional if keyboard.c already includes it

#define VERSION "Build: 0.3.1+130226"
#define ROOTUSER "root"
enum vga_color {                                                                /* VGA color constants */
	VGA_COLOR_BLACK = 0,                                                        /* black, blue e.t.c */
	VGA_COLOR_BLUE = 1,
	VGA_COLOR_GREEN = 2,
	VGA_COLOR_CYAN = 3,
	VGA_COLOR_RED = 4,
	VGA_COLOR_MAGENTA = 5,
	VGA_COLOR_BROWN = 6,
	VGA_COLOR_LIGHT_GREY = 7,
	VGA_COLOR_DARK_GREY = 8,
	VGA_COLOR_LIGHT_BLUE = 9,
	VGA_COLOR_LIGHT_GREEN = 10,
	VGA_COLOR_LIGHT_CYAN = 11,
	VGA_COLOR_LIGHT_RED = 12,
	VGA_COLOR_LIGHT_MAGENTA = 13,
	VGA_COLOR_LIGHT_BROWN = 14,
	VGA_COLOR_WHITE = 15,
};

static inline uint8_t vga_entry_color(enum vga_color fg, enum vga_color bg) {    /* entry color for vga buffer */
	return fg | bg << 4;
}

static inline uint16_t vga_entry(unsigned char uc, uint8_t color) {
	return (uint16_t) uc | (uint16_t) color << 8;
}

size_t strlen(const char* str) {
	size_t len = 0;
	while (str[len])
		len++;
	return len;
}

int strcmp(const char* a, const char* b) {                                      /* string comparison */
    while (*a && (*a == *b)) {
        a++;
        b++;
    }
    return *(const unsigned char*)a - *(const unsigned char*)b;
}


#define VGA_WIDTH   80
#define VGA_HEIGHT  25
#define VGA_MEMORY  0xB8000 

#define INPUT_SIZE 128
char input_buffer[INPUT_SIZE];

size_t terminal_row;
size_t terminal_column;
uint8_t terminal_color;
uint16_t* terminal_buffer = (uint16_t*)VGA_MEMORY;

void terminal_init(void){
	terminal_row = 0;
	terminal_column = 0;
	terminal_color = vga_entry_color(VGA_COLOR_LIGHT_GREY, VGA_COLOR_BLACK);
	
	for (size_t y = 0; y < VGA_HEIGHT; y++) {
		for (size_t x = 0; x < VGA_WIDTH; x++) {
			const size_t index = y * VGA_WIDTH + x;
			terminal_buffer[index] = vga_entry(' ', terminal_color);
		}
	}
}

void terminal_setcolor(uint8_t color) {
	terminal_color = color;
}

void terminal_putentryat(char c, uint8_t color, size_t x, size_t y) {
	const size_t index = y * VGA_WIDTH + x;
	terminal_buffer[index] = vga_entry(c, color);
}

void terminal_putchar(char c, uint8_t color) {
    terminal_color=color;
    if (c == '\n') {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT)
            terminal_row = 0;
        terminal_move_cursor(terminal_row, terminal_column);
        return;
    }

    if (c == '\b') {
        if (terminal_column > 0) {
            terminal_column--;
            terminal_putentryat(' ', terminal_color, terminal_column, terminal_row);
        }
        terminal_move_cursor(terminal_row, terminal_column);
        return;
    }

    terminal_putentryat(c, terminal_color, terminal_column, terminal_row);
    terminal_column++;

    if (terminal_column >= VGA_WIDTH) {
        terminal_column = 0;
        terminal_row++;
        if (terminal_row >= VGA_HEIGHT)
            terminal_row = 0;
    }
    terminal_move_cursor(terminal_row, terminal_column);
}


void terminal_write(const char* data, size_t size, uint8_t color) {
	for (size_t i = 0; i < size; i++)
		terminal_putchar(data[i], color);
}

void terminal_writestring(const char* data, bool newline, uint8_t color) {
	terminal_write(data, strlen(data), color);
    if (newline==true) {
        ++terminal_row;
        terminal_column = 0;
    }
}

void execute_command(const char* cmd) {

    if (strcmp(cmd, "help") == 0) {
        terminal_writestring("Commands:", true, VGA_COLOR_LIGHT_GREY);
        terminal_writestring("ver", true, VGA_COLOR_LIGHT_GREY);
        terminal_writestring("help", true, VGA_COLOR_LIGHT_GREY);
        terminal_writestring("clear", true, VGA_COLOR_LIGHT_GREY);
        terminal_writestring("vgamd", true, VGA_COLOR_LIGHT_GREY);
    } else if (strcmp(cmd, "clear") == 0) {
        terminal_init();
    } else if (strcmp(cmd, "ver") == 0) {
        terminal_writestring(VERSION, true, VGA_COLOR_LIGHT_GREY);
    } else if (strcmp(cmd, "vgamd 13h") == 0) {
        terminal_writestring("Switching to VGA mode (13h)...", true, VGA_COLOR_LIGHT_GREY);
    } else if (strcmp(cmd, "vgamd 03h") == 0) {
        terminal_writestring("Switching to TTY mode (03h)...", true, VGA_COLOR_LIGHT_GREY);
    } else {
        terminal_writestring("Unknown command.", true, VGA_COLOR_LIGHT_GREY);
    }
}

void shell() {
    while (1) {
        terminal_writestring(ROOTUSER, false, VGA_COLOR_GREEN);
        terminal_writestring("#> ", false, VGA_COLOR_LIGHT_GREY);

        // Reset buffer and index at start of command
        size_t index = 0;
        for (size_t i = 0; i < INPUT_SIZE; i++) {
            input_buffer[i] = '\0';
        }

        while (1) {
            char c = keyboard_getchar();
            if (!c) continue;

            if (c == '\n') {
                terminal_putchar('\n', VGA_COLOR_LIGHT_GREY);
                input_buffer[index] = '\0'; // ensure null-termination
                break;
            }

            if (c == '\b') {
                if (index > 0) {
                    index--;
                    terminal_putchar('\b', VGA_COLOR_LIGHT_GREY);
                }
                continue;
            }

            if (index < INPUT_SIZE - 1) {
                input_buffer[index++] = c;
                terminal_putchar(c, VGA_COLOR_LIGHT_GREY);
            }
        }

        // Only execute if the user typed something
        if (index > 0) {
            execute_command(input_buffer);
        }
    }
}


void kernel_main(void) {
    terminal_init();
    terminal_writestring("Booting KerNULL OS...", true, VGA_COLOR_LIGHT_BLUE);
    shell();
}
