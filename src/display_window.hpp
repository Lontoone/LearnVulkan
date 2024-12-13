#pragma once
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <string>
namespace lontoone{
    class Window{
        public:
            Window(int w , int h , std::string name);
            ~Window();

            Window(const Window&) = delete;  //delete copy constructure
            Window &operator =(const Window &) = delete;

            bool shouldClose(){return glfwWindowShouldClose(window);}

        private : 
            void initWindow();
            const int width ;
            const int height;
            std::string windowName;

            GLFWwindow *window;
    } ;
}