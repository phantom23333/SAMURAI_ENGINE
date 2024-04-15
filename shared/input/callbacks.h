#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#if !SAMURAI_SDL
void mouseCallback(GLFWwindow *window, int key, int action, int mods);
void windowFocusCallback(GLFWwindow *window, int focused);
void keyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
void characterCallback(GLFWwindow *window, unsigned int codepoint);

#endif