#include <GLFW/glfw3.h>

int main() {
    if (!glfwInit())
        return -1;
    
    GLFWwindow* window = glfwCreateWindow(640, 480, "OpenGL", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }
    
    glfwMakeContextCurrent(window);
    
    while (!glfwWindowShouldClose(window)) {
        // 渲染代码
        
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    
    glfwTerminate();
    return 0;
}