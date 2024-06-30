#pragma once

class Chip8 {
private:
  bool vram[64 * 32]; // Video RAM
  bool vramDirty; // Dirty flag for VRAM

  unsigned char memory[4096]; // Memory

  unsigned short stack[16]; // Stack
  unsigned short sp; // Stack pointer

  unsigned char delayTimer; // Delay timer
  unsigned char soundTimer; // Sound timer

  bool keys[16]; // Keypad

  unsigned short pc; // Program counter
  unsigned short index; // Index register

  unsigned char v[16]; // Registers

  // Helper stuff
  inline void writeMemory(unsigned short address, unsigned char value);
  inline unsigned char readMemory(unsigned short address);

  inline void writePixel(unsigned short x, unsigned short y, bool value);
  inline bool readPixel(unsigned short x, unsigned short y);

public:
  Chip8(const unsigned char *gameBinaryData, unsigned int gameBinaryDataSize);
  ~Chip8();

  int run();
};
