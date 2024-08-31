#include "window.h"

int main() {
    // Инициализация окна
    GLFWwindow* window = initWindow();
    if (!window) {
        return -1;  // Если окно не инициализировалось, завершить программу
    }

    float lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;

        // Обработка ввода
        processInput(window);

        // Обновление игрового процесса
        updateGame(deltaTime);

        // Отрисовка объектов
        renderObject();

        // Отрисовка игры
        renderGame();

        // Обработка событий и смена буферов
        glfwPollEvents();
        glfwSwapBuffers(window);
    }

    // Завершение работы
    terminateWindow();
    return 0;
}
