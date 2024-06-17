#ifndef __ImageGenerate_H__
#define __ImageGenerate_H__
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stdlib.h>
#include <stdio.h>
extern int DrawShape(char title[], int dSigIndex, int sSigIndex);
static void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
static void ErrorCallback(int error, const char* description);
#endif // !__ImageGenerate_H__
