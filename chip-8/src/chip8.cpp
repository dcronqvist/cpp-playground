#include <cstring>
#include <cassert>
#include <glad/gl.h>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
#include <format>

#include "chip8.h"

Chip8::Chip8(const unsigned char *gameBinaryData, unsigned int gameBinaryDataSize) {
  // -- Initialize VRAM --
  memset(vram, 0, sizeof(vram)); // Clear VRAM
  vramDirty = true;

  // -- Initialize memory --
  assert(gameBinaryDataSize < (4096 - 512)); // Make sure game binary data fits into memory

  // Initialize memory
  std::memset(memory, 0, sizeof(memory));

  // Load game binary data into memory
  std::memcpy(memory + 512, gameBinaryData, gameBinaryDataSize);

  // Font
  unsigned char font[] = {
    0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
    0x20, 0x60, 0x20, 0x20, 0x70, // 1
    0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
    0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
    0x90, 0x90, 0xF0, 0x10, 0x10, // 4
    0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
    0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
    0xF0, 0x10, 0x20, 0x40, 0x40, // 7
    0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
    0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
    0xF0, 0x90, 0xF0, 0x90, 0x90, // A
    0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
    0xF0, 0x80, 0x80, 0x80, 0xF0, // C
    0xE0, 0x90, 0x90, 0x90, 0xE0, // D
    0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
    0xF0, 0x80, 0xF0, 0x80, 0x80  // F
  };

  // Load font into memory
  std::memcpy(memory + 0x50, font, sizeof(font));

  // -- Initialize stack --
  memset(stack, 0, sizeof(stack)); // Clear stack
  sp = 0; // Reset stack pointer

  // -- Initialize timers --
  delayTimer = 0;
  soundTimer = 0;

  // -- Initialize keys --
  memset(keys, 0, sizeof(keys)); // Clear keys

  // -- Initialize registers --
  pc = 0x200; // Program counter starts at 0x200
  index = 0; // Reset index register
  memset(v, 0, sizeof(v)); // Clear registers
}

Chip8::~Chip8() {
  memset(memory, 0, sizeof(memory)); // Clear memory, will be deallocated when object is destroyed anyway
}

inline void Chip8::writeMemory(unsigned short address, unsigned char value) {
  assert(address < 4096); // Make sure address is within memory bounds
  memory[address] = value;
}

inline unsigned char Chip8::readMemory(unsigned short address) {
  assert(address < 4096); // Make sure address is within memory bounds
  return memory[address];
}

inline void Chip8::writePixel(unsigned short x, unsigned short y, bool value) {
  if (x >= 64 || y >= 32 || x < 0 || y < 0) {
    return;
  }

  vram[y * 64 + x] = value;
  vramDirty = true;
}

inline bool Chip8::readPixel(unsigned short x, unsigned short y) {
  if (x >= 64 || y >= 32 || x < 0 || y < 0) {
    return false;
  }

  return vram[y * 64 + x];
}

static void glfwErrorCallback(int error, const char *description)
{
  fprintf(stderr, "Error: %s\n", description);
}

unsigned int shaderProgram;
unsigned int vao;
unsigned int mvpLocation;

void setupPixelDrawing() {
  // Setup pixel drawing
  const char *vertexShaderSource = R"(
    #version 330 core
    layout (location = 0) in vec2 aPos;
    uniform mat4 mvp;
    void main() {
      gl_Position = mvp * vec4(aPos.x, aPos.y, 0.0, 1.0);
    }
  )";

  const char *fragmentShaderSource = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
      FragColor = vec4(1.0, 1.0, 1.0, 1.0);
    }
  )";

  unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
  glCompileShader(vertexShader);

  unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
  glCompileShader(fragmentShader);

  shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);
  glLinkProgram(shaderProgram);

  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);

  // Quad with top left at (0, 0)
  float vertices[] = {
    // Triangle 1
    0.0F, 0.0F,
    0.0F, 1.0F,
    1.0F, 0.0F,
    // Triangle 2
    0.0F, 1.0F,
    1.0F, 1.0F,
    1.0F, 0.0F
  };

  unsigned int VBO;
  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &VBO);

  glBindVertexArray(vao);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
  glEnableVertexAttribArray(0);

  glUseProgram(shaderProgram);
  mvpLocation = glGetUniformLocation(shaderProgram, "mvp");

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);
}

void drawPixel(int x, int y, int displayScale, float r, float g, float b) {
  // Draw pixel
  glUseProgram(shaderProgram);

  // Set MVP in vertex shader
  float pixelSizeX = 2.0F / 64.0F;
  float pixelSizeY = 2.0F / 32.0F;
  float xTrans = -1.0F + (x * pixelSizeX);
  float yTrans = 1.0F - (y * pixelSizeY) - pixelSizeY;
  float mvp[16] = {
    pixelSizeX, 0.0F, 0.0F, 0.0F,
    0.0F, pixelSizeY, 0.0F, 0.0F,
    0.0F, 0.0F, 1.0F, 0.0F,
    xTrans, yTrans, 0.0F, 1.0F
  };

  glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, mvp);

  glBindVertexArray(vao);
  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);
}

