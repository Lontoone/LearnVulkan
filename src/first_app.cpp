#include "first_app.hpp"

namespace lontoone{
    void FirstApp::run(){
        while (!window.shouldClose())
        {
            glfwPollEvents();
        }
        
    }
}