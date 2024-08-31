#ifndef WINDOW_H
#define WINDOW_H

#include <GLFW/glfw3.h>

GLFWwindow* initWindow();
void terminateWindow();
void processInput(GLFWwindow* window);
void updateGame(float deltaTime);
void renderObject();
void renderGame();

#endif // WINDOW_H