void drawVRAM(const bool *vram, int displayScale) {
  // Draw VRAM
  for (int y = 0; y < 32; y++) {
    for (int x = 0; x < 64; x++) {
      if (vram[y * 64 + x]) {
        drawPixel(x, y, displayScale, 1.0F, 1.0F, 1.0F);
      }
    }
  }
}

void handleKeys(GLFWwindow *window, bool *keys) {
  // Handle key presses
  keys[0] = glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS;
  keys[1] = glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS;
  keys[2] = glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS;
  keys[3] = glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS;
  keys[4] = glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS;
  keys[5] = glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS;
  keys[6] = glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS;
  keys[7] = glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS;
  keys[8] = glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS;
  keys[9] = glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS;
  keys[10] = glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS;
  keys[11] = glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS;
  keys[12] = glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS;
  keys[13] = glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS;
  keys[14] = glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS;
  keys[15] = glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS;
}

int Chip8::run() {
  // Display configuration
  int scale = 10;

  // Display calculated dimensions and stuff
  int displayWidth = 64 * scale;
  int displayHeight = 32 * scale;

  GLFWwindow *window;
  glfwSetErrorCallback(glfwErrorCallback);

  if (!glfwInit())
    return -1;

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  window = glfwCreateWindow(displayWidth, displayHeight, "Chip-8 by @dcronqvist", NULL, NULL);
  if (!window) {
    glfwTerminate();
  }

  // Center window
  GLFWmonitor *monitor = glfwGetPrimaryMonitor();
  const GLFWvidmode *mode = glfwGetVideoMode(monitor);
  int monitorX, monitorY;
  glfwGetMonitorPos(monitor, &monitorX, &monitorY);
  int windowWidth, windowHeight;
  glfwGetWindowSize(window, &windowWidth, &windowHeight);
  glfwSetWindowPos(window, monitorX + (mode->width - windowWidth) / 2, monitorY + (mode->height - windowHeight) / 2);

  // Initialize OpenGL
  glfwMakeContextCurrent(window);
  gladLoadGL(glfwGetProcAddress);
  glfwSwapInterval(1);

  float totalTime = 0.0F;
  float deltaTime = 0.0F;
  float lastTime = 0.0F;
  float timerLastDecrement = 0.0F;

  glViewport(0, 0, displayWidth, displayHeight);

  setupPixelDrawing();

  int targetCyclesPerSecond = 60000;
  float targetCycleTime = 1.0F / targetCyclesPerSecond;

  while (!glfwWindowShouldClose(window)) {
    if (vramDirty) {
      glClearColor(0.0F, 0.0F, 0.0F, 1.0F);
      glClear(GL_COLOR_BUFFER_BIT);

      drawVRAM(vram, scale);
      vramDirty = false;

      glfwSwapBuffers(window);
    }

    totalTime = glfwGetTime();
    deltaTime = totalTime - lastTime;

    // Update timers
    if (totalTime - timerLastDecrement >= 1.0F / 60.0F) {
      if (delayTimer > 0) {
        delayTimer--;
      }
      if (soundTimer > 0) {
        soundTimer--;
      }
      timerLastDecrement = totalTime;
    }

    if (deltaTime < targetCycleTime) {
      continue;
    }

    // Handle key presses
    handleKeys(window, keys);

    // Execute one cycle

    // Fetch
    unsigned short opcode = (memory[pc] << 8) | memory[pc + 1];
    pc += 2;

    // Decode and execute
    unsigned short nibble = opcode & 0xF000;
    unsigned char x = (opcode & 0x0F00) >> 8;
    unsigned char y = (opcode & 0x00F0) >> 4;
    unsigned char n = opcode & 0x000F;
    unsigned short nn = opcode & 0x00FF;
    unsigned short nnn = opcode & 0x0FFF;

    bool keyPressed = false;

    switch (nibble) {
    case 0x0000:
      switch (nnn) {
      case 0x0E0: // 00E0 - CLS
        memset(vram, 0, sizeof(vram));
        vramDirty = true;
        break;
      case 0x0EE: // 00EE - RET
        pc = stack[sp];
        sp--;
        break;
      }
      break;

    case 0x1000: // 1NNN - JP addr
      pc = nnn;
      break;

    case 0x2000: // 2NNN - CALL addr
      sp++;
      stack[sp] = pc;
      pc = nnn;
      break;

    case 0x3000: // 3XNN - SE Vx, byte
      if (v[x] == nn) {
        pc += 2;
      }
      break;

    case 0x4000: // 4XNN - SNE Vx, byte
      if (v[x] != nn) {
        pc += 2;
      }
      break;

    case 0x5000: // 5XY0 - SE Vx, Vy
      if (v[x] == v[y]) {
        pc += 2;
      }
      break;

    case 0x6000: // 6XNN - LD Vx, byte
      v[x] = nn;
      break;

    case 0x7000: // 7XNN - ADD Vx, byte
      v[x] += nn;
      break;

    case 0x8000:
      switch (n) {
      case 0: // 0x8XY0 - LD Vx, Vy
        v[x] = v[y];
        break;
      case 1: // 0x8XY1 - OR Vx, Vy
        v[x] |= v[y];
        break;
      case 2: // 0x8XY2 - AND Vx, Vy
        v[x] &= v[y];
        break;
      case 3: // 0x8XY3 - XOR Vx, Vy
        v[x] ^= v[y];
        break;
      case 4: // 0x8XY4 - ADD Vx, Vy
        v[0xF] = (((int)v[x] + (int)v[y]) > 255) ? 1 : 0;
        v[x] += v[y];
        break;
      case 5: // 0x8XY5 - SUB Vx, Vy
        v[0xF] = (v[x] > v[y]) ? 1 : 0;
        v[x] -= v[y];
        break;
      case 6: // 0x8XY6 - SHR Vx {, Vy}
        v[0xF] = v[x] & 0x1;
        v[x] >>= 1;
        break;
      case 7: // 0x8XY7 - SUBN Vx, Vy
        v[0xF] = (v[y] > v[x]) ? 1 : 0;
        v[x] = v[y] - v[x];
        break;
      case 0xE: // 0x8XYE - SHL Vx {, Vy}
        v[0xF] = (v[x] & 0x80) >> 7;
        v[x] <<= 1;
        break;
      }
      break;

    case 0x9000: // 9XY0 - SNE Vx, Vy
      if (v[x] != v[y]) {
        pc += 2;
      }
      break;

    case 0xA000: // ANNN - LD I, addr
      index = nnn;
      break;

    case 0xB000: // BNNN - JP V0, addr
      pc = v[0] + nnn;
      break;

    case 0xC000: // CXNN - RND Vx, byte
      v[x] = (rand() & 255) & nn;
      break;

    case 0xD000: // DXYN - DRW Vx, Vy, nibble
      v[0xF] = 0;

      for (int yLine = 0; yLine < n; yLine++) {
        const unsigned char pixel = memory[index + yLine];
        for (int xLine = 0; xLine < 8; xLine++) {
          if ((pixel & (0x80 >> xLine)) != 0) {
            if (readPixel((v[x] & 63) + xLine, (v[y] & 31) + yLine)) {
              v[0xF] = 1;
            }
            writePixel((v[x] & 63) + xLine, (v[y] & 31) + yLine, !readPixel((v[x] & 63) + xLine, (v[y] & 31) + yLine));
          }
        }
      }

      break;

    case 0xE000:
      switch (nn) {
      case 0x9E: // EX9E - SKP Vx
        if (keys[v[x]]) {
          pc += 2;
        }
        break;
      case 0xA1: // EXA1 - SKNP Vx
        if (!keys[v[x]]) {
          pc += 2;
        }
        break;
      }
      break;
    case 0xF000:
      switch (nn) {
      case 0x07: // FX07 - LD Vx, DT
        v[x] = delayTimer;
        break;
      case 0x15: // FX15 - LD DT, Vx
        delayTimer = v[x];
        break;
      case 0x18: // FX18 - LD ST, Vx
        soundTimer = v[x];
        break;
      case 0x1E: // FX1E - ADD I, Vx
        index += v[x];
        v[0xF] = (index > 0xFFF) ? 1 : 0;
        break;
      case 0x0A: // FX0A - Get key
        for (int i = 0; i < 0xF; i++) {
          if (keys[i]) {
            v[x] = i;
            keyPressed = true;
            break;
          }
        }
        if (!keyPressed)
          pc -= 2;
        break;
      case 0x29: // FX29 - LD F, Vx
        index = 0x50 + (v[x] * 5);
        break;
      case 0x33: // FX33 - LD B, Vx
        memory[index] = v[x] / 100;
        memory[index + 1] = (v[x] / 10) % 10;
        memory[index + 2] = v[x] % 10;
        break;
      case 0x55: // FX55 - LD [I], Vx
        for (int i = 0; i <= x; i++) {
          memory[index + i] = v[i];
        }
        break;
      case 0x65: // FX65 - LD Vx, [I]
        for (int i = 0; i <= x; i++) {
          v[i] = memory[index + i];
        }
        break;
      }
      break;
    }

    lastTime = totalTime;
    glfwPollEvents();
    glfwSetWindowTitle(window, std::format("Chip-8 by @dcronqvist - {:} DT, {:} ST", delayTimer, soundTimer).c_str());
  }

  glfwDestroyWindow(window);
  glfwTerminate();
  return 0;
}
