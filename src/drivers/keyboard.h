#ifndef KEYBOARD_H
#define KEYBOARD_H

char keyboard_getchar(void);
void terminal_move_cursor(size_t row, size_t column);
void terminal_hide_cursor(void);
void terminal_show_cursor(void);
static inline void outb(uint16_t port, uint8_t val);

#endif
