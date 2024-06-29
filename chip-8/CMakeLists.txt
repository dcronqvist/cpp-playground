project(chip-8 VERSION 1.0.0 LANGUAGES CXX C)

set(CHIP8_SOURCES
  # GLAD
  third-party/glad/src/gl.c

  # CHIP-8
  src/main.cpp
)

add_executable(chip-8 ${CHIP8_SOURCES})
target_include_directories(chip-8 PRIVATE include)

# GLAD
target_include_directories(chip-8 PRIVATE third-party/glad/include)

# GLFW
add_subdirectory(third-party/glfw)
target_include_directories(chip-8 PRIVATE third-party/glfw/include)
target_link_libraries(chip-8 PRIVATE glfw)
