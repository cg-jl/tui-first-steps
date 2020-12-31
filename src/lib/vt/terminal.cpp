#include "terminal.h"

#include <cstdio>
#include <cstdlib>
extern "C" {
#include <stdarg.h>
#include <termios.h>
#include <unistd.h>
}

namespace vt {
struct termios t_base;
bool n_init = false;
void init() {
    if (n_init) return;
    tcgetattr(STDIN_FILENO, &t_base);
    termios raw = t_base;

    raw.c_iflag &= ~(BRKINT | INPCK | ISTRIP | ICRNL | IXON);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cflag |= CS8;

    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);
}

void end() {
    if (!n_init) return;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &t_base);
}

Coordinate getCursorPos() {
    vt::init();
    std::fflush(stdin);
    vt::puts("\x1b[6n");
    std::fflush(stdout);
    unsigned int y, x;
    scanf("\x1b[%u;%uR", &y, &x);
    std::fflush(stdin);

    return {x, y};
}

void moveTo(unsigned int x, unsigned int y) {
    printf("\x1b[%u;%uH", y, x);
    // std::fflush(stdout);
}

void moveTo(const Coordinate& coord) {
    const auto& [x, y] = coord;
    moveTo(x, y);
}

void puts(const char* str) { fputs(str, stdout); }

void move(Direction dir, unsigned int amount) {
    if (amount == 0) return;
    std::printf("\x1b[%u%c", amount, (char)dir);
}

Coordinate getSize() {
    const auto current = getCursorPos();
    moveTo(10000, 10000);
    std::fflush(stdout);
    const auto size = getCursorPos();
    moveTo(current);
    std::fflush(stdout);
    return size;
}

void setBg(byte r, byte g, byte b) { printf("\x1b[48;2;%u;%u;%um", r, g, b); }
void setBg(byte svg) { printf("\x1b[48;5;%um", svg); }
void setFg(byte r, byte g, byte b) { printf("\x1b[38;2;%u;%u;%um", r, g, b); }
void setFg(byte svg) { printf("\x1b[38;5;%um", svg); }

void eraseToEnd() { vt::puts("\x1b[J"); }

std::optional<unsigned char> readByte() {
    vt::init();
    char b;
    if (read(STDIN_FILENO, &b, 1) == 1) return b;
    return {};
}

std::optional<byte> readByte(unsigned int skip) {
    byte c;
    for (unsigned int i = 0; i < skip; ++i) {
        const auto& opt = readByte();
        if (!opt)
            return {};  // if there's no char before, then there will be no char
                        // after }
        c = *opt;
    }
    return c;
}

void consumeInput() { std::fflush(stdin); }

void cursor(bool show) {
    if (show)
        vt::puts("\x1b[?25h");
    else
        vt::puts("\x1b[?25l");
}

void putc(char c) { std::putc(c, stdout); }
void flushOut() { std::fflush(stdout); }

void repeat(char c, unsigned int times) {
    if (times == 0) return;
    for (unsigned int i = 0; i < times; ++i) vt::putc(c);
}

void clearColor() { vt::puts("\x1b[0m"); }
void bold() { vt::puts("\x1b[1m"); }
void clearLine() { vt::puts("\x1b[2K"); }
void gotoX(unsigned int x) { std::printf("\x1b[%uG", x); }
void lineDown() { vt::puts("\r\x1b[1B"); }
void lineUp() { vt::puts("\r\x1b[1A"); }

}  // namespace vt