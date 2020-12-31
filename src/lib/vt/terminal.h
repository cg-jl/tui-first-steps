#pragma once
#include <memory>
#include <optional>

namespace vt {

void init();
void end();

enum Direction {
    UP = 'A',
    DOWN = 'B',
    LEFT = 'C',
    RIGHT = 'D',
};

void move(Direction dir, unsigned int amount);
struct Coordinate {
    unsigned int x, y;
};
Coordinate getCursorPos();
Coordinate getSize();
void moveTo(unsigned int x, unsigned int y);
void moveTo(const Coordinate& coord);
using byte = unsigned char;
void setBg(byte r, byte g, byte b);
void setBg(byte svg);
void setFg(byte r, byte g, byte b);
void setFg(byte svg);
std::optional<byte> readByte();
std::optional<byte> readByte(unsigned int skip);
void eraseToEnd();
void consumeInput();

void cursor(bool show);

void putc(char c);
void flushOut();

void repeat(char c, unsigned int times);
void fill(char c);

void clearColor();
void bold();
void clearLine();
void gotoX(unsigned int x);

// equivalent to std::fputs(str, stdout)
void puts(const char* str);

void lineDown();
void lineUp();

}  // namespace vt