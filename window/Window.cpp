#include <GLFW/glfw3.h>
#include <iostream>
#include "window.h"

static const int WIDTH = 800;
static const int HEIGHT = 600;

GLFWwindow* initWindow() {
    if (!glfwInit()) {
        std::cerr << "Ошибка инициализации GLFW" << std::endl;
        return nullptr;
    }

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Окно", nullptr, nullptr);
    if (!window) {
        std::cerr << "Ошибка создания окна" << std::endl;
        glfwTerminate();
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    return window;
}

void terminateWindow() {
    glfwTerminate();
}

void processInput(GLFWwindow* window) {
    // Проверка нажатия клавиши ESC W S A D для выхода из программы
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void updateGame(float deltaTime) {
    // обновления игры
}


void renderObject() {
    // отрисовки объектов
}

void renderGame() {
    // отрисовки игры
}
